// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2026 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_MWT_LEXICON_H
#define DEEPLIMA_MWT_LEXICON_H

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <istream>
#include <map>
#include <ostream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace deeplima
{

/**
 * Multiword-token (MWT) lexicon: how to expand a surface token (French "du")
 * into its syntactic words ("de", "le"). Shared by deeplima's MwtExpander, by
 * deeplima-mwt-dict, which builds it from training CoNLL-U files, and by
 * LIMA's RnnTokenizer.
 *
 * Two layers:
 *  - a dictionary of the surfaces seen in training, with their most frequent
 *    expansion;
 *  - affix rules, the fallback for a surface the segmenter flags as multiword
 *    but that the dictionary does not know. A suffix rule cuts a surface
 *    ending, rewrites the end of the remaining stem and adds fixed words:
 *    Polish "zrobiłem" = "zrobił" + "em" (suffix "em", no rewrite, word "em");
 *    a Tamil surface may end with a fused form of the clitic, the rule's
 *    rewrite restores the stem's last letters. A prefix rule is the mirror
 *    (Arabic "و" + stem). Rules are learned from the dictionary itself: each
 *    entry gives one suffix rule (from its longest common prefix with its first
 *    word) and one prefix rule (from its longest common suffix with its last
 *    word); a rule is kept when at least min_support entries give it, and, for
 *    each affix, only the best rule. Rules are scored by their precision on the
 *    dictionary, support / (entries having the affix + 1), smoothed so that
 *    a rare affix does not win over a frequent one.
 *
 * Rules are only meant for surfaces the segmenter flagged: applied to every
 * token, they would split most words.
 *
 * File format (UTF-8, one entry per line, tab-separated):
 *   surface  count  word1  word2 ...                             dictionary
 *   #rule  suffix|prefix  affix  rewrite  support  applicable  word1 ...
 * A line starting with '#' is a comment to older readers, which thus keep
 * reading the dictionary and ignore the rules.
 */
class MwtLexicon
{
public:
  struct Rule
  {
    bool suffix = true;
    std::string affix;    // removed from the surface
    std::string rewrite;  // appended (suffix) or prepended (prefix) to the stem
    std::vector<std::string> words; // after (suffix) or before (prefix) the stem word
    uint32_t support = 0;    // dictionary entries the rule reproduces
    uint32_t applicable = 0; // dictionary entries having the affix

    double score() const
    {
      return double(support) / double(applicable + 1);
    }

    // Is this rule better than the other one (score, support, affix length,
    // then a fixed order for determinism)?
    bool better_than(const Rule& o) const
    {
      if (score() != o.score()) return score() > o.score();
      if (support != o.support) return support > o.support;
      if (affix.size() != o.affix.size()) return affix.size() > o.affix.size();
      return std::tie(suffix, affix, rewrite, words) < std::tie(o.suffix, o.affix, o.rewrite, o.words);
    }

    std::vector<std::string> apply(const std::string& surface) const
    {
      std::vector<std::string> result;
      if (suffix)
      {
        result.push_back(surface.substr(0, surface.size() - affix.size()) + rewrite);
        result.insert(result.end(), words.begin(), words.end());
      }
      else
      {
        result = words;
        result.push_back(rewrite + surface.substr(affix.size()));
      }
      return result;
    }
  };

  MwtLexicon() = default;

  explicit MwtLexicon(const std::string& fn)
  {
    load(fn);
  }

  void load(const std::string& fn)
  {
    std::ifstream f(fn);
    if (!f.is_open())
    {
      throw std::runtime_error("MwtLexicon: can't open \"" + fn + "\"");
    }
    load(f);
  }

  void load(std::istream& f)
  {
    m_dict.clear();
    m_suffix_rules.clear();
    m_prefix_rules.clear();
    std::string line;
    while (std::getline(f, line))
    {
      if (!line.empty() && '\r' == line.back())
      {
        line.pop_back();
      }
      if (line.empty())
      {
        continue;
      }
      const std::vector<std::string> fields = split(line);
      if ('#' == line[0])
      {
        // #rule  suffix|prefix  affix  rewrite  support  applicable  word1 ...
        if (fields.size() >= 7 && "#rule" == fields[0]
            && ("suffix" == fields[1] || "prefix" == fields[1]) && !fields[2].empty())
        {
          Rule r;
          r.suffix = ("suffix" == fields[1]);
          r.affix = fields[2];
          r.rewrite = fields[3];
          try
          {
            r.support = uint32_t(std::stoul(fields[4]));
            r.applicable = uint32_t(std::stoul(fields[5]));
          }
          catch (const std::logic_error&)
          {
            continue; // malformed counts: not a rule
          }
          r.words.assign(fields.begin() + 6, fields.end());
          add_rule(std::move(r));
        }
        continue;
      }
      // surface  count  word1  word2 ...: at least two words
      if (fields.size() < 4 || fields[0].empty())
      {
        continue;
      }
      m_dict.emplace(fields[0], std::vector<std::string>(fields.begin() + 2, fields.end()));
    }
  }

  size_t size() const
  {
    return m_dict.size();
  }

  size_t rules_size() const
  {
    return m_suffix_rules.size() + m_prefix_rules.size();
  }

  void add_entry(const std::string& surface, const std::vector<std::string>& words)
  {
    m_dict.emplace(surface, words);
  }

  // Keeps, for each affix, the best rule.
  void add_rule(Rule r)
  {
    auto& rules = r.suffix ? m_suffix_rules : m_prefix_rules;
    auto it = rules.find(r.affix);
    if (rules.end() == it)
    {
      std::string affix = r.affix;
      rules.emplace(std::move(affix), std::move(r));
    }
    else if (r.better_than(it->second))
    {
      it->second = std::move(r);
    }
  }

  // The dictionary's expansion of surface, or nullptr.
  const std::vector<std::string>* lookup(const std::string& surface) const
  {
    const auto it = m_dict.find(surface);
    return (m_dict.end() == it) ? nullptr : &it->second;
  }

  // The best rule for surface, or nullptr. The stem keeps at least one character.
  const Rule* find_rule(const std::string& surface) const
  {
    const Rule* best = nullptr;
    // Every character boundary strictly inside the surface
    for (size_t i = 1; i < surface.size(); ++i)
    {
      if (is_continuation(surface[i]))
      {
        continue;
      }
      const auto s = m_suffix_rules.find(surface.substr(i));
      if (m_suffix_rules.end() != s && (nullptr == best || s->second.better_than(*best)))
      {
        best = &s->second;
      }
      const auto p = m_prefix_rules.find(surface.substr(0, i));
      if (m_prefix_rules.end() != p && (nullptr == best || p->second.better_than(*best)))
      {
        best = &p->second;
      }
    }
    return best;
  }

  // Expands surface into words: from the dictionary, then, when use_rules,
  // with the best rule. False when the surface stays as it is.
  bool expand(const std::string& surface, std::vector<std::string>& words, bool use_rules) const
  {
    if (const auto* w = lookup(surface))
    {
      words = *w;
      return true;
    }
    if (use_rules)
    {
      if (const Rule* r = find_rule(surface))
      {
        words = r->apply(surface);
        return true;
      }
    }
    return false;
  }

  // Learns the affix rules from the dictionary (see the class comment).
  // Returns the number of rules kept.
  size_t learn_rules(uint32_t min_support)
  {
    m_suffix_rules.clear();
    m_prefix_rules.clear();
    if (0 == min_support)
    {
      return 0;
    }

    std::map<Rule, uint32_t, RuleOrder> support;
    for (const auto& [surface, words] : m_dict)
    {
      for (Rule& r : rules_of(surface, words))
      {
        support[r]++;
      }
    }

    // Entries having each affix, for the affixes of the rules that are kept
    std::map<std::pair<bool, std::string>, uint32_t> applicable;
    for (const auto& [r, n] : support)
    {
      if (n >= min_support)
      {
        applicable.emplace(std::make_pair(r.suffix, r.affix), 0);
      }
    }
    for (const auto& [surface, words] : m_dict)
    {
      for (size_t i = 1; i < surface.size(); ++i)
      {
        if (is_continuation(surface[i]))
        {
          continue;
        }
        auto s = applicable.find(std::make_pair(true, surface.substr(i)));
        if (applicable.end() != s) s->second++;
        auto p = applicable.find(std::make_pair(false, surface.substr(0, i)));
        if (applicable.end() != p) p->second++;
      }
    }

    for (const auto& [r, n] : support)
    {
      if (n < min_support)
      {
        continue;
      }
      Rule kept = r;
      kept.support = n;
      kept.applicable = applicable[std::make_pair(r.suffix, r.affix)];
      add_rule(std::move(kept));
    }
    return rules_size();
  }

  // Writes the rules in the file format, by side then affix.
  void write_rules(std::ostream& out) const
  {
    for (const auto* rules : {&m_suffix_rules, &m_prefix_rules})
    {
      std::map<std::string, const Rule*> sorted;
      for (const auto& [affix, r] : *rules)
      {
        sorted.emplace(affix, &r);
      }
      for (const auto& [affix, r] : sorted)
      {
        out << "#rule\t" << (r->suffix ? "suffix" : "prefix") << "\t" << r->affix << "\t"
            << r->rewrite << "\t" << r->support << "\t" << r->applicable;
        for (const auto& w : r->words)
        {
          out << "\t" << w;
        }
        out << "\n";
      }
    }
  }

  // The suffix and the prefix rule an entry gives, if any.
  static std::vector<Rule> rules_of(const std::string& surface, const std::vector<std::string>& words)
  {
    std::vector<Rule> result;
    if (words.size() < 2)
    {
      return result;
    }
    // Suffix: surface = stem + affix, first word = stem + rewrite
    const std::string& first = words.front();
    const size_t k = common_prefix(surface, first);
    if (k > 0 && k < surface.size())
    {
      Rule r;
      r.suffix = true;
      r.affix = surface.substr(k);
      r.rewrite = first.substr(k);
      r.words.assign(words.begin() + 1, words.end());
      result.push_back(std::move(r));
    }
    // Prefix: surface = affix + stem, last word = rewrite + stem
    const std::string& last = words.back();
    const size_t l = common_suffix(surface, last);
    if (l > 0 && l < surface.size())
    {
      Rule r;
      r.suffix = false;
      r.affix = surface.substr(0, surface.size() - l);
      r.rewrite = last.substr(0, last.size() - l);
      r.words.assign(words.begin(), words.end() - 1);
      result.push_back(std::move(r));
    }
    return result;
  }

  // Byte length of the longest common prefix of a and b, on a character boundary
  static size_t common_prefix(const std::string& a, const std::string& b)
  {
    size_t k = 0;
    while (k < a.size() && k < b.size() && a[k] == b[k])
    {
      ++k;
    }
    // Back to the start of a character, in both strings
    while (k > 0 && ((k < a.size() && is_continuation(a[k])) || (k < b.size() && is_continuation(b[k]))))
    {
      --k;
    }
    return k;
  }

  // Byte length of the longest common suffix of a and b, on a character boundary
  static size_t common_suffix(const std::string& a, const std::string& b)
  {
    size_t k = 0;
    while (k < a.size() && k < b.size() && a[a.size() - 1 - k] == b[b.size() - 1 - k])
    {
      ++k;
    }
    // The common part must start a character
    while (k > 0 && is_continuation(a[a.size() - k]))
    {
      --k;
    }
    return k;
  }

private:
  static bool is_continuation(char c)
  {
    return (static_cast<unsigned char>(c) & 0xC0) == 0x80;
  }

  static std::vector<std::string> split(const std::string& line)
  {
    std::vector<std::string> fields;
    size_t start = 0;
    for (;;)
    {
      const size_t tab = line.find('\t', start);
      fields.push_back(line.substr(start, std::string::npos == tab ? std::string::npos : tab - start));
      if (std::string::npos == tab)
      {
        break;
      }
      start = tab + 1;
    }
    return fields;
  }

  // Identity of a rule, its counts excluded
  struct RuleOrder
  {
    bool operator()(const Rule& a, const Rule& b) const
    {
      return std::tie(a.suffix, a.affix, a.rewrite, a.words) < std::tie(b.suffix, b.affix, b.rewrite, b.words);
    }
  };

  std::unordered_map<std::string, std::vector<std::string>> m_dict;
  std::unordered_map<std::string, Rule> m_suffix_rules; // by affix
  std::unordered_map<std::string, Rule> m_prefix_rules; // by affix
};

} // namespace deeplima

#endif
