// Copyright 2002-2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_LIBS_TASKS_LEMMATIZATION_PARAMS_H
#define DEEPLIMA_LIBS_TASKS_LEMMATIZATION_PARAMS_H

#include <string>

#include "tasks/common/train_params.h"

namespace deeplima
{
namespace lemmatization
{
namespace train
{

struct train_params_lemmatization_t : public deeplima::train::train_params_t
{
  // Encoder
  size_t m_encoder_rnn_hidden_dim;
  size_t m_encoder_embd_dim;

  // Decoder
  size_t m_decoder_rnn_hidden_dim;
  size_t m_decoder_embd_dim;

  size_t m_sequence_length;

  train_params_lemmatization_t()
    : m_encoder_rnn_hidden_dim(256),
      m_encoder_embd_dim(128),
      m_decoder_rnn_hidden_dim(256),
      m_decoder_embd_dim(128),
      m_sequence_length(64)
  {
    m_batch_size = 16;
  }
};

} // namespace train
} // namespace lemmatization
} // namespace deeplima

#endif

