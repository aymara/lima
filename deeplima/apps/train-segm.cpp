/*
    Copyright 2021 CEA LIST

    This file is part of LIMA.

    LIMA is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    LIMA is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with LIMA.  If not, see <http://www.gnu.org/licenses/>
*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <functional>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include "conllu/treebank.h"
#include "tasks/segmentation/train/train_segmentation.h"

using namespace std;
namespace po = boost::program_options;

int train_segmentation_model(const string& ud_path, const string& corpus, const string& model_name, bool train_ss);

int main(int argc, char* argv[])
{
  setlocale(LC_ALL, "en_US.UTF-8");

  string corpus, ud_path, model;
  bool train_ss;

  po::options_description desc("DeepLima Segmentation Trainer");
  desc.add_options()
  ("help,h",                                            "Display this help message")
  ("corpus,c",          po::value<string>(&corpus),     "Training corpus name")
  ("ud-path,u",         po::value<string>(&ud_path),    "Path to UD collection")
  ("name,n",            po::value<string>(&model),      "Model name")
  ("sentence,s",        po::bool_switch()->default_value(false),     "Train sentence segmentation")
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

  if (vm.count("help")) {
      cout << desc << endl;
      return 0;
  }
  if (vm.count("corpus")==0) {
      std::cerr << "Mandatory parameter missing: corpus" << std::endl;
      std::cerr << desc << std::endl;
      return 0;
  }
  if (vm.count("ud-path")==0) {
      std::cerr << "Mandatory parameter missing: ud-path" << std::endl;
      std::cerr << desc << std::endl;
      return 0;
  }
  if (vm.count("name")==0) {
      std::cerr << "Mandatory parameter missing: name" << std::endl;
      std::cerr << desc << std::endl;
      return 0;
  }
  train_ss = vm["sentence"].as<bool>();

  return train_segmentation_model(ud_path, corpus, model, train_ss);
}

using namespace deeplima;

int train_segmentation_model(const string& ud_path, const string& corpus, const string& model_name, bool train_ss)
{
  boost::filesystem::path path_to_treebank = boost::filesystem::path(ud_path) / corpus;
  CoNLLU::Treebank tb(path_to_treebank.string());

  return train_segmentation_model(tb, model_name, train_ss);
}

