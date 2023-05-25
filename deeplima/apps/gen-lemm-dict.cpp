#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

using namespace std;
namespace po = boost::program_options;

int generate_dict(const vector<string>& input_files, const set<string>& upos_to_skip, const string& conflict_resolution);

int main(int argc, char* argv[])
{
  setlocale(LC_ALL, "en_US.UTF-8");

  vector<string> input_files;
  string conflict_resolution = "reject";
  vector<string> upos_to_skip = { "PUNCT", "SYM", "X" };

  po::options_description desc("deeplima (generate lemmatization dictionary)");
  desc.add_options()
  ("help,h",
   "Display this help message")
  ("input,i",       po::value<vector<string>>(&input_files)->multitoken(),
   "Input files (.conllu)")
  ("conflict,c",    po::value<string>(&conflict_resolution),
   "Conflict resolution strategy: \"reject\" (all) or \"vote\" (not implemented)")
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
   || (conflict_resolution.size() > 0 && conflict_resolution != "reject"))
  {
    cout << desc << endl;
    return 0;
  }

  return generate_dict(input_files, set<string>(upos_to_skip.begin(), upos_to_skip.end()), conflict_resolution);
}

#include <unordered_map>
#include <unicode/unistr.h>
#include <unicode/regex.h>

#include "conllu/treebank.h"

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
};

UnicodeString reGoodToLemmatize = "^\\p{Letter}+$";

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

  for (auto it = annotation.words_begin(); it != annotation.words_end(); it++)
  {
    const CoNLLU::CoNLLULine& line = annotation.get_line((*it).m_line_idx);
    if (line.is_foreign() || line.is_typo() || upos_to_skip.end() != upos_to_skip.find(line.upos()))
    {
      continue;
    }

    const string& form = line.form();
    const string& lemma = line.lemma();

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
  if (conflict_resolution != "reject")
  {
    throw runtime_error("Only \"reject\" conflict resolution is supported");
  }
  map<pair<UnicodeString, string>, UnicodeString> out;

  for (const auto& [form_info, value] : dict.data)
  {
    if (value.size() == 0)
    {
      throw runtime_error(string("Zero lemmata for form \"") + toUtf8(form_info.m_form) + "\"");
    }
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
      continue;
    }

    out[make_pair(form_info.m_form, form_info.m_upos + " " + form_info.m_feats)] = value.begin()->first;
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

int generate_dict(const vector<string>& input_files, const set<string>& upos_to_skip, const string& conflict_resolution)
{
  try
  {
    lemmatization_dict_t dict = load(input_files, upos_to_skip);

    const auto output = process(dict, conflict_resolution);

    print(output);
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
