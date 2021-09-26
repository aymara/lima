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
#include "tasks/ner/train/train_ner.h"

using namespace std;
namespace po = boost::program_options;

int main(int argc, char* argv[])
{
  setlocale(LC_ALL, "en_US.UTF-8");

  string corpus, ud_path;
  deeplima::tagging::train::train_params_tagging_t params;

  po::options_description desc("DeepLima - train tagging");
  desc.add_options()
  ("help,h",                                                            "Display this help message")
  ("corpus,c",          po::value<string>(&corpus),                     "Training corpus name")
  ("ud-path,u",         po::value<string>(&ud_path),                    "Path to UD collection")
  ("name,n",            po::value<string>(&params.m_output_model_name), "Model name")
  ("pretrained,p",      po::value<string>(&params.m_input_model_name),  "Pretrained model name")
  ("embeddings,e",      po::value<string>(&params.m_embeddings_fn),     "File with embeddings (fastText .bin or .ftz)")
  ("train,t",           po::value<string>(&params.m_train_set_fn),      "File with training data")
  ("dev,d",             po::value<string>(&params.m_dev_set_fn),        "File with evaluation data")
  ("maxepoch,m",        po::value<size_t>(&params.m_max_epochs),        "Max epochs")
  ("hidden-dim,w",      po::value<size_t>(&params.m_rnn_hidden_dim),    "RNN hidden dim")
  ("device",            po::value<string>(&params.m_device_string),     "Computing device: (cpu|cuda)[:<device-index>]")
  ("tasks",             po::value<string>(&params.m_tasks_string),      "Tasks to train (comma separated list: (upos,feats,xpos)+)" )
  ("tag",               po::value<vector<string>>(&params.m_tags),      "Tags (plain text)")
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

  if (params.get_train_set_fn().empty() || params.get_dev_set_fn().empty())
  {
    if (ud_path.empty())
    {
      cerr << "No training data given: --train / --dev or --corpus parameters are required." << endl;
      return -1;
    }
    params.guess_data_sets(ud_path, corpus);
  }

  if (params.get_train_set_fn().empty() || params.get_dev_set_fn().empty())
  {
    cerr << "Can't find training data." << endl;
    return -1;
  }

  return deeplima::tagging::train::train_entity_tagger(params);
}
