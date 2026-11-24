// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2026 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <boost/program_options.hpp>

#include "conllu/mwt_dict.h"
#include "deeplima/mwt_lexicon.h"

using namespace std;
namespace po = boost::program_options;

int main(int argc, char* argv[])
{
  setlocale(LC_ALL, "en_US.UTF-8");

  vector<string> input_files;
  string output_file;
  string dict_file;
  uint32_t min_rule_support = 2;

  po::options_description desc("deeplima (generate multiword-token expansion dictionary)");
  desc.add_options()
  ("help,h",
   "Display this help message")
  ("input,i",  po::value<vector<string>>(&input_files)->multitoken(),
   "Input files (.conllu)")
  ("dict,d",   po::value<string>(&dict_file),
   "Existing dictionary (.mwt) to (re)compute the affix rules of, instead of input files: "
   "its entries are copied, its old rules replaced")
  ("output,o", po::value<string>(&output_file),
   "Output dictionary file (default: stdout)")
  ("rule-min-support", po::value<uint32_t>(&min_rule_support)->default_value(2),
   "Minimum number of dictionary entries giving an affix rule (the fallback for "
   "flagged multiword tokens missing from the dictionary); 0 disables the rules")
  ;

  po::variables_map vm;
  try
  {
    po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
    po::notify(vm);
  }
  catch (const std::exception& e)
  {
    cerr << e.what() << endl;
    return -1;
  }

  if (vm.count("help") || (input_files.empty() == dict_file.empty()))
  {
    cout << desc << endl;
    return 0;
  }

  if (!dict_file.empty())
  {
    // Entries copied as they are, the rules learned again from them
    ifstream in(dict_file);
    if (!in.is_open())
    {
      cerr << "Can't open dictionary " << dict_file << endl;
      return -1;
    }
    vector<string> lines;
    string line;
    while (getline(in, line))
    {
      if (0 != line.rfind("#rule\t", 0))
      {
        lines.push_back(line);
      }
    }
    in.close();
    deeplima::MwtLexicon lexicon;
    {
      stringstream entries;
      for (const auto& l : lines)
      {
        entries << l << "\n";
      }
      lexicon.load(entries);
    }
    const size_t n_rules = lexicon.learn_rules(min_rule_support);
    ofstream file;
    if (!output_file.empty())
    {
      file.open(output_file);
      if (!file.is_open())
      {
        cerr << "Can't open output file " << output_file << endl;
        return -1;
      }
    }
    ostream& out = output_file.empty() ? cout : file;
    for (const auto& l : lines)
    {
      out << l << "\n";
    }
    lexicon.write_rules(out);
    cerr << "Kept " << lexicon.size() << " multiword-token entries, wrote " << n_rules
         << " affix rules" << (output_file.empty() ? "" : " to " + output_file) << "." << endl;
    return 0;
  }

  try
  {
    if (output_file.empty())
    {
      size_t n_rules = 0;
      const size_t n = deeplima::CoNLLU::extract_mwt_dict(input_files, cout, min_rule_support, &n_rules);
      cerr << "Wrote " << n << " multiword-token entries and " << n_rules << " affix rules." << endl;
    }
    else
    {
      ofstream out(output_file);
      if (!out.is_open())
      {
        cerr << "Can't open output file " << output_file << endl;
        return -1;
      }
      size_t n_rules = 0;
      const size_t n = deeplima::CoNLLU::extract_mwt_dict(input_files, out, min_rule_support, &n_rules);
      cerr << "Wrote " << n << " multiword-token entries and " << n_rules << " affix rules to "
           << output_file << endl;
    }
  }
  catch (const std::exception& e)
  {
    cerr << e.what() << endl;
    return -1;
  }

  return 0;
}
