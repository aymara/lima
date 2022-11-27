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
#include "tasks/segmentation/model/train_params_segmentation.h"

using namespace std;
namespace po = boost::program_options;

int train_segmentation_model(const std::string& ud_path, const std::string& corpus,
                             deeplima::segmentation::train::train_params_segmentation_t &params, int gpuid);

int main(int argc, char* argv[])
{
  setlocale(LC_ALL, "en_US.UTF-8");

  string corpus, ud_path;
  deeplima::segmentation::train::train_params_segmentation_t params;
  int gpuid = -1;

  po::options_description desc("DeepLima Segmentation Trainer");
  desc.add_options()
  ("help,h",                                            "Display this help message")
  ("corpus,c",          po::value<string>(&corpus),     "Training corpus name")
  ("ud-path,u",         po::value<string>(&ud_path),    "Path to UD collection")
  ("name,n",            po::value<string>(&params.m_output_model_name),      "Model name")
  ("sentence,s",        po::bool_switch()->default_value(false),     "Train sentence segmentation")
  ("hidden-dim,w",      po::value<size_t>(&params.m_rnn_hidden_dim),    "RNN hidden dim")
  ("batch-size,b",      po::value<size_t>(&params.m_batch_size),        "Batch size")
  ("seq-len,q",         po::value<size_t>(&params.m_sequence_length),   "Sequence length")
  ("learning-rate,l",   po::value<float>(&params.m_learning_rate),        "Learning rate")
  ("weight-decay,d",    po::value<float>(&params.m_weight_decay),        "Weight Decay")
  ("beta-one",          po::value<float>(&params.m_beta_one),        "Beta One value of Adam Optimizer")
  ("beta-two",          po::value<float>(&params.m_beta_two),        "Beta Two value of Adam Optimizer")
  ("output-format,f",   po::value<std::string>(&params.m_output_format)->default_value("txt"), "The output format: txt or json")
  ("gpuid,g",           po::value<int>(&gpuid)->default_value(-1), "The id of the gpu to use. -1 for cpu.")
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
  params.train_ss = vm["sentence"].as<bool>();

  return train_segmentation_model(ud_path, corpus, params, gpuid);
}

using namespace deeplima;

int train_segmentation_model(const std::string& ud_path, const std::string& corpus,
                             deeplima::segmentation::train::train_params_segmentation_t &params, int gpuid)
{
  boost::filesystem::path path_to_treebank = boost::filesystem::path(ud_path) / corpus;
  CoNLLU::Treebank tb(path_to_treebank.string());

  return train_segmentation_model(tb, params, gpuid);
}

