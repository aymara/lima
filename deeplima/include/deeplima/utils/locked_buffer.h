// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_APPS_LOCKED_BUFFER_H
#define DEEPLIMA_APPS_LOCKED_BUFFER_H

struct locked_buffer_t
{
  uint8_t* m_data;
  uint32_t m_len;
  uint32_t m_lock_count;
  const char* m_char_aligned_data;

  inline uint8_t* end() const
  {
    if (nullptr == m_char_aligned_data)
    {
      return nullptr;
    }
    return (uint8_t*)m_char_aligned_data + m_len;
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
    m_len = 0;
    m_char_aligned_data = nullptr;
    assert(m_lock_count > 0);
    m_lock_count--;
  }

  inline void set_read_start(const char* new_start)
  {
    assert((uint8_t*)new_start < end());
    m_len -= (new_start - m_char_aligned_data);
    m_char_aligned_data = new_start;
  }

  locked_buffer_t()
    : m_data(nullptr),
      m_len(0),
      m_lock_count(0),
      m_char_aligned_data(nullptr)
  {}
};

struct locked_buffer_set_t
{
  std::vector<locked_buffer_t> m_data;
  uint8_t* m_mem;
  uint32_t m_max_buff_size;

  locked_buffer_set_t(size_t n = 0, uint32_t buffer_size = 0)
    : m_mem(nullptr),
      m_max_buff_size(buffer_size)
  {}

  void init(size_t n, uint32_t buffer_size)
  {
    assert(n > 0);
    assert(buffer_size > 0);

    m_max_buff_size = buffer_size;

    m_data.resize(n);
    m_mem = new uint8_t [ 8 + n * buffer_size ];

    if (nullptr == m_mem)
    {
      throw std::runtime_error("Out of memory.");
    }

    uint8_t* p_base = m_mem + 8;

    for (size_t i = 0; i < m_data.size(); i++)
    {
      m_data[i].m_data = p_base + (i * buffer_size);
      m_data[i].m_len = buffer_size;
    }
  }

  inline uint32_t max_buff_size() const
  {
    return m_max_buff_size;
  }

  inline size_t next(size_t n) const
  {
    return (m_data.size() == (n + 1)) ? 0 : (n + 1);
  }

  inline uint32_t bytes_available(size_t n)
  {
    uint32_t s = 0;
    for (size_t i = n; i < m_data.size(); i++)
    {
      s += m_data[i].m_len;
    }
    return s;
  }

  inline size_t size() const
  {
    return m_data.size();
  }

  inline locked_buffer_t& get(size_t n)
  {
    assert(n < m_data.size());
    return m_data[n];
  }

  ~locked_buffer_set_t()
  {
    if (nullptr != m_mem)
    {
      delete[] m_mem;
    }
  }

  void pretty_print()
  {
    std::cerr << "BUFFS: ";
    for (size_t i = 0; i < m_data.size(); i++)
    {
      std::cerr << " | " << m_data[i].m_lock_count;
    }
    std::cerr << " |" << std::endl;
  }
};

#endif
