// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_APPS_SEGMENTATION_DECODER_H
#define DEEPLIMA_APPS_SEGMENTATION_DECODER_H

#include <iostream>
#include <functional>
#include <stdexcept>
#include <string>
#include <vector>

#include <unicode/uchar.h>

#include "deeplima/utils/std_matrix.h"

namespace deeplima
{
namespace segmentation
{

struct token_pos
{
  enum flag_t : uint8_t
  {
    none = 0x00,
    sentence_brk = 0x01,
    paragraph_brk = 0x02,
    max_flags
  };

  uint16_t m_offset; // offset from previous token end
  uint16_t m_len;    // length of this token in bytes
  const char* m_pch;
  flag_t m_flags;

  token_pos()
    : m_offset(0), m_len(0), m_pch(nullptr), m_flags(none) {}

  inline void clear()
  {
    m_offset = m_len = 0;
    m_pch = nullptr;
    m_flags = none;
  }

  inline bool empty() const
  {
    return (0 == m_offset) && (0 == m_len) && (nullptr == m_pch);
  }

  inline bool too_long() const
  {
    return m_len & (1 << (sizeof(uint16_t) * 8 - 1));
  }
};

typedef std::function < void (const std::vector<token_pos>& tokens,
                              uint32_t len) > segmentation_callback_t;

enum segm_tag_t : uint8_t
{
  // tokenization-only tags
  X           = 0x00,
  B           = 0x01,
  I           = 0x02,
  E           = 0x03,
  S           = 0x04,
  max_tok_tag = 0x05,

  // + sentence segmentation tags
  E_EOS       = 0x05,
  S_EOS       = 0x06,
  max_segm_tag
};

namespace impl
{

template <uint8_t M = 4>
class CharReader
{
public:

  CharReader()
    : m_buff_len(0)
  {
    memset(m_buff, 0, M);
  }

  inline bool read(const char** pch, const char* end, uint8_t len)
  {
    assert(*pch < end);
    assert(len <= M);
    assert(m_buff_len <= M);

    size_t available_bytes = end - *pch;

    if (available_bytes < len)
    {
      assert(m_buff_len + available_bytes <= M);
      memcpy(m_buff + m_buff_len, *pch, available_bytes);
      m_buff_len += available_bytes;
      *pch += available_bytes;
      assert(m_buff_len <= M);
      return false;
    }

    return true;
  }

  inline bool not_empty() const
  {
    return m_buff_len > 0;
  }

  inline bool start_reading(const char** pch,
                            const char* end,
                            uint8_t len,                // length of the current character in bytes
                            std::vector<uint8_t>& dst,  // destination buffer
                            token_pos& tmp,
                            bool inside)
  {
    assert(*pch < end);
    assert(len <= M);
    assert(m_buff_len <= M);

    if (m_buff_len + (end - *pch) < len)
    {
      // full character isn't still available
      return read(pch, end, len);
    }

    // TODO: Don't trust ML model.
    // This code skips non-space characters
    // in case of ML errors (wrong X tag).
    if (inside)
    {
      if (dst.size() < size_t(tmp.m_len + len + 1))
      {
        dst.resize(tmp.m_len + len + 1);
      }

      memcpy(dst.data() + tmp.m_len, m_buff, m_buff_len);
      memcpy(dst.data() + tmp.m_len + m_buff_len, *pch, len - m_buff_len);

      tmp.m_len += len;
      tmp.m_pch = (const char*)dst.data();
    }
    else
    {
      tmp.m_offset += len;
    }

    m_buff_len = 0;

    return true;
  }

protected:
  char m_buff[M];
  uint8_t m_buff_len;
};

class SegmentationDecoder : public CharReader<>
{
public:

  SegmentationDecoder(std::shared_ptr< StdMatrix<uint8_t> > out, const std::vector<uint8_t>& len)
    : m_out(out),
      m_len(len)
  {
    init();
  }

  void init()
  {
    m_tokens.resize(1024);
  }

  void register_handler(const segmentation_callback_t fn)
  {
    m_callback = fn;
  }

  inline void save_current_token(size_t& pos, size_t& temp_token_len, const char* start)
  {
    if (m_tokens[pos].m_len > 0)
    {
      if (m_tokens[pos].m_pch == (const char*)m_temp_text.data())
      {
        assert(0 == pos);
        if (m_temp_text.size() < size_t(m_tokens[pos].m_len + 1))
        {
          m_temp_text.resize(m_tokens[pos].m_len + 1);
        }
        memcpy(m_temp_text.data() + temp_token_len, start, m_tokens[pos].m_len - temp_token_len);
        m_tokens[pos].m_pch = (const char*)(m_temp_text.data());
      }
      pos++;
      if (pos >= m_tokens.size())
      {
        m_tokens.resize(m_tokens.size() + 1024);
      }
      m_tokens[pos].clear();
    }
  }

