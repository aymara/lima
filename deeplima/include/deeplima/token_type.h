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
