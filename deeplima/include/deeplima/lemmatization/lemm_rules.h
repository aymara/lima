// Copyright 2026 CEA LIST
// SPDX-FileCopyrightText: 2026 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_LEMMATIZATION_LEMM_RULES_H
#define DEEPLIMA_LEMMATIZATION_LEMM_RULES_H

/**
 * Edit rules for lemmatizing words absent from the lemma dictionary.
 *
 * A suffix rule turns a form into its lemma: optionally lowercase the form,
 * strip m_strip trailing characters, append m_add ("chantera" -> "chanter" is
 * {false, 1, ""}, "chevaux" -> "cheval" is {false, 2, "l"}). Rules are
 * learned from a treebank by deeplima-gen-lemm-dict --rules, indexed by the
 * word's UPOS (+FEATS) and lowercased ending, and applied by
 * TokenSequenceAnalyzer after a dictionary miss, before the seq2seq model.
 *
 * Optionally (--rules-max-prefix), a prefix rule edits the beginning of the
 * word the same way, indexed by its lowercased beginning: strip m_strip leading
 * characters, prepend m_add. Suffix rules alone cannot lemmatize the languages
 * that change the start of words (Irish "bhean" -> "bean", Indonesian
 * "membaca" -> "baca", Hebrew and Arabic prefixes). Prefix rules are learned
 * along the longest common substring of form and lemma, the suffix rules along
 * the longest common prefix: a word gets a prefix edit only when that makes its
 * whole edit shorter, and within max_prefix characters.
 *
 * Unlike a character seq2seq model, a rule cannot scramble letters and costs a
 * few hash lookups. On UD_French-Sequoia it lemmatizes the unseen forms better
 * than the seq2seq model did (87% vs 72% with gold tags).
 *
 * File format: UTF-8, '#' starts a comment line, one rule per line, 7
 * tab-separated fields for a suffix rule (suffix and add may be empty):
 *   upos  feats  suffix  lower  strip  add  count
 * and 8 for a prefix rule, whose lower is always 0 (lowercasing belongs to the
 * suffix rule, applied before both edits):
 *   prefix  upos  feats  beginning  lower  strip  add  count
 * feats is the FEATS column ("_" for none) or "*" for the rule used when no
 * rule matches the exact features; suffix and beginning are lowercased; lower
 * is 0 or 1; count is how many training words support the rule (it breaks ties
 * when two spellings of the same features collide at load time). The prefix
 * lines have their own field count so that a reader predating them rejects the
 * file instead of applying them as suffix rules.
 *
 * All lengths are in code points, and lowercasing is ICU simple case mapping,
 * code point by code point, so that it never changes a length the rules
 * depend on.
 */

#include <algorithm>
#include <cstdint>
#include <functional>
#include <istream>
#include <map>
#include <ostream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

#include <unicode/uchar.h>
#include <unicode/unistr.h>

namespace deeplima
{
namespace lemmatization
{

/** Lowercases code point by code point (ICU simple case mapping, any script). */
inline std::u32string to_lower(const std::u32string& src)
{
  std::u32string result(src);
  for (char32_t& c : result)
  {
    c = static_cast<char32_t>(u_tolower(static_cast<UChar32>(c)));
  }
  return result;
}

inline std::u32string utf8_to_u32(const std::string& src)
{
  const icu::UnicodeString u = icu::UnicodeString::fromUTF8(src);
  std::u32string result(static_cast<size_t>(u.countChar32()), U'\0');
  UErrorCode status = U_ZERO_ERROR;
  u.toUTF32(reinterpret_cast<UChar32*>(result.data()), static_cast<int32_t>(result.size()), status);
  if (U_FAILURE(status))
  {
    throw std::runtime_error("utf8_to_u32: can't convert \"" + src + "\"");
  }
  return result;
}

inline std::string u32_to_utf8(const std::u32string& src)
{
  const icu::UnicodeString u = icu::UnicodeString::fromUTF32(
      reinterpret_cast<const UChar32*>(src.data()), static_cast<int32_t>(src.size()));
  std::string result;
  u.toUTF8String(result);
  return result;
}

struct edit_rule_t
{
  bool m_lower = false;
  uint32_t m_strip = 0;
  std::u32string m_add;

