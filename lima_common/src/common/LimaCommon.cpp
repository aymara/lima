/*
    Copyright 2002-2020 CEA LIST

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
