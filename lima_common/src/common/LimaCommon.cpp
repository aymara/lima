// Copyright 2002-2020 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "common/LimaCommon.h"

#ifdef WIN32

#ifdef LIMA_COMMON_EXPORTING
#define LIMA_COMMON_EXPORT    __declspec(dllexport)
#else
#define LIMA_COMMON_EXPORT    __declspec(dllimport)
#endif


#else // Not WIN32

#define LIMA_COMMON_EXPORT

#endif


namespace Lima
{
namespace Common
{

LIMA_COMMON_EXPORT void fakeSymbolForWindowsLinking() {}

}

#ifndef USE_STD_BITSET_FOR_LINGUISTIC_CODE
LIMA_COMMON_EXPORT QDebug& operator<<(QDebug& os, const uint128_t value)
{
  std::stringstream ss;
  ss << value;
  return os << ss.str();
}

LIMA_COMMON_EXPORT QTextStream& operator<<(QTextStream& os, const uint128_t value)
{
  std::stringstream ss;
  ss << value;
  return os << ss.str().c_str();
}

LIMA_COMMON_EXPORT std::ostream& operator<<(std::ostream& os, const uint128_t value)
{
  std::ostream::sentry s(os);
  if (s)
  {
    char buffer[128];
    char* d = std::end(buffer);
    uint128_t t = value;
    do
    {
      -- d;
      *d = "0123456789"[t % 10];
      t /= 10;
    } while (t != 0);

    int len = std::end(buffer) - d;
    if (os.rdbuf()->sputn(d, len) != len)
    {
      os.setstate(std::ios_base::badbit);
    }
  }
  return os;
}

LIMA_COMMON_EXPORT std::istream& operator>>(std::istream& is, uint128_t& value)
{
  value = 0;
  char c = 0;

  while (is.get(c))
  {
    if (!std::isspace(c))
    {
      is.unget();
      break;
    }
  }
  while (is.get(c))
  {
    if (!std::isdigit(c))
    {
      is.unget();
      break;
    }
    value *= 10;
    value += c - '0';
  }

  return is;
}
#endif

}