  bool operator==(const edit_rule_t& other) const
  {
    return m_lower == other.m_lower && m_strip == other.m_strip && m_add == other.m_add;
  }

  /** Simplest first: fewer characters stripped, then fewer added. */
  bool operator<(const edit_rule_t& other) const
  {
    return std::make_tuple(m_strip, m_add.size(), m_add, m_lower)
         < std::make_tuple(other.m_strip, other.m_add.size(), other.m_add, other.m_lower);
  }
};

/**
 * The rule turning form into lemma that keeps the longest common prefix:
 * lowercasing is part of the rule only when it shortens the edit
 * ("Princes" -> "prince" is {true, 1, ""}, "Paris" -> "Paris" is {false, 0, ""}).
 */
inline edit_rule_t extract_rule(const std::u32string& form, const std::u32string& lemma)
{
  auto make = [&lemma](const std::u32string& f, bool lower)
  {
    size_t p = 0;
    while (p < f.size() && p < lemma.size() && f[p] == lemma[p])
    {
      ++p;
    }
    return edit_rule_t{lower, static_cast<uint32_t>(f.size() - p), lemma.substr(p)};
  };
  const edit_rule_t keep = make(form, false);
  const edit_rule_t lower = make(to_lower(form), true);
  return (std::make_pair(lower.m_strip, lower.m_add.size())
          < std::make_pair(keep.m_strip, keep.m_add.size())) ? lower : keep;
}

/** A whole word's edit: a prefix rule (never lowercasing) and a suffix rule. */
struct affix_rule_t
{
  edit_rule_t m_prefix;
  edit_rule_t m_suffix;
};

/** The length of the edit: characters stripped and added at both ends. */
inline size_t edit_cost(const affix_rule_t& r)
{
  return r.m_prefix.m_strip + r.m_prefix.m_add.size() + r.m_suffix.m_strip + r.m_suffix.m_add.size();
}

/**
 * The edit turning form into lemma around their longest common substring, the
 * earliest one in form, then in lemma, on equal lengths.
 */
inline affix_rule_t extract_substring_rule(const std::u32string& form, const std::u32string& lemma,
                                           bool lower)
{
  // Longest common substring, by dynamic programming on these short strings
  size_t best = 0, best_i = 0, best_j = 0;
  std::vector<size_t> previous(lemma.size() + 1, 0), current(lemma.size() + 1, 0);
  for (size_t i = 1; i <= form.size(); ++i)
  {
    for (size_t j = 1; j <= lemma.size(); ++j)
    {
      current[j] = form[i - 1] == lemma[j - 1] ? previous[j - 1] + 1 : 0;
      // Strictly longer only: on equal lengths, the earliest in form, then in
      // lemma, stays
      if (current[j] > best)
      {
        best = current[j];
        best_i = i - best;
        best_j = j - best;
      }
    }
    std::swap(previous, current);
  }
  affix_rule_t r;
  if (best == 0)
  {
    // Nothing in common: replace the whole word
    r.m_suffix = edit_rule_t{lower, static_cast<uint32_t>(form.size()), lemma};
    return r;
  }
  r.m_prefix = edit_rule_t{false, static_cast<uint32_t>(best_i), lemma.substr(0, best_j)};
  r.m_suffix = edit_rule_t{lower, static_cast<uint32_t>(form.size() - best_i - best),
                           lemma.substr(best_j + best)};
  return r;
}

/**
 * The rule turning form into lemma with a prefix edit stripping at most
 * max_prefix characters: the suffix rule of extract_rule unless an edit around
 * the longest common substring is strictly shorter. max_prefix 0 gives the
 * suffix rule alone.
 */
inline affix_rule_t extract_affix_rule(const std::u32string& form, const std::u32string& lemma,
                                       size_t max_prefix)
{
  affix_rule_t best;
  best.m_suffix = extract_rule(form, lemma);
  if (max_prefix == 0)
  {
    return best;
  }
  for (bool lower : {false, true})
  {
    const affix_rule_t r = extract_substring_rule(lower ? to_lower(form) : form, lemma, lower);
    if (r.m_prefix.m_strip <= max_prefix && edit_cost(r) < edit_cost(best))
    {
      best = r;
    }
  }
  return best;
}

/**
 * Applies the prefix and suffix rules to form. False when they do not apply
 * (they strip more than the form) or would give an empty lemma.
 */
inline bool apply_rule(const std::u32string& form, const edit_rule_t& prefix, const edit_rule_t& suffix,
                       std::u32string& lemma)
{
  if (static_cast<size_t>(prefix.m_strip) + suffix.m_strip > form.size())
  {
    return false;
  }
  const std::u32string word = suffix.m_lower ? to_lower(form) : form;
  std::u32string result = prefix.m_add;
  result.append(word, prefix.m_strip, word.size() - prefix.m_strip - suffix.m_strip);
  result += suffix.m_add;
  if (result.empty())
  {
    return false;
  }
  lemma = std::move(result);
  return true;
}

/** Applies a suffix rule alone (see above). */
inline bool apply_rule(const std::u32string& form, const edit_rule_t& rule, std::u32string& lemma)
{
  return apply_rule(form, edit_rule_t{}, rule, lemma);
}

/** The feats value of the rule used when no rule matches the exact features. */
inline const std::string& any_feats()
{
  static const std::string s("*");
  return s;
}

/**
 * Learns rules from (form, UPOS, FEATS, lemma) observations and writes them.
 * With max_prefix 0, suffix rules only, and the file is what it was before
 * prefix rules existed.
 *
 * Callers add each distinct observation once (a rule's count is a number of
 * word types, not tokens: a frequent irregular word must not outvote the
 * regular paradigm) and preferably only rare words, which resemble the words
 * the rules will meet: words missing from the dictionary.
 *
 * A rule is indexed under endings at least as long as what it strips, so that
 * "lisait" -> "lire" (strip "sait") is never applied on the strength of "-ait"
 * alone ("utilisait" would give "utilire"). Likewise, a prefix rule is indexed
 * under beginnings at least as long as what it strips.
 */
class lemm_rules_builder_t
{
public:
  explicit lemm_rules_builder_t(size_t max_suffix = 6, size_t max_prefix = 0)
    : m_max_suffix(max_suffix), m_max_prefix(max_prefix) {}

