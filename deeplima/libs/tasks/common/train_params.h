// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_LIBS_TASKS_COMMON_TRAIN_PARAMS_H
#define DEEPLIMA_LIBS_TASKS_COMMON_TRAIN_PARAMS_H

#include <string>
#include <boost/filesystem.hpp>

#include "conllu/treebank.h"

namespace deeplima
{
namespace train
{

struct train_params_t
{
  std::string m_output_model_name;
  std::string m_input_model_name; // pretrained model
  std::string m_train_set_fn;
  std::string m_dev_set_fn;
  std::map<std::string, std::string> m_tags;

  std::string m_device_string; // goes directly into Device struct constuctor

  float m_learning_rate;
  float m_weight_decay;

  float m_input_dropout_prob;

  size_t m_max_epochs;
  size_t m_max_epochs_without_improvement;
  size_t m_batch_size;  // sequences per iteration

  std::string m_optimizers;

  train_params_t(std::string device_string="cpu",
                 float learning_rate=0.001,
                 float weight_decay=0.00001,
                 size_t max_epochs=100,
                 size_t max_epochs_without_improvement=10,
                 size_t batch_size=4)
    : m_device_string(device_string),
      m_learning_rate(learning_rate),
      m_weight_decay(weight_decay),
      m_input_dropout_prob(0.3),
      m_max_epochs(max_epochs),
      m_max_epochs_without_improvement(max_epochs_without_improvement),
      m_batch_size(batch_size),
      m_optimizers("adam") {}

  virtual const std::string& get_train_set_fn() const
  {
    return m_train_set_fn;
  }

  virtual const std::string& get_dev_set_fn() const
  {
    return m_dev_set_fn;
  }

  virtual void guess_data_sets(const std::string& ud_path, const std::string& corpus);
};

} // namespace train
} // namespace deeplima

#endif
