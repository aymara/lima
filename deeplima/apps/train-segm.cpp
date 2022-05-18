// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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

  po::options_description desc("DeepLima");
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

