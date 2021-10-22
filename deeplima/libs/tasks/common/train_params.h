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

  size_t m_max_epochs;
  size_t m_max_epochs_without_improvement;
  size_t m_batch_size;  // sequences per iteration

  train_params_t(std::string device_string="cpu",
                 float learning_rate=0.001,
                 float weight_decay=0.00001,
                 size_t max_epochs=100,
                 size_t max_epochs_without_improvement=10,
                 size_t batch_size=4)
    : m_device_string(device_string),
      m_learning_rate(learning_rate),
      m_weight_decay(weight_decay),
      m_max_epochs(max_epochs),
      m_max_epochs_without_improvement(max_epochs_without_improvement),
      m_batch_size(batch_size) {}

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
