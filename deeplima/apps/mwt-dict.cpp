// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2026 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <boost/program_options.hpp>

#include "conllu/mwt_dict.h"

using namespace std;
namespace po = boost::program_options;

int main(int argc, char* argv[])
{
  setlocale(LC_ALL, "en_US.UTF-8");

  vector<string> input_files;
  string output_file;

  po::options_description desc("deeplima (generate multiword-token expansion dictionary)");
  desc.add_options()
  ("help,h",
   "Display this help message")
  ("input,i",  po::value<vector<string>>(&input_files)->multitoken(),
   "Input files (.conllu)")
  ("output,o", po::value<string>(&output_file),
   "Output dictionary file (default: stdout)")
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

  if (vm.count("help") || input_files.empty())
  {
    cout << desc << endl;
    return 0;
  }

  try
  {
    if (output_file.empty())
    {
      const size_t n = deeplima::CoNLLU::extract_mwt_dict(input_files, cout);
      cerr << "Wrote " << n << " multiword-token entries." << endl;
    }
    else
    {
      ofstream out(output_file);
      if (!out.is_open())
      {
        cerr << "Can't open output file " << output_file << endl;
        return -1;
      }
      const size_t n = deeplima::CoNLLU::extract_mwt_dict(input_files, out);
      cerr << "Wrote " << n << " multiword-token entries to " << output_file << endl;
    }
  }
  catch (const std::exception& e)
  {
    cerr << e.what() << endl;
    return -1;
  }

  return 0;
}