  void add(const std::string& upos, const std::string& feats,
           const std::u32string& form, const std::u32string& lemma)
  {
    if (form.empty() || lemma.empty())
    {
      return;
    }
    const affix_rule_t rule = extract_affix_rule(form, lemma, m_max_prefix);
    const std::u32string low = to_lower(form);
    for (const std::string* f : {&feats, &any_feats()})
    {
      for (size_t n = rule.m_suffix.m_strip; n <= std::min(m_max_suffix, low.size()); ++n)
      {
        m_counts[std::make_tuple(upos, *f, low.substr(low.size() - n))][rule.m_suffix]++;
      }
      // Without prefix rules, nothing: the identity everywhere
      for (size_t n = rule.m_prefix.m_strip; m_max_prefix > 0 && n <= std::min(m_max_prefix, low.size()); ++n)
      {
        m_prefix_counts[std::make_tuple(upos, *f, low.substr(0, n))][rule.m_prefix]++;
      }
    }
  }

  /**
   * The most supported rule of each (UPOS, FEATS, ending), then of each (UPOS,
   * FEATS, beginning); ties go to the simplest rule.
   */
  void write(std::ostream& out) const
  {
    out << "# deeplima lemmatization rules\n"
        << "# upos\tfeats\tsuffix\tlower\tstrip\tadd\tcount\n";
    write(out, m_counts, "");
    if (!m_prefix_counts.empty())
    {
      out << "# prefix\tupos\tfeats\tbeginning\tlower\tstrip\tadd\tcount\n";
      write(out, m_prefix_counts, "prefix\t");
    }
  }

private:
  typedef std::map<std::tuple<std::string, std::string, std::u32string>, std::map<edit_rule_t, size_t>> counts_t;

