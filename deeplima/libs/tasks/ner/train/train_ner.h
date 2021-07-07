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

#ifndef DEEPLIMA_LIBS_TASKS_NER_TRAIN_TRAIN_NER_H
#define DEEPLIMA_LIBS_TASKS_NER_TRAIN_TRAIN_NER_H

#include <string>

#include "conllu/treebank.h"

namespace deeplima
{
namespace tagging
{
namespace train
{

struct train_params_tagging_t
{
  std::string m_output_model_name;
  std::string m_input_model_name; // pretrained model
  std::string m_embeddings_fn;
  std::string m_train_set_fn;
  std::string m_dev_set_fn;

  size_t m_conllu_column_num; // 4 - UPOS, 5 - XPOS

  size_t m_rnn_hidden_dim;
  size_t m_trainable_embeddings_dim;
  size_t m_trainable_embeddings_cutoff_freq; // ipm

  bool m_use_eos;

  float m_learning_rate;
  float m_weight_decay;

  size_t m_max_epochs;
  size_t m_max_epochs_without_improvement;
  size_t m_batch_size;  // sequences per iteration
  size_t m_sequence_length;

  train_params_tagging_t()
    : m_conllu_column_num(4),
      m_rnn_hidden_dim(64),
      m_trainable_embeddings_dim(4),
      m_trainable_embeddings_cutoff_freq(100),
      m_use_eos(true),
      m_learning_rate(0.001),
      m_weight_decay(0.00001),
      m_max_epochs(100),
      m_max_epochs_without_improvement(4),
      m_batch_size(4),
      m_sequence_length(256) {}
};

int train_entity_tagger(const train_params_tagging_t& params);

} // namespace train
} // namespace tagging
} // namespace deeplima

#endif
