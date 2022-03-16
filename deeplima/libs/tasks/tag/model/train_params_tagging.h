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

#ifndef DEEPLIMA_LIBS_TASKS_TAGGING_PARAMS_H
#define DEEPLIMA_LIBS_TASKS_TAGGING_PARAMS_H

#include <string>

#include "tasks/common/train_params.h"

namespace deeplima
{
namespace tagging
{
namespace train
{

struct train_params_tagging_t : public deeplima::train::train_params_t
{
  std::string m_embeddings_fn;

  std::string m_tasks_string;

  size_t m_conllu_column_num; // 4 - UPOS, 5 - XPOS

  size_t m_rnn_hidden_dim;
  size_t m_trainable_embeddings_dim;
  size_t m_trainable_embeddings_cutoff_freq; // ipm

  bool m_use_eos;

  size_t m_sequence_length;

  train_params_tagging_t()
    : m_tasks_string("upos"),
      m_conllu_column_num(4),
      m_rnn_hidden_dim(64),
      m_trainable_embeddings_dim(0),
      m_trainable_embeddings_cutoff_freq(100),
      m_use_eos(false),
      m_sequence_length(256) {}
};

} // namespace train
} // namespace tagging
} // namespace deeplima

#endif
