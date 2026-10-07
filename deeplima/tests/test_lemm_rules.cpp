// Copyright 2026 CEA LIST
// SPDX-FileCopyrightText: 2026 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

// Unit tests for the suffix edit rules used to lemmatize words missing from
// the lemma dictionary: rule extraction and application, Unicode lowercasing,
// the builder's indexing and tie-breaking, the file round trip and the lookup
// order.

#include <iostream>
#include <sstream>
#include <string>

#include "deeplima/lemmatization/lemm_rules.h"

using namespace deeplima::lemmatization;

static int g_failures = 0;

#define CHECK(cond, msg)                                        \
  do {                                                          \
    if (!(cond)) {                                              \
      std::cerr << "FAIL: " << (msg) << std::endl;              \
      ++g_failures;                                             \
    }                                                           \
  } while (0)

static std::u32string u(const std::string& s) { return utf8_to_u32(s); }

typedef lemm_rules_t<std::string> string_rules_t;

/**
 * Loads a rules file the way TokenSequenceAnalyzer does. With upos_only_keys,
 * keys are the UPOS alone, the way the analyzer's encoding makes "NOUN _" and
 * the fallback "NOUN *" the same key; otherwise "UPOS FEATS" strings.
 */
static string_rules_t load(const std::string& text, bool upos_only_keys = false)
{
  string_rules_t rules;
  std::istringstream in(text);
  read_rules(in, [&rules, upos_only_keys](const rule_line_t& r)
  {
    const bool fallback = r.m_feats == any_feats();
    const std::string key = upos_only_keys ? r.m_upos : r.m_upos + " " + (fallback ? "*" : r.m_feats);
    rules.add(key, r.m_suffix, r.m_rule, r.m_count, fallback);
  });
  return rules;
}

static std::string lemmatize(const string_rules_t& rules, const std::string& upos,
                             const std::string& feats, const std::string& form)
{
  std::u32string lemma;
  if (!rules.lemmatize(upos + " " + feats, upos + " *", u(form), lemma))
  {
    return "<none>";
  }
  return u32_to_utf8(lemma);
}

