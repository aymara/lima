// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
