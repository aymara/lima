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

