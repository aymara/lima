// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_TOKEN_TYPE_H
#define DEEPLIMA_TOKEN_TYPE_H

#include <vector>

namespace deeplima
{

namespace impl
{
struct token_t
{
  enum token_flags_t : uint8_t
  {
    none = 0x00,
    sentence_brk = 0x01,
    paragraph_brk = 0x02,
    max_flags
  };

  inline bool eos() const
  {
    return m_flags & sentence_brk;
  }

  token_t(uint16_t offset = 0, uint16_t len = 0, uint32_t idx = 0, token_flags_t flags = token_flags_t::none)
    : m_offset(offset), m_len(len), m_form_idx(idx), m_flags(flags) { }

  uint16_t m_offset; // offset from previous token end
  uint16_t m_len;    // length of this token in bytes
  uint32_t m_form_idx;
  token_flags_t m_flags;
};
}

struct token_buffer_t : public std::vector<impl::token_t>
{
protected:
  uint32_t m_lock_count;

public:
  typedef impl::token_t token_t;

  token_buffer_t()
    : m_lock_count(0) { }

  token_buffer_t(size_t size)
    : m_lock_count(0)
  {
    resize(size);
  }

  inline bool locked() const
  {
    return m_lock_count > 0;
  }

  inline void lock()
  {
    m_lock_count++;
  }

  inline void unlock()
  {
    assert(m_lock_count > 0);
    m_lock_count--;
  }
};

} // namespace deeplima

#endif
