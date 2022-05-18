// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_UTILS_STR_INDEX
#define DEEPLIMA_UTILS_STR_INDEX

#include <unordered_map>

namespace deeplima
{

template <typename S=std::string, typename I=uint32_t>
class TStringIndex
{
public:
  typedef S string_t;
  typedef I idx_t;

  inline idx_t get_idx(const char* p, size_t len)
  {
    std::string s = std::string(p, len);
    return get_idx(s);
  }

  inline idx_t get_idx(const string_t& str)
  {
    assert(m_str2idx.size() == m_idx2str.size());

    auto it = m_str2idx.find(str);
    if (m_str2idx.end() == it)
    {
      uint32_t idx = m_str2idx.size();
      m_idx2str[idx] = str;
      m_str2idx[str] = idx;

      assert(m_str2idx.size() == m_idx2str.size());

      return idx;
    }
    else
    {
      assert(m_str2idx.size() == m_idx2str.size());
      return it->second;
    }
  }

  inline const S& get_str(const idx_t idx) const
  {
    assert(m_str2idx.size() == m_idx2str.size());

    auto it = m_idx2str.find(idx);
    if (m_idx2str.end() == it)
    {
      throw std::runtime_error("Unknown index in StringIndex.");
    }
    else
    {
      return it->second;
    }
  }

  size_t size() const
  {
    return m_idx2str.size();
  }

protected:
  std::unordered_map<string_t, idx_t> m_str2idx;
  std::unordered_map<idx_t, string_t> m_idx2str;
};

typedef TStringIndex<std::string, uint32_t> StringIndex;

}

#endif