int main()
{
  // to_lower: any script, length preserved (the former Latin-1-only version
  // truncated every code point above U+00FF)
  CHECK(to_lower(u("ÉVÊQUE Œuvre ĞΣΑ")) == u("évêque œuvre ğσα"), "to_lower beyond Latin-1");
  CHECK(to_lower(u("İstanbul")).size() == u("İstanbul").size(), "to_lower keeps the length");
  CHECK(u32_to_utf8(u("Bar-le-Duc 80 000")) == "Bar-le-Duc 80 000", "UTF-8 round trip");

  // extract_rule / apply_rule
  {
    const edit_rule_t r = extract_rule(u("chevaux"), u("cheval"));
    CHECK(!r.m_lower && r.m_strip == 2 && r.m_add == u("l"), "chevaux -> cheval is strip 2, add l");
    const edit_rule_t f = extract_rule(u("chantera"), u("chanter"));
    CHECK(!f.m_lower && f.m_strip == 1 && f.m_add.empty(), "chantera -> chanter is strip 1");
    const edit_rule_t p = extract_rule(u("Princes"), u("prince"));
    CHECK(p.m_lower && p.m_strip == 1 && p.m_add.empty(), "Princes -> prince lowercases");
    const edit_rule_t c = extract_rule(u("Paris"), u("Paris"));
    CHECK(!c.m_lower && c.m_strip == 0 && c.m_add.empty(), "Paris -> Paris is a copy");
    const edit_rule_t m = extract_rule(u("M."), u("monsieur"));
    CHECK(m.m_strip == 1 && m.m_add == u("onsieur"), "M. -> monsieur");

    std::u32string lemma;
    CHECK(apply_rule(u("travaux"), r, lemma) && lemma == u("traval"), "apply chevaux's rule blindly");
    CHECK(apply_rule(u("dansera"), f, lemma) && lemma == u("danser"), "apply chantera's rule to dansera");
    CHECK(apply_rule(u("Offices"), p, lemma) && lemma == u("office"), "apply Princes's rule to Offices");
    CHECK(!apply_rule(u("a"), edit_rule_t{false, 2, U""}, lemma), "a rule stripping more than the form fails");
    CHECK(!apply_rule(u("ab"), edit_rule_t{false, 2, U""}, lemma), "a rule leaving an empty lemma fails");
  }

  // Builder: indexing, the stem guard, tie-breaking, written format
  {
    lemm_rules_builder_t builder(6);
    builder.add("VERB", "Mood=Ind|Tense=Imp", u("lisait"), u("lire"));           // strip "sait"
    builder.add("VERB", "Mood=Ind|Tense=Imp", u("chantait"), u("chanter"));      // strip "ait", add "er"
    builder.add("VERB", "Mood=Ind|Tense=Imp", u("parlait"), u("parler"));
    builder.add("NOUN", "Number=Plur", u("maisons"), u("maison"));
    builder.add("NOUN", "Number=Plur", u("Princes"), u("prince"));
    std::ostringstream out;
    builder.write(out);
    const std::string text = out.str();
    CHECK(text.rfind("# deeplima lemmatization rules", 0) == 0, "header line");
    // lisait's rule strips 4 characters: never indexed under the 3-letter ending
    CHECK(text.find("VERB\t*\tait\t0\t3\ter\t2\n") != std::string::npos, "-ait -> -er, supported by 2 words");
    CHECK(text.find("VERB\t*\tait\t0\t4") == std::string::npos, "lire's 4-letter strip not under -ait");
    CHECK(text.find("VERB\t*\tsait\t0\t4\tre\t1\n") != std::string::npos, "lire's rule under -sait");

    const string_rules_t rules = load(text);
    CHECK(!rules.empty(), "rules loaded");
    CHECK(lemmatize(rules, "VERB", "Mood=Ind|Tense=Imp", "affichait") == "afficher",
          "affichait gets -ait -> -er: lisait's 4-letter strip is not indexed under -ait (stem guard)");
    CHECK(lemmatize(rules, "VERB", "Mood=Ind|Tense=Imp", "conduisait") == "conduire",
          "conduisait gets the longer -sait ending");
    CHECK(lemmatize(rules, "VERB", "Mood=Cnd", "chantait") == "chanter",
          "unknown features fall back on the UPOS-only rules");
    CHECK(lemmatize(rules, "NOUN", "Number=Plur", "Offices") == "office", "capitalised plural noun");
    CHECK(lemmatize(rules, "ADJ", "_", "grandes") == "<none>", "no rule for an unknown UPOS");
  }

  // Exact features win over the fallback at the same ending length, and a
  // longer ending wins over both
  {
    const string_rules_t rules = load(
        "NOUN\t*\ts\t0\t1\t\t10\n"
        "NOUN\tNumber=Sing\ts\t0\t0\t\t3\n"
        "NOUN\t*\taux\t0\t3\tal\t5\n");
    CHECK(lemmatize(rules, "NOUN", "Number=Sing", "souris") == "souris", "exact features first");
    CHECK(lemmatize(rules, "NOUN", "Number=Plur", "tables") == "table", "fallback when features differ");
    CHECK(lemmatize(rules, "NOUN", "Number=Sing", "chevaux") == "cheval", "longest ending first");
  }

  // Two lines with the same key (features spelled in two orders map to one
  // encoding in the analyzer): the most supported rule stays
  {
    const string_rules_t rules = load(
        "NOUN\t*\ts\t0\t0\t\t2\n"
        "NOUN\t*\ts\t0\t1\t\t9\n"
        "NOUN\t*\ts\t0\t0\t\t4\n");
    CHECK(rules.size() == 1, "one rule per key");
    CHECK(lemmatize(rules, "NOUN", "_", "tables") == "table", "the rule with count 9 kept");
  }

  // A word without features: its exact key equals the fallback key in the
  // analyzer's encoding. The exact rule must still win at equal length, even
  // though the fallback is better supported
  {
    const string_rules_t rules = load(
        "ADV\t*\tment\t0\t4\t\t40\n"
        "ADV\t_\tment\t0\t0\t\t2\n", true);
    CHECK(rules.size() == 2, "exact and fallback rules both kept under the same key");
    std::u32string lemma;
    CHECK(rules.lemmatize("ADV", "ADV", u("normalement"), lemma) && lemma == u("normalement"),
          "the exact (no-features) rule wins over the fallback under an equal key");
  }

  // Malformed files are rejected with the line number
  {
    bool thrown = false;
    try
    {
      load("# comment\nNOUN\t*\ts\t0\t1\n");
    }
    catch (const std::runtime_error& e)
    {
      thrown = std::string(e.what()).find("line 2") != std::string::npos;
    }
    CHECK(thrown, "a 5-field line is rejected, naming its line");
    thrown = false;
    try
    {
      load("NOUN\t*\ts\t2\t1\t\t1\n");
    }
    catch (const std::runtime_error&)
    {
      thrown = true;
    }
    CHECK(thrown, "lower must be 0 or 1");
  }

  if (g_failures == 0)
  {
    std::cout << "test_lemm_rules: all tests passed" << std::endl;
    return 0;
  }
  std::cerr << "test_lemm_rules: " << g_failures << " failure(s)" << std::endl;
  return 1;
}
