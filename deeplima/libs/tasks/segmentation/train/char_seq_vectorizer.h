// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_SRC_INCLUDE_SEGMENTATION_TRAIN_CHAR_SEQ_VECTORIZER_H
#define DEEPLIMA_SRC_INCLUDE_SEGMENTATION_TRAIN_CHAR_SEQ_VECTORIZER_H

#include <vector>
#include <string>

#include "deeplima/segmentation/impl/char_ngram.h"

namespace deeplima
{
namespace segmentation
{
namespace train
{

template <class InputEncoder, class Matrix, class Adapter>
class CharSeqVectorizerImpl : public Adapter
{
public:
  CharSeqVectorizerImpl(const std::vector<impl::ngram_descr_t>& ngram_descr)
    : Adapter(),
      m_input_encoder(ngram_descr)
  {
  }

  std::shared_ptr<Matrix> process(const std::string& text, int64_t len = -1)
  {
    assert(len > 0); // TODO: calculate length of text in characters in case len == -1
    return read_string(text, len);
  }

protected:
  std::shared_ptr<Matrix> read_string(const std::string& text, uint64_t len)
  {
    std::shared_ptr<Matrix> target(new Matrix(len, m_input_encoder.size()));

    uint64_t current_timepoint = 0;
    int32_t pos = 0;
    m_input_encoder.reset();

    while (! m_input_encoder.ready_to_generate())
    {
      m_input_encoder.warmup((const uint8_t*)text.data(), &pos, text.size());
    }

    while (size_t(pos) < text.size())
    {
      if (m_input_encoder.parse((const uint8_t*)text.data(), &pos, text.size()) > 0)
      {
        handle_timepoint(*target, current_timepoint);
      }
    }

    char final_spaces[] = " ";
    for (size_t i = 0; i < m_input_encoder.get_lookahead(); i++)
    {
      int32_t pos = 0;
      if (m_input_encoder.parse((uint8_t*)final_spaces, &pos, 1) > 0)
      {
        handle_timepoint(*target, current_timepoint);
      }
      else
      {
        throw std::runtime_error("Something wrong.");
      }
    }

    return target;
  }

  inline void handle_timepoint(Matrix& target, uint64_t& current_timepoint)
  {
    for (size_t i = 0; i < m_input_encoder.size(); i++)
    {
      uint64_t v = m_input_encoder.get_feat(i);
      Adapter::set(target, current_timepoint, i, v);
    }
    current_timepoint++;
    if (current_timepoint == std::numeric_limits<uint64_t>::max())
    {
      throw std::overflow_error("Too much characters in training set.");
    }
  }

  InputEncoder m_input_encoder;
};

} // namespace train
} // namespace segmentation
} // namespace deeplima

#endif
