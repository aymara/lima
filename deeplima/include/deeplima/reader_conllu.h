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

#ifndef DEEPLIMA_READER_CONLLU_H
#define DEEPLIMA_READER_CONLLU_H

#include "segmentation/impl/segmentation_impl.h"
#include "segmentation/impl/segmentation_decoder.h"
namespace deeplima
{
namespace segmentation
{

class FormattedReaderBase : public ISegmentation
{
public:

  virtual void register_handler(const segmentation_callback_t fn)
  {
    m_callback = fn;
  }

  virtual ~FormattedReaderBase() { }

protected:

  segmentation_callback_t m_callback;
};

class CoNLLUReader : public FormattedReaderBase
{
public:

  CoNLLUReader()
  {
    m_next = m_buffer;
    m_len = 0;
  }

  virtual ~CoNLLUReader() { }

  virtual void parse_from_stream(const read_callback_t fn)
  {
    uint32_t buffer_size = 1024*16;
    char buff[buffer_size];
    char* end = buff + buffer_size;
    bool continue_reading = true;
    uint32_t start_writting = 0;

    std::vector<token_pos> tokens(1024);
    size_t token_idx = 0;

    do
    {
      uint32_t bytes_read = 0;
      continue_reading = fn((uint8_t*)buff + start_writting, bytes_read, buffer_size - start_writting);
      if (0 == bytes_read)
      {
        break;
      }

      const char* p = buff;
      char* p_eol = nullptr;

      if (bytes_read < buffer_size - start_writting)
      {
        end = buff + start_writting + bytes_read;
      }

      //while ('\n' != *p && p < end) p++;

      while (p < end)
      {
        p_eol = (char*)memchr(p, '\n', end - p);
        if (nullptr == p_eol)
        {
          break;
          //p_eol = end;
        }

        if (*p == '\n')
        {
          p++;
          if (token_idx > 0)
          {
            token_pos& token = tokens[token_idx - 1];
            token.m_flags = token_pos::flag_t(token.m_flags | token_pos::flag_t::sentence_brk);
          }
          continue;
        }

        if (*p == '#')
        {
          p = p_eol + 1;
          continue;
        }

        if (read_token(tokens[token_idx], p, p_eol))
        {
          token_idx++;

          if (token_idx >= tokens.size())
          {
            m_callback(tokens, token_idx);
            token_idx = 0;
            m_next = m_buffer;
          }
        }

        p = p_eol + 1;
      }

      if (token_idx > 0)
      {
        m_callback(tokens, token_idx);
        token_idx = 0;
      }

      start_writting = 0;
      if (p < end /*&& nullptr == p_eol*/)
      {
        start_writting = end - p;
        memcpy(buff, p, start_writting);
      }

    } while (continue_reading);
  }

protected:
  inline bool read_token(token_pos& token, const char* p, const char* end, bool eos=false)
  {
    while (*p != '\t' && *p != '-' && *p != '.') p++;
    if (*p == '.') return false;

    token.clear();
    if (*p == '-')
    {
      const char *start = (const char*)memchr(p, '\t', end - p) + 1;
      assert('\n' != *start);
      m_len = (const char*)memchr(start, '\t', end - start) - start;
      assert(m_len > 0);
      memcpy(m_next, start, m_len);
      m_next[m_len] = 0;
      return false;
    }

    token.m_pch = (const char*)memchr(p, '\t', end - p) + 1;
    assert('\n' != *(token.m_pch));

    /*if ('_' == *token.m_pch)
    {
      if (m_len > 0)
      {
        token.m_pch = m_next;
        token.m_len = m_len;
        //m_next[m_len + 1] = 0;
        m_next += m_len + 1;
        m_len = 0;
      }
      else
      {
        return false;
      }
    }
    else
    {*/
      token.m_len = (const char*)memchr(token.m_pch, '\t', end - token.m_pch) - token.m_pch;
    /*}*/

    if (eos)
    {
      token.m_flags = token_pos::flag_t(token.m_flags | token_pos::flag_t::sentence_brk);
    }

    return true;
  }

protected:
  char m_buffer[1024 * 4];
  char* m_next;
  size_t m_len;
};

} // namespace segmentation
} // namespace deeplima

#endif
