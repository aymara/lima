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

#ifndef DEEPLIMA_UTILS_STR_INDEX
#define DEEPLIMA_UTILS_STR_INDEX

#include <unordered_map>
#include <codecvt>
#include <locale>

namespace deeplima
{

template <typename S=std::string, typename U=std::u32string, typename I=uint32_t>
class TStringIndex
{
public:
  typedef S string_t;
  typedef U ustring_t;
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

  inline idx_t get_idx(const ustring_t& ustr)
  {
    assert(m_str2idx.size() == m_idx2str.size());
    string_t str = m_cvt.to_bytes(ustr);
    return get_idx(str);
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

  inline const U& get_ustr(const idx_t idx)
  {
    auto it = m_idx2ustr.find(idx);
    if (m_idx2ustr.end() != it)
    {
      return it->second;
    }

    const S& str = get_str(idx);
    ustring_t ustr = m_cvt.from_bytes(str);
    it = m_idx2ustr.insert({idx, ustr}).first;
    if (m_idx2ustr.end() != it)
    {
      return it->second;
    }

    throw std::runtime_error("StringIndex: can't find recently inserted element.");
  }

  inline const U& get_ustr(const idx_t idx) const
  {
    auto it = m_idx2ustr.find(idx);
    if (m_idx2ustr.end() != it)
    {
      return it->second;
    }

    throw std::runtime_error("StringIndex: can't find an element by index.");
  }

  size_t size() const
  {
    return m_idx2str.size();
  }

protected:
  std::unordered_map<string_t, idx_t> m_str2idx;
  std::unordered_map<idx_t, string_t> m_idx2str;
  mutable std::unordered_map<idx_t, ustring_t> m_idx2ustr;

  std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> m_cvt;
};

typedef TStringIndex<std::string, std::u32string, uint32_t> StringIndex;

}

#endif
