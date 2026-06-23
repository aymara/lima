// Copyright 2022 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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

  bool m_input_includes_root;

  train_params_graph_dp_t()
    : m_tasks_string("arc"),
      m_rnn_hidden_dims({64}),
      m_trainable_embeddings_dim(0),
      m_trainable_embeddings_cutoff_freq(100),
      m_sequence_length(256),
      m_input_includes_root(true) {
    m_batch_size = 8;
  }
};

} // namespace train
} // namespace graph_dp
} // namespace deeplima

#endif
