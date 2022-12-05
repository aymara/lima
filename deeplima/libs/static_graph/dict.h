// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_SRC_STATIC_GRAPH_DICT_H
#define DEEPLIMA_SRC_STATIC_GRAPH_DICT_H

#include <vector>
#include <unordered_map>
#include <string>
#include <codecvt>

#include "dict_base.h"

namespace deeplima
{

template <class T>
class Dict : public DictBase
{
public:
  typedef T value_t;
  typedef typename std::vector<T>::size_type key_t;

  Dict(const std::vector<T>& v1, const std::vector<T>& v2 = {})
  {
    i2v.reserve(v1.size() + v2.size());

    for (const T& v : v1)
    {
      add(v);
    }

    for (const T& v : v2)
    {
      add(v);
    }
  }

  Dict(const T& UNK, const std::vector<T>& v1, const std::vector<T>& v2 = {})
  {
    i2v.reserve(v1.size() + v2.size() + 1);
    add(UNK);

    for (const T& v : v1)
    {
      add(v);
    }

    for (const T& v : v2)
    {
      add(v);
    }
  }

  template <class InputIt>
  explicit Dict(InputIt begin, InputIt end)
    : Dict(begin, end, [](uint64_t /*c*/){ return true; })
  { }

  template<class InputIt, typename F>
  Dict(const T& UNK, InputIt begin, InputIt end, F f)
  {
    size_t count = 1;
    for (auto it = begin; it != end; ++it)
    {
      if (f(it->second))
      {
        count++;
      }
    }

    i2v.reserve(count);
    add(UNK);
    for (auto it = begin; it != end; ++it)
    {
      if (f(it->second))
      {
        add(it->first);
      }
    }
  }

  template<class InputIt, typename F>
  Dict(const T& UNK, const T& EOS, InputIt begin, InputIt end, F f)
  {
    size_t count = 2;
    for (auto it = begin; it != end; ++it)
    {
      if (f(it->second))
      {
        count++;
      }
    }

    i2v.reserve(count);
    add(UNK);
    add(EOS);
    for (auto it = begin; it != end; ++it)
    {
      if (f(it->second))
      {
        add(it->first);
      }
    }
  }

  template<class InputIt, typename F>
  Dict(const T& UNK, const T& EOS, const T& OTHER, InputIt begin, InputIt end, F f)
  {
    size_t count = 3;
    for (auto it = begin; it != end; ++it)
    {
      if (f(it->second))
      {
        count++;
      }
    }

    i2v.reserve(count);
    add(UNK);
    add(EOS);
    add(OTHER);
    for (auto it = begin; it != end; ++it)
    {
      if (f(it->second))
      {
        add(it->first);
      }
    }
  }

  template<class InputIt, typename F>
  Dict(InputIt begin, InputIt end, F f)
  {
    size_t count = 0;
    for (auto it = begin; it != end; ++it)
    {
      if (f(it->second))
      {
        count++;
      }
    }

    i2v.reserve(count);
    for (auto it = begin; it != end; ++it)
    {
      if (f(it->second))
      {
        add(it->first);
      }
    }
  }

  Dict() {}

  inline const T& get_value(key_t idx) const
  {
    if (idx >= i2v.size())
    {
      throw std::range_error("Index too big in Dict::get_value.");
    }
    return i2v[idx];
  }

  inline key_t get_idx(const T& val) const
  {
    typename std::unordered_map<T, key_t>::const_iterator i = v2i.find(val);
    if (v2i.cend() == i)
    {
      return 0;
    }
    return i->second;
  }

  virtual uint64_t size() const
  {
    return i2v.size();
  }

  inline key_t get_unk() const
  {
    return 0;
  }

  std::string to_string()
  {
    std::stringstream ss;

    for (size_t i = 0; i < i2v.size(); i++)
    {
      ss << i2v[i] << std::endl;
    }

    return ss.str();
  }

  const std::unordered_map<T, key_t>& get_v2i() const
  {
    return v2i;
  }

protected:
  static inline std::wstring to_wstring(const std::wstring& s)
  {
    return s;
  }

  static inline std::u32string to_wstring(char32_t ch)
  {
    char32_t v[2] = { ch, 0 };
    return std::u32string(v);
  }

  static inline void from_wstring(const std::wstring& src, std::wstring& k)
  {
    k = src;
  }

  static inline void from_wstring(const std::wstring& src, wchar_t& k)
  {
    if (src.length() < 1)
    {
      throw std::runtime_error("Dict::from_wstring empty input.");
    }
    k = src[0];
  }

public:

  static inline const std::string& class_id()
  {
    return m_class_id;
  }

  virtual const std::string& get_class_id() const
  {
    return class_id();
  }

  virtual c10::IValue toIValue() const;
  virtual void fromIValue(const c10::IValue& v);

protected:
  inline void add(const T& val)
  {
    i2v.push_back(val);
    if (v2i.cend() != v2i.find(val))
    {
      throw std::logic_error("Elements must be unique");
    }
    v2i[val] = i2v.size() - 1;
  }

  std::unordered_map<T, key_t> v2i;
  std::vector<T> i2v;

  const static std::string m_class_id;
};

typedef Dict<std::wstring> WstringDict;
typedef Dict<std::string> StringDict;
typedef Dict<uint64_t> UInt64Dict;
typedef Dict<char32_t> Char32Dict;

} // namespace deeplima

#endif