  static void write(std::ostream& out, const counts_t& counts, const char* line_start)
  {
    for (const auto& [key, rules] : counts)
    {
      const auto best = std::max_element(rules.begin(), rules.end(),
                                         [](const auto& a, const auto& b)
                                         {
                                           // max_element: b wins if a < b; on equal
                                           // counts the simpler rule must win
                                           return a.second < b.second
                                               || (a.second == b.second && b.first < a.first);
                                         });
      out << line_start
          << std::get<0>(key) << '\t' << std::get<1>(key) << '\t' << u32_to_utf8(std::get<2>(key)) << '\t'
          << (best->first.m_lower ? 1 : 0) << '\t' << best->first.m_strip << '\t'
          << u32_to_utf8(best->first.m_add) << '\t' << best->second << '\n';
    }
  }

  size_t m_max_suffix;
  size_t m_max_prefix;
  counts_t m_counts;
  counts_t m_prefix_counts;
};

/** One line of a rules file, as read_rules() hands it over. */
struct rule_line_t
{
  bool m_prefix = false;     // a prefix rule, m_suffix then being the word's beginning
  std::string m_upos;
  std::string m_feats;
  std::u32string m_suffix;
  edit_rule_t m_rule;
  size_t m_count = 0;
};

/** Splits on tabs, keeping empty fields (utils::split drops them). */
inline std::vector<std::string> split_tabs(const std::string& line)
{
  std::vector<std::string> fields;
  size_t start = 0;
  while (true)
  {
    const size_t end = line.find('\t', start);
    fields.push_back(line.substr(start, end == std::string::npos ? std::string::npos : end - start));
    if (end == std::string::npos)
    {
      return fields;
    }
    start = end + 1;
  }
}

/** Reads a rules file, calling fn(const rule_line_t&) on each rule. Throws on a malformed line. */
inline void read_rules(std::istream& in, const std::function<void(const rule_line_t&)>& fn)
{
  std::string line;
  size_t line_no = 0;
  while (std::getline(in, line))
  {
    ++line_no;
    if (!line.empty() && line.back() == '\r')
    {
      line.pop_back();
    }
    if (line.empty() || line[0] == '#')
    {
      continue;
    }
    std::vector<std::string> f = split_tabs(line);
    auto fail = [&](const std::string& why)
    {
      throw std::runtime_error("Lemmatization rules line " + std::to_string(line_no)
                               + ": " + why + ": \"" + line + "\"");
    };
    rule_line_t r;
    if (f.size() == 8 && f[0] == "prefix")
    {
      r.m_prefix = true;
      f.erase(f.begin());
    }
    else if (f.size() != 7)
    {
      fail("expected 7 tab-separated fields (8 for a prefix rule)");
    }
    if (f[0].empty() || f[1].empty() || (f[3] != "0" && f[3] != "1") || (r.m_prefix && f[3] != "0"))
    {
      fail("bad upos, feats or lower field");
    }
    r.m_upos = f[0];
    r.m_feats = f[1];
    r.m_suffix = utf8_to_u32(f[2]);
    r.m_rule.m_lower = f[3] == "1";
    r.m_rule.m_add = utf8_to_u32(f[5]);
    try
    {
      r.m_rule.m_strip = static_cast<uint32_t>(std::stoul(f[4]));
      r.m_count = std::stoul(f[6]);
    }
    catch (const std::logic_error&)
    {
      fail("bad strip or count field");
    }
    fn(r);
  }
}

/**
 * Rules ready for lookup, keyed by K, the analyzer's encoding of UPOS(+FEATS)
 * (morph_feats_t in TokenSequenceAnalyzer, strings in tests).
 *
 * Exact-features rules and UPOS-only fallback rules are kept apart, for the
 * suffixes and the beginnings alike: in the analyzer a word without features
 * ("NOUN _") and the fallback ("NOUN *") encode to the same key, and sharing
 * one table let the fallback overwrite the exact rule.
 */
template <typename K, typename KHash = std::hash<K>>
class lemm_rules_t
{
public:
  /**
   * Adds a rule to the exact or the fallback table, of the suffixes or of the
   * beginnings (prefix, affix then being the word's beginning); of two rules
   * under the same key, the most supported one stays.
   */
  void add(const K& key, const std::u32string& affix, const edit_rule_t& rule, size_t count,
           bool fallback, bool prefix = false)
  {
    tables_t& tables = prefix ? m_prefix : m_suffix;
    table_t& table = fallback ? tables.m_fallback : tables.m_exact;
    auto [it, inserted] = table.try_emplace(std::make_pair(key, affix), entry_t{rule, count});
    if (!inserted && count > it->second.m_count)
    {
      it->second = entry_t{rule, count};
    }
    tables.m_max_length = std::max(tables.m_max_length, affix.size());
  }