  inline void consome_character(size_t& pos, uint64_t from, const char* pch)
  {
    if (nullptr == m_tokens[pos].m_pch)
    {
      assert(0 == m_tokens[pos].m_len);
      m_tokens[pos].m_pch = pch;
    }
    m_tokens[pos].m_len += m_len[from];
  }

  inline uint64_t decode(const char** pch, uint32_t max, uint64_t from, uint64_t to)
  {
    assert(nullptr != pch);
    assert(nullptr != *pch);
    const char* start = *pch;
    const char* end = *pch + max;

    size_t pos = 0;
    size_t temp_token_len = m_tokens[0].m_len;

    if (not_empty())
    {
      uint8_t bytes_stored = m_buff_len;
      if (start_reading(pch, end, m_len[from], m_temp_text, m_tokens[0], m_out->get(from, 0)))
      {
        *pch += m_len[from] - bytes_stored;
        start += m_len[from] - bytes_stored;
        if (m_out->get(from, 0) != segm_tag_t::X)
        {
          temp_token_len += m_len[from];
        }
        from++;
      }
      else
      {
        // can't read full character (we need more data)
        return from;
      }
    }

    while (from < to && *pch < end)
    {
      if (! read(pch, end, m_len[from]))
      {
        break;
      }

//#ifndef NDEBUG
//      std::cerr << "[" << from << "]==" << (int)(m_out->get(from, 0)) << std::endl;
//#endif
      int8_t gen_cat = 0;
      UChar uch;
      int32_t zero = 0;
      // TODO: move U8_NEXT from here (we don't handle UTF-8 while decoding)
      U8_NEXT(*pch, zero, m_len[from], uch);
      gen_cat = u_charType(uch);

      auto tag = m_out->get(from, 0);

      switch (tag)
      {
        case segm_tag_t::X:
          save_current_token(pos, temp_token_len, start);

          if (gen_cat == U_SPACE_SEPARATOR || gen_cat == U_LINE_SEPARATOR
              || gen_cat == U_PARAGRAPH_SEPARATOR || gen_cat == U_CONTROL_CHAR
              || gen_cat == U_FORMAT_CHAR)
          {
            m_tokens[pos].m_offset += m_len[from];
          }
          else
          {
            m_tokens[pos].m_pch = *pch;
            m_tokens[pos].m_len += m_len[from];
            save_current_token(pos, temp_token_len, start);
          }
          break;

        case segm_tag_t::B:
          save_current_token(pos, temp_token_len, start);

          if (gen_cat == U_SPACE_SEPARATOR || gen_cat == U_PARAGRAPH_SEPARATOR
              || gen_cat == U_LINE_SEPARATOR || gen_cat == U_CONTROL_CHAR
              || gen_cat == U_FORMAT_CHAR)
          {
            m_tokens[pos].m_offset += m_len[from];
            break;
          }
          else
          {
            assert(0 == m_tokens[pos].m_len);
            m_tokens[pos].m_pch = *pch;
          }

        case segm_tag_t::I:
          if (0 == m_tokens[pos].m_len
              && (gen_cat == U_SPACE_SEPARATOR || gen_cat == U_PARAGRAPH_SEPARATOR
                  || gen_cat == U_LINE_SEPARATOR || gen_cat == U_CONTROL_CHAR
                  || gen_cat == U_FORMAT_CHAR))
          {
            m_tokens[pos].m_offset += m_len[from];
          }
          else
          {
            consome_character(pos, from, *pch);
          }
          break;

        // TODO insert the marker for case continuing [[case_]]
        case segm_tag_t::E_EOS:
          m_tokens[pos].m_flags = token_pos::flag_t(m_tokens[pos].m_flags | token_pos::flag_t::sentence_brk);

        case segm_tag_t::E:
          if (0 == m_tokens[pos].m_len
              && (gen_cat == U_SPACE_SEPARATOR || gen_cat == U_PARAGRAPH_SEPARATOR
                  || gen_cat == U_LINE_SEPARATOR || gen_cat == U_CONTROL_CHAR
                  || gen_cat == U_FORMAT_CHAR))
          {
            m_tokens[pos].m_offset += m_len[from];
          }
          else
          {
            consome_character(pos, from, *pch);
            save_current_token(pos, temp_token_len, start);
          }
          break;

        case segm_tag_t::S_EOS:
          save_current_token(pos, temp_token_len, start);

          if (gen_cat == U_SPACE_SEPARATOR || gen_cat == U_PARAGRAPH_SEPARATOR
              || gen_cat == U_LINE_SEPARATOR || gen_cat == U_CONTROL_CHAR
              || gen_cat == U_FORMAT_CHAR)
          {
            m_tokens[pos].m_offset += m_len[from];
          }
          else
          {
            assert(0 == m_tokens[pos].m_len);
            m_tokens[pos].m_pch = *pch;
            m_tokens[pos].m_len += m_len[from];
            m_tokens[pos].m_flags = token_pos::flag_t(m_tokens[pos].m_flags | token_pos::flag_t::sentence_brk);
            save_current_token(pos, temp_token_len, start);
          }
          break;

        case segm_tag_t::S:
          save_current_token(pos, temp_token_len, start);

          if (gen_cat == U_SPACE_SEPARATOR || gen_cat == U_PARAGRAPH_SEPARATOR
              || gen_cat == U_LINE_SEPARATOR || gen_cat == U_CONTROL_CHAR
              || gen_cat == U_FORMAT_CHAR)
          {
            m_tokens[pos].m_offset += m_len[from];
          }
          else
          {
            assert(0 == m_tokens[pos].m_len);
            m_tokens[pos].m_pch = *pch;
            m_tokens[pos].m_len += m_len[from];
            save_current_token(pos, temp_token_len, start);
          }
          break;

        default:
          throw std::runtime_error("Unknown code in output.");
      }

      if (m_tokens[pos].too_long())
      {
        // This is a workaround to handle garbage in the input data
        // very long (and meaningless) tokens are artificially splitted
        // into several parts.
        // TODO: the same type of handling is required for very long
        // sequence of spaces: an empty token must be generated
        // to avoid overflow of token_pos::m_offset field.
        save_current_token(pos, temp_token_len, start);
      }

      *pch += m_len[from]; // next byte
      from++; // next character
    }

    if (pos > 0)
    {
      m_callback(m_tokens, pos);
    }
    else
    {
      if (m_tokens[0].m_pch == (const char*)m_temp_text.data())
      {
        if (m_temp_text.size() < size_t(m_tokens[0].m_len + 1))
        {
          m_temp_text.resize(m_tokens[0].m_len + 1);
        }
        assert(0 == pos);
        assert(m_tokens[pos].m_len >= temp_token_len);
        memcpy(m_temp_text.data() + temp_token_len, start, m_tokens[pos].m_len - temp_token_len);
        m_tokens[pos].m_pch = (const char*)(m_temp_text.data());
      }
      // std::cerr << std::endl;
    }

    if (!m_tokens[pos].empty())
    {
      if (pos > 0)
      {
        m_tokens[0] = m_tokens[pos];
      }
      if (m_temp_text.size() < size_t(m_tokens[0].m_len + 1))
      {
        m_temp_text.resize(m_tokens[0].m_len + 1);
      }
      if ((const char*)m_temp_text.data() != m_tokens[0].m_pch)
      {
        if (m_tokens[0].m_pch == nullptr)
        {
          std::cerr << std::string("Got null pointer for m_pch at ")+ __FILE__+":"+std::to_string(__LINE__) << std::endl;
          std::cerr << std::string("We ask to copy ") << m_tokens[0].m_len << " bytes from there." << std::endl;
          // throw std::runtime_error((std::string("Got null pointer for m_pch at ")+ __FILE__+":"+std::to_string(__LINE__)).c_str());
        }
        memcpy(m_temp_text.data(), m_tokens[0].m_pch, m_tokens[0].m_len);
        m_tokens[0].m_pch = (const char*)(m_temp_text.data());
      }
    }
    else
    {
      m_tokens[0].clear();
    }

    return from;
  }

protected:
  // input
  std::shared_ptr< StdMatrix<uint8_t> > m_out;
  const std::vector<uint8_t>& m_len;

  // output
  std::vector<token_pos> m_tokens;

  // callback
  segmentation_callback_t m_callback;

  // temp buffers
  std::vector<uint8_t> m_temp_text;
};

} // namespace impl
} // namespace segmentation
} // namespace deeplima

#endif
