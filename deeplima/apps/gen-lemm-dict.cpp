#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

using namespace std;
namespace po = boost::program_options;

struct rules_options_t
{
  string m_output;        // empty: no rules
  size_t m_max_suffix;
  size_t m_max_form_freq; // 0: all forms
  size_t m_max_prefix;    // 0: suffix rules only
};

int generate_dict(const vector<string>& input_files, const set<string>& upos_to_skip,
                  const string& conflict_resolution, const rules_options_t& rules_options);

int main(int argc, char* argv[])
{
  setlocale(LC_ALL, "en_US.UTF-8");

  vector<string> input_files;
  string conflict_resolution = "majority";
  vector<string> upos_to_skip = { "PUNCT", "SYM", "X" };
  rules_options_t rules_options{"", 6, 2, 0};

  po::options_description desc("deeplima (generate lemmatization dictionary)");
  desc.add_options()
  ("help,h",
   "Display this help message")
  ("input,i",       po::value<vector<string>>(&input_files)->multitoken(),
   "Input files (.conllu)")
  ("conflict,c",    po::value<string>(&conflict_resolution)->default_value(conflict_resolution),
   "When a (form, UPOS, FEATS) has several lemmata: \"majority\" keeps the most frequent "
   "one (none on a tie), \"reject\" drops the entry")
  ("rules,r",       po::value<string>(&rules_options.m_output),
   "Also write edit rules to this file, for words missing from the dictionary "
   "(deeplima --lem-rules)")
  ("rules-max-suffix", po::value<size_t>(&rules_options.m_max_suffix)->default_value(rules_options.m_max_suffix),
   "Longest word ending (in characters) the rules are indexed by")
  ("rules-max-freq", po::value<size_t>(&rules_options.m_max_form_freq)->default_value(rules_options.m_max_form_freq),
   "Learn rules only from forms occurring at most this many times (0: all forms). "
   "Rare words resemble the unknown words the rules are for")
  ("rules-max-prefix", po::value<size_t>(&rules_options.m_max_prefix)->default_value(rules_options.m_max_prefix),
   "Also learn rules editing the beginning of words, stripping at most this many characters "
   "(0: suffix rules only). For languages that change the start of words (Irish "
   "mutations, Indonesian, Hebrew or Arabic prefixes)")
  ;

  po::variables_map vm;

  try
  {
    po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
    po::notify(vm);
  }
  catch (const boost::program_options::unknown_option& e)
  {
    cerr << e.what() << endl;
    return -1;
  }

  if (vm.count("help") || input_files.size() == 0
   || (conflict_resolution != "reject" && conflict_resolution != "majority"))
  {
    cout << desc << endl;
    return 0;
  }

  return generate_dict(input_files, set<string>(upos_to_skip.begin(), upos_to_skip.end()),
                       conflict_resolution, rules_options);
}

#include <tuple>
#include <unordered_map>
#include <unicode/unistr.h>
#include <unicode/regex.h>

#include "conllu/treebank.h"
#include "deeplima/lemmatization/lemm_rules.h"

using namespace icu;
using namespace deeplima;

inline string toUtf8(const UnicodeString& src)
{
  string out;
  src.toUTF8String(out);
  return out;
}

struct UnicodeStringHash
{
  inline size_t operator()(const UnicodeString& k) const
  {
    return size_t(k.hashCode());
  }
};

struct lemmatization_dict_t
{
  vector<string> m_sources;
  struct form_t
   {
     UnicodeString m_form;
     string m_upos;
     string m_feats;

     struct hasher
     {
       inline size_t operator()(const form_t& k) const
       {
         return size_t(k.m_form.hashCode()) ^ hash<string>()(k.m_upos) ^ hash<string>()(k.m_feats);
       }
     };

     inline bool operator==(const form_t& other) const
     {
       return (m_form == other.m_form) && (m_upos == other.m_upos) && (m_feats == other.m_feats);
     }
  };

  unordered_map<form_t, unordered_map<UnicodeString, map<size_t, size_t>, UnicodeStringHash>, form_t::hasher> data;
  // form -> { lemma_t -> counter per source }

  // For the edit rules: every word's form, and the distinct
  // (form, UPOS, FEATS, lemma) observations, whatever their UPOS
  unordered_map<string, size_t> form_freq;
  set<tuple<string, string, string, string>> observations;
};

// A form is worth a dictionary entry if it holds a letter or a digit. Requiring
// letters only ("^\\p{Letter}+$") dropped "M.", "Bar-le-Duc", "aujourd'hui",
// "d'" or "80 000", sending these known words to the lemmatization model.
UnicodeString reGoodToLemmatize = "[\\p{Letter}\\p{Number}]";

bool load(const string& fn, const set<string>& upos_to_skip, lemmatization_dict_t& dict, size_t src_idx)
{
  CoNLLU::Annotation annotation;
  annotation.load(fn);
  UErrorCode regex_status = U_ZERO_ERROR;
  RegexMatcher goodToLemmatize(reGoodToLemmatize, 0, regex_status);
  if (U_FAILURE(regex_status))
  {
    throw runtime_error("Failed while compiling regexp");
  }

  for (const auto& word: annotation.words())
  {
    const CoNLLU::CoNLLULine& line = annotation.get_line(word.m_line_idx);
    const string& form = line.form();
    const string& lemma = line.lemma();

    dict.form_freq[form]++;
    // Rules are learned from every UPOS, foreign words included: copying the
    // form is precisely what they must learn for PROPN, X or NUM
    if (!line.is_typo() && !form.empty() && !lemma.empty() && (lemma != "_" || form == "_"))
    {
      dict.observations.emplace(form, line.upos(), line.feats_str(), lemma);
    }

    if (line.is_foreign() || line.is_typo() || upos_to_skip.end() != upos_to_skip.find(line.upos()))
    {
      continue;
    }

    UnicodeString u_form = UnicodeString::fromUTF8(form);
    UnicodeString u_lemma = UnicodeString::fromUTF8(lemma);

    goodToLemmatize.reset(u_form);
    if (!goodToLemmatize.find())
    {
      cerr << "Form \"" << form << "\" isn't good for lemmatization (regexp test)" << endl;
      continue;
    }
    lemmatization_dict_t::form_t value{u_form, line.upos(), line.feats_str()};
    dict.data[value][u_lemma][src_idx]++;
  }
  return true;
}

