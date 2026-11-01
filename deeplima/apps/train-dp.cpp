// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <functional>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include "deeplima/utils/split_string.h"
#include "conllu/treebank.h"
#include "tasks/graph_dp/train/train_graph_dp.h"

#include "version/version.h"

using namespace std;
namespace po = boost::program_options;

string get_datetime_str();

int main(int argc, char* argv[])
{
  setlocale(LC_ALL, "en_US.UTF-8");
  // std::cerr << "deeplima (git commit hash: " << deeplima::version::get_git_commit_hash() << ", "
  //      << "git branch: " << deeplima::version::get_git_branch()
  //      << ")" << std::endl;

  string corpus, ud_path;
  deeplima::graph_dp::train::train_params_graph_dp_t params;
  vector<string> m_raw_tags;
  string rnn_hidden_dims;

  po::options_description desc("deeplima (train graph dependency parsing model)");
  desc.add_options()
  ("help,h",                                                              "Display this help message")
  ("corpus,c",            po::value<string>(&corpus),                     "Training corpus name")
  ("ud-path,u",           po::value<string>(&ud_path),                    "Path to UD collection")
  ("name,n",              po::value<string>(&params.m_output_model_name), "Model name")
  ("pretrained,p",        po::value<string>(&params.m_input_model_name),  "Pretrained model name")
  ("embeddings,e",        po::value<string>(&params.m_embeddings_fn),     "File with embeddings (fastText .bin or .ftz)")
  ("train,t",             po::value<string>(&params.m_train_set_fn),      "File with training data")
  ("dev,d",               po::value<string>(&params.m_dev_set_fn),        "File with evaluation data")
  ("maxepoch,m",          po::value<size_t>(&params.m_max_epochs)->default_value(params.m_max_epochs), "Max epochs")
  ("max-epochs-without-improvement", po::value<size_t>(&params.m_max_epochs_without_improvement)->default_value(params.m_max_epochs_without_improvement),
                                                                          "Stop after this many epochs below the best dev score (and above its dev loss)")
  ("hidden-dim,w",        po::value<string>(&rnn_hidden_dims),            "RNN hidden dims (comma separated list: one value per layer)")
  ("device",              po::value<string>(&params.m_device_string),     "Computing device: (cpu|cuda)[:<device-index>]")
  ("tasks",               po::value<string>(&params.m_tasks_string),      "Tasks to train (comma separated list: (arc,rel)+)" )
  ("input-includes-root", po::value<bool>(&params.m_input_includes_root), "Add <ROOT> token before the first token of the sentence (true by default)")
  ("tag",                 po::value<vector<string>>(&m_raw_tags),         "Tags (plain text)")
  // Training only, except the dims: rel-dim dominates the analysis time
  // (labels x rel-dim^2 per word), arc-dim and tag-embd-dim count less
  ("batch-size,b",        po::value<size_t>(&params.m_batch_size)->default_value(params.m_batch_size), "Batch size (sentences)")
  ("opt,o",               po::value<string>(&params.m_optimizers)->default_value(params.m_optimizers), "Optimizers to use one after the other (comma separated list: (adam,sgd)+); sgd uses 1000 x learning-rate")
  ("learning-rate",       po::value<float>(&params.m_learning_rate)->default_value(params.m_learning_rate), "Learning rate")
  ("lr-decay",            po::value<float>(&params.m_lr_decay)->default_value(params.m_lr_decay), "Learning rate factor after each epoch below the best dev score")
  ("weight-decay",        po::value<float>(&params.m_weight_decay)->default_value(params.m_weight_decay), "Weight decay")
  ("beta-one",            po::value<float>(&params.m_beta_one)->default_value(params.m_beta_one), "Beta One value of Adam Optimizer")
  ("beta-two",            po::value<float>(&params.m_beta_two)->default_value(params.m_beta_two), "Beta Two value of Adam Optimizer")
  ("clip",                po::value<float>(&params.m_clip)->default_value(params.m_clip), "Gradient norm clipping (0: none)")
  ("input-dropout",       po::value<float>(&params.m_input_dropout_prob)->default_value(params.m_input_dropout_prob), "Dropout on the BiLSTM input")
  ("rnn-dropout",         po::value<float>(&params.m_rnn_dropout)->default_value(params.m_rnn_dropout), "Dropout between BiLSTM layers")
  ("decoder-dropout",     po::value<float>(&params.m_decoder_dropout)->default_value(params.m_decoder_dropout), "Dropout on the BiLSTM output, before the arc and label decoders")
  ("word-dropout",        po::value<float>(&params.m_word_dropout)->default_value(params.m_word_dropout), "Probability to zero a word's form embedding")
  ("tag-embd-dim",        po::value<size_t>(&params.m_tag_embd_dim)->default_value(params.m_tag_embd_dim), "Embedding dim of each tag input (upos, each feature)")
  ("arc-dim",             po::value<size_t>(&params.m_arc_dim)->default_value(params.m_arc_dim), "Hidden dim of the arc decoder")
  ("rel-dim",             po::value<size_t>(&params.m_rel_dim)->default_value(params.m_rel_dim), "Hidden dim of the label decoder (its analysis cost grows with its square)")
  ("select-on",           po::value<string>(&params.m_select_on)->default_value(params.m_select_on), "Dev metric that selects the saved model: arc (UAS) or las")
  ("min-sentence-len",    po::value<size_t>(&params.m_min_sentence_len)->default_value(params.m_min_sentence_len), "Skip shorter sentences (words)")
  ("seed",                po::value<uint64_t>(&params.m_seed)->default_value(params.m_seed), "Seed of the initialisation, dropout and batch sampling (0: not seeded)")
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

  if (vm.count("help"))
  {
      cout << desc << endl;
      return 0;
  }

  try
  {
    params.m_rnn_hidden_dims = deeplima::utils::split_list_of_nums(rnn_hidden_dims, ',');
  }
  catch (const std::exception &e)
  {
    cerr << e.what() << endl;
    return -1;
  }

  if (params.m_rnn_hidden_dims.empty())
  {
    cerr << "List of RNN hidden dims must be non-empty" << endl << endl;
    cout << desc << endl;
    return -1;
  }

  if (params.get_train_set_fn().empty() || params.get_dev_set_fn().empty())
  {
    if (ud_path.empty())
    {
      cerr << "No training data given: --train / --dev or --corpus parameters are required." << endl << endl;
      cout << desc << endl;
      return -1;
    }
    params.guess_data_sets(ud_path, corpus);
  }

  if (params.get_train_set_fn().empty() || params.get_dev_set_fn().empty())
  {
    cerr << "Can't find training data." << endl;
    return -1;
  }

  params.m_tags["git_commit_hash"] = deeplima::version::get_git_commit_hash();
  params.m_tags["git_branch"] = deeplima::version::get_git_branch();
  params.m_tags["training_start_datetime"] = get_datetime_str();

  string cmd_line;
  for (int i = 0; i < argc; i++)
  {
    if (!cmd_line.empty())
    {
      cmd_line += " ";
    }
    cmd_line += argv[i];
  }
  params.m_tags["training_cmd_line"] = cmd_line;

  for ( const string& s : m_raw_tags )
  {
    if (s.empty())
    {
      continue;
    }

    string::size_type pos = s.find('=');
    string k, v;
    if (string::npos == pos)
    {
      k = s;
    }
    else
    {
      k = s.substr(0, pos);
      v = s.substr(pos+1);
    }

    if (params.m_tags.end() != params.m_tags.find(k))
    {
      cerr << "Duplicated tags \"" << k << "\"" << endl;
      return -1;
    }

    params.m_tags[k] = v;
  }

  return deeplima::graph_dp::train::train_graph_dp(params);
}

string get_datetime_str()
{
  time_t t = time(nullptr);
  tm *ptm = localtime(&t);
  assert(nullptr != ptm);
  ostringstream oss;
  oss << put_time(ptm, "%Y-%m-%d %H-%M-%S");
  return oss.str();
}