  bool empty() const { return m_suffix.empty(); }
  size_t size() const { return m_suffix.size() + m_prefix.size(); }

  /**
   * Lemmatizes form with the rule of its longest known ending, the exact
   * features before the UPOS-only fallback at equal length, and then the rule
   * of its longest known beginning likewise (none: the beginning is kept). A
   * rule stripping more than the form leaves is passed over for a shorter key.
   */
  bool lemmatize(const K& exact, const K& fallback, const std::u32string& form, std::u32string& lemma) const
  {
    if (empty())
    {
      return false;
    }
    const std::u32string low = to_lower(form);
    for (size_t n = std::min(m_suffix.m_max_length, low.size()) + 1; n-- > 0; )
    {
      const std::u32string suffix = low.substr(low.size() - n);
      for (const table_t* table : {&m_suffix.m_exact, &m_suffix.m_fallback})
      {
        const edit_rule_t* rule = find(*table, table == &m_suffix.m_exact ? exact : fallback, suffix);
        if (nullptr != rule && rule->m_strip <= form.size()
            && apply_rule(form, find_prefix(exact, fallback, low, form.size() - rule->m_strip), *rule, lemma))
        {
          return true;
        }
      }
    }
    return false;
  }

private:
  struct entry_t
  {
    edit_rule_t m_rule;
    size_t m_count;
  };
  struct key_hash
  {
    size_t operator()(const std::pair<K, std::u32string>& k) const
    {
      return KHash()(k.first) ^ (std::hash<std::u32string>()(k.second) * 31);
    }
  };

  typedef std::unordered_map<std::pair<K, std::u32string>, entry_t, key_hash> table_t;

  struct tables_t
  {
    table_t m_exact;
    table_t m_fallback;
    size_t m_max_length = 0;   // of the longest affix

    bool empty() const { return m_exact.empty() && m_fallback.empty(); }
    size_t size() const { return m_exact.size() + m_fallback.size(); }
  };

  static const edit_rule_t* find(const table_t& table, const K& key, const std::u32string& affix)
  {
    const auto it = table.find(std::make_pair(key, affix));
    return table.end() == it ? nullptr : &it->second.m_rule;
  }

  /** The prefix rule of low's longest known beginning stripping at most max_strip characters. */
  edit_rule_t find_prefix(const K& exact, const K& fallback, const std::u32string& low, size_t max_strip) const
  {
    for (size_t n = std::min(m_prefix.m_max_length, low.size()) + 1; !m_prefix.empty() && n-- > 0; )
    {
      const std::u32string beginning = low.substr(0, n);
      for (const table_t* table : {&m_prefix.m_exact, &m_prefix.m_fallback})
      {
        const edit_rule_t* rule = find(*table, table == &m_prefix.m_exact ? exact : fallback, beginning);
        if (nullptr != rule && rule->m_strip <= max_strip)
        {
          return *rule;
        }
      }
    }
    return edit_rule_t{};
  }

  tables_t m_suffix;
  tables_t m_prefix;
};

} // namespace lemmatization
} // namespace deeplima

#endif