lemmatization_dict_t load(const vector<string>& fn, const set<string>& upos_to_skip)
{
  lemmatization_dict_t dict;
  dict.m_sources = fn;
  for (size_t i = 0; i < fn.size(); i++)
  {
    load(fn[i], upos_to_skip, dict, i);
  }
  return dict;
}

string stat2str(const map<size_t, size_t>& stat, const vector<string>& src_fn)
{
  ostringstream oss;
  for (size_t i = 0; i < src_fn.size(); i++)
  {
    const string& fn = src_fn[i];
    boost::filesystem::path p(fn);
    if (i > 0)
    {
      oss << " ";
    }
    const auto it = stat.find(i);
    if (stat.end() != it)
    {
      oss << p.filename() << ":" << it->second;
    }
  }
  return oss.str();
}

map<pair<UnicodeString, string>, UnicodeString> process(const lemmatization_dict_t& dict,
                                                        const string& conflict_resolution)
{
  if (conflict_resolution != "reject" && conflict_resolution != "majority")
  {
    throw runtime_error("Unknown conflict resolution \"" + conflict_resolution + "\"");
  }
  map<pair<UnicodeString, string>, UnicodeString> out;

  for (const auto& [form_info, value] : dict.data)
  {
    if (value.size() == 0)
    {
      throw runtime_error(string("Zero lemmata for form \"") + toUtf8(form_info.m_form) + "\"");
    }
    const UnicodeString* chosen = &value.begin()->first;
    if (value.size() > 1)
    {
      cerr << "Multiple (" << value.size() << ") lemmata for form \"" << toUtf8(form_info.m_form) << "\":" << endl;
      for (const auto& [ lemma, src_info ] : value)
      {
        cerr << "\t" << form_info.m_upos
             << "\t" << form_info.m_feats
             << "\t" << toUtf8(lemma)
             << "\t" << stat2str(src_info, dict.m_sources) << endl;
      }
      if (conflict_resolution == "reject")
      {
        continue;
      }
      // majority: the most frequent lemma over all sources, none on a tie
      auto total = [](const map<size_t, size_t>& src_info)
      {
        size_t n = 0;
        for (const auto& kv : src_info) n += kv.second;
        return n;
      };
      size_t best = 0;
      bool tie = false;
      for (const auto& [ lemma, src_info ] : value)
      {
        const size_t n = total(src_info);
        if (n > best)
        {
          best = n;
          chosen = &lemma;
          tie = false;
        }
        else if (n == best)
        {
          tie = true;
        }
      }
      if (tie)
      {
        cerr << "\tno majority: entry dropped" << endl;
        continue;
      }
      cerr << "\tkept \"" << toUtf8(*chosen) << "\"" << endl;
    }

    out[make_pair(form_info.m_form, form_info.m_upos + " " + form_info.m_feats)] = *chosen;
  }

  return out;
}

void print(const map<pair<UnicodeString, string>, UnicodeString>& output)
{
  for (const auto& [key, value] : output)
  {
    cout << toUtf8(key.first) << "\t"
         << key.second << "\t"
         << toUtf8(value) << endl;
  }
}

void write_rules(const lemmatization_dict_t& dict, const rules_options_t& options)
{
  deeplima::lemmatization::lemm_rules_builder_t builder(options.m_max_suffix, options.m_max_prefix);
  size_t used = 0;
  for (const auto& [form, upos, feats, lemma] : dict.observations)
  {
    if (options.m_max_form_freq > 0 && dict.form_freq.at(form) > options.m_max_form_freq)
    {
      continue;
    }
    builder.add(upos, feats,
                deeplima::lemmatization::utf8_to_u32(form),
                deeplima::lemmatization::utf8_to_u32(lemma));
    ++used;
  }
  ofstream out(options.m_output);
  if (!out)
  {
    throw runtime_error("Can't open rules file " + options.m_output);
  }
  builder.write(out);
  if (!out)
  {
    throw runtime_error("Failed writing rules file " + options.m_output);
  }
  cerr << "Rules learned from " << used << " of " << dict.observations.size()
       << " distinct (form, UPOS, FEATS, lemma) written to " << options.m_output << endl;
}

int generate_dict(const vector<string>& input_files, const set<string>& upos_to_skip,
                  const string& conflict_resolution, const rules_options_t& rules_options)
{
  try
  {
    lemmatization_dict_t dict = load(input_files, upos_to_skip);

    const auto output = process(dict, conflict_resolution);

    print(output);

    if (!rules_options.m_output.empty())
    {
      write_rules(dict, rules_options);
    }
  }
  catch (const std::exception& e)
  {
    cerr << e.what() << endl;
    return -1;
  }
  catch (...)
  {
    cerr << "Unknown error happened" << endl;
    return -1;
  }

  return 0;
}
