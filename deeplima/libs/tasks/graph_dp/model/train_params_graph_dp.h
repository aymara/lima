/*
    Copyright 2022 CEA LIST

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

#ifndef DEEPLIMA_LIBS_TASKS_GRAPH_DP_PARAMS_H
#define DEEPLIMA_LIBS_TASKS_GRAPH_DP_PARAMS_H

#include <string>

#include "tasks/common/train_params.h"

namespace deeplima
{
namespace graph_dp
{
namespace train
{

struct train_params_graph_dp_t : public deeplima::train::train_params_t
{
  std::string m_embeddings_fn;

  std::string m_tasks_string;

  std::vector<size_t> m_rnn_hidden_dims;
  size_t m_trainable_embeddings_dim;
  size_t m_trainable_embeddings_cutoff_freq; // ipm

  size_t m_sequence_length;

  train_params_graph_dp_t()
    : m_tasks_string("arc"),
      m_rnn_hidden_dims({64}),
      m_trainable_embeddings_dim(0),
      m_trainable_embeddings_cutoff_freq(100),
      m_sequence_length(256) {
    m_batch_size = 8;
  }
};

} // namespace train
} // namespace graph_dp
} // namespace deeplima

#endif
