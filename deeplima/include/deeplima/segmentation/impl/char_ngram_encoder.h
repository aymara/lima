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

#ifndef DEEPLIMA_SEGMENTATION_IMPL_CHAR_NGRAM_ENCODER_H
#define DEEPLIMA_SEGMENTATION_IMPL_CHAR_NGRAM_ENCODER_H

#include <sstream>
#include <vector>
#include <tuple>
#include <cassert>
#include <limits>

#ifndef NDEBUG
#include <iostream>
#endif

#include "utils/pretty.h"

#include "char_ngram.h"

namespace deeplima
{
namespace segmentation
{
namespace impl
{

#define ONE_POS_MASK(BITS_PER_POS, VAR_TYPE) \
  ( (BITS_PER_POS == sizeof(VAR_TYPE) * 8) ? ~((VAR_TYPE)(0)) : ((1 << BITS_PER_POS) - 1) )

template <class StreamDecoder>
class CharNgramEncoder : public StreamDecoder
{
public:
  CharNgramEncoder()
    : m_lookahead(0),
      m_chars_to_consume(0)
  {}

  explicit CharNgramEncoder(const std::vector<ngram_descr_t>& ngram_descr)
    : m_ngram_descr(ngram_descr),
      m_lookahead(0),
      m_chars_to_consume(0)
  {
    init();
  }

  void init(const std::vector<ngram_descr_t>& ngram_descr)
  {
    assert(m_ngram_descr.size() == 0);
    m_ngram_descr = ngram_descr;
    init();
  }

  inline void reset()
  {
    StreamDecoder::reset();
    m_chars_to_consume = m_lookahead;
  }

  inline size_t size() const
  {
    return m_ngram_descr.size();
  }

  inline uint64_t get_feat(size_t idx) const
  {
    assert(idx < m_ngram_descr.size());
    return (StreamDecoder::get_buffer(m_ngram_descr[idx].m_type) & m_mask[idx]) >> m_shift[idx];
  }

  inline uint8_t get_len() const
  {
    return StreamDecoder::get_len(m_lookahead);
  }

  inline uint8_t get_len(uint8_t idx) const
  {
    return StreamDecoder::get_len(idx);
  }

  inline uint8_t get_lookahead() const
  {
    return m_lookahead;
  }

  inline bool ready_to_generate() const
  {
    return m_chars_to_consume == 0;
  }

  inline bool allow_unk(uint8_t idx) const
  {
    assert(idx < m_ngram_descr.size());
    return StreamDecoder::bits_per_position(m_ngram_descr[idx].m_type) != 1;
  }

  inline uint8_t warmup(const uint8_t* str, int32_t& pos, int32_t len)
  {
    assert(!ready_to_generate());
    uint8_t char_len = StreamDecoder::parse(str, pos, len);
    if (0 == char_len)
    {
      throw std::runtime_error("Something wrong with stream decoder.");
    }
    m_chars_to_consume--;
    return char_len;
  }

protected:

  void init()
  {
    // calculate m_lookahead
    for (size_t i = 0; i < m_ngram_descr.size(); i++)
    {
      const ngram_descr_t& nd = m_ngram_descr[i];
      assert(nd.m_length > 0);
      uint8_t l = nd.m_length + nd.m_offset - 1;
      if (l > m_lookahead)
      {
        m_lookahead = l;
      }
    }

    m_chars_to_consume = m_lookahead;

    // calculate masks
    m_mask.resize(m_ngram_descr.size());
    m_shift.resize(m_ngram_descr.size());
    for (size_t i = 0; i < m_ngram_descr.size(); i++)
    {
      const ngram_descr_t& nd = m_ngram_descr[i];
      int8_t l = nd.m_length + nd.m_offset - 1;
      typename StreamDecoder::buffer_t one_pos_mask
          = ONE_POS_MASK(StreamDecoder::bits_per_position(nd.m_type), typename StreamDecoder::buffer_t);

#ifndef NDEBUG
      std::cerr << "one_pos_mask == " << pretty_bits_to_string(one_pos_mask) << std::endl;
#endif

      typename StreamDecoder::buffer_t mask = 0;

      for (size_t j = 0; j < nd.m_length; j++)
      {
        mask = (mask << StreamDecoder::bits_per_position(nd.m_type)) | one_pos_mask;
      }

      for (size_t j = 0; j < (m_lookahead - l); j++)
      {
        mask <<= StreamDecoder::bits_per_position(nd.m_type);
      }

      m_mask[i] = mask;
      m_shift[i] = StreamDecoder::bits_per_position(nd.m_type) * (m_lookahead - l);

#ifndef NDEBUG
      std::cerr << "mask  [" << i << "]    == " << pretty_bits_to_string(m_mask[i]) << std::endl;
      std::cerr << "shift [" << i << "]    == " << (uint32_t)m_shift[i] << std::endl;
#endif
    }

    assert(m_ngram_descr.size() == m_mask.size());
    assert(m_ngram_descr.size() == m_shift.size());
  }

  std::vector<ngram_descr_t> m_ngram_descr;
  uint8_t m_lookahead; // can't be negative
  uint8_t m_chars_to_consume; // before encoder can start to generate

  std::vector<typename StreamDecoder::buffer_t> m_mask;
  std::vector<uint8_t> m_shift;
};

} // namespace impl
} // namespace segmentation
} // namespace deeplima

#endif
