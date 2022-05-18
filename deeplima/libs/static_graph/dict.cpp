// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "dict.h"

using namespace std;

namespace deeplima
{

#define DEFINE_DICT_CLASS_ID(x) \
  template <> const std::string x::m_class_id = #x;

DEFINE_DICT_CLASS_ID(Dict<wstring>)
DEFINE_DICT_CLASS_ID(Dict<string>)
DEFINE_DICT_CLASS_ID(Dict<wchar_t>)
DEFINE_DICT_CLASS_ID(Dict<uint64_t>)

template <class T>
c10::IValue Dict<T>::toIValue() const
{
  std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

  c10::List<std::string> l;
  l.reserve(i2v.size());
  for ( const T& v : i2v )
  {
    std::string s = converter.to_bytes(to_wstring(v));
    l.push_back(s);
  }
  return l;
}

template <class T>
void Dict<T>::fromIValue(const c10::IValue& v)
{
  if (!v.isList())
  {
    throw std::runtime_error("Dict expects c10::List object.");
  }

  std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

  const c10::List<c10::IValue>& l = v.toList();
  for (size_t i = 0; i < l.size(); i++)
  {
    if (!l.get(i).isString())
    {
      throw std::runtime_error("Dict expects list of strings.");
    }
    const std::string& str = l.get(i).toStringRef();
    std::wstring wstr = converter.from_bytes(str);
    T k;
    from_wstring(wstr, k);
    add(k);
  }
}

// Dict specialisation for T=wstring

template <>
void Dict<wstring>::fromIValue(const c10::IValue& v)
{
  if (!v.isList())
  {
    throw std::runtime_error("Dict expects c10::List object.");
  }

  std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

  const c10::List<c10::IValue>& l = v.toList();
  for (size_t i = 0; i < l.size(); i++)
  {
    if (!l.get(i).isString())
    {
      throw std::runtime_error("Dict expects list of strings.");
    }
    const std::string& str = l.get(i).toStringRef();
    std::wstring wstr = converter.from_bytes(str);
    wstring k;
    from_wstring(wstr, k);
    add(k);
  }
}

template <>
c10::IValue Dict<wstring>::toIValue() const
{
  std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

  c10::List<std::string> l;
  l.reserve(i2v.size());
  for ( const wstring& v : i2v )
  {
    std::string s = converter.to_bytes(to_wstring(v));
    l.push_back(s);
  }
  return l;
}

// Dict specialisation for T=string

template <>
void Dict<string>::fromIValue(const c10::IValue& v)
{
  if (!v.isList())
  {
    throw std::runtime_error("Dict expects c10::List object.");
  }

  const c10::List<c10::IValue>& l = v.toList();
  for (size_t i = 0; i < l.size(); i++)
  {
    if (!l.get(i).isString())
    {
      throw std::runtime_error("Dict expects list of strings.");
    }
    const std::string& k = l.get(i).toStringRef();
    add(k);
  }
}

template <>
c10::IValue Dict<string>::toIValue() const
{
  c10::List<std::string> l;
  l.reserve(i2v.size());
  for ( const string& v : i2v )
  {
    l.push_back(v);
  }
  return l;
}

// Dict specialisation for T=wchar_t

template <>
c10::IValue Dict<wchar_t>::toIValue() const
{
  std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

  c10::List<std::string> l;
  l.reserve(i2v.size());
  for ( const wchar_t v : i2v )
  {
    std::string s = converter.to_bytes(to_wstring(v));
    l.push_back(s);
  }
  return l;
}

template <>
c10::IValue Dict<uint64_t>::toIValue() const
{
  c10::List<int64_t> l;

  l.reserve(i2v.size());
  for ( const uint64_t v : i2v )
  {
    l.push_back(int64_t(v));
  }
  return l;
}

// Dict specialisation for T=uint64_t

template <>
void Dict<uint64_t>::fromIValue(const c10::IValue& v)
{
  if (!v.isList())
  {
    throw std::runtime_error("Dict expects c10::List object.");
  }

  const c10::List<int64_t>& l = v.toIntList();
  for (size_t i = 0; i < l.size(); i++)
  {
    add(l.get(i));
  }
}

} // namespace deeplima

