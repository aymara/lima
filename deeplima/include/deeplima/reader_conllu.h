// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
    uint32_t buffer_size = 1024*64;
    char* buff = new char[buffer_size];
    char* end = buff + buffer_size;
    bool continue_reading = true;
    uint32_t start_writting = 0;
    uint32_t reserved_for_lookahead = 1024*2;
    // longest last token line in UD 2.8 collection.

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

      if (!continue_reading || p >= end - reserved_for_lookahead)
      {
        reserved_for_lookahead = 0;
      }

      while (p < end - reserved_for_lookahead)
      {
        p_eol = (char*)memchr(p, '\n', end - p);
        if (nullptr == p_eol)
        {
          break;
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
            // Lookahead for the possible sentence break
            if (end - p_eol > 1) {
              const char* p_after_eol = p_eol + 1;
              if (*p_after_eol == '\n') {
                token_pos& token = tokens[tokens.size() - 1];
                token.m_flags = token_pos::flag_t(token.m_flags | token_pos::flag_t::sentence_brk);
              }
            }

            m_callback(tokens, token_idx);
            token_idx = 0;
            m_next = m_buffer;
          }
        }

        p = p_eol + 1;
      }

      if (token_idx > 0)
      {
        // Lookahead for the possible sentence break
        if (p < end && *p == '\n')
        {
          p++;

          token_pos& token = tokens[token_idx - 1];
          token.m_flags = token_pos::flag_t(token.m_flags | token_pos::flag_t::sentence_brk);
        }
        m_callback(tokens, token_idx);
        token_idx = 0;
      }

      start_writting = 0;
      if (p < end)
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
      const char *start = (const char*)memchr(p, '\t', end - p);
      assert(nullptr != start);
      start += 1;
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
