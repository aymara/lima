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

#ifndef DEEPLIMA_SRC_INCLUDE_PRETTY_H
#define DEEPLIMA_SRC_INCLUDE_PRETTY_H

#include <string>
#include <sstream>

namespace deeplima
{

inline std::string pretty_bits_to_string_one_byte(const uint8_t arg)
{
  std::stringstream s;
  for (size_t i = 7; i > 0; i--)
  {
    s << ((arg & (0x01 << i)) >> i);
  }
  s << (arg & 0x01);
  return s.str();
}

template<class T>
inline std::string pretty_bits_to_string(const T arg)
{
  std::stringstream s;
  for (size_t i = sizeof(T) - 1; i > 0; i--)
  {
    if (i < sizeof(T) - 1)
    {
      s << " ";
    }
    s << pretty_bits_to_string_one_byte(uint8_t(arg >> (i * 8)));
  }
  s << " " << pretty_bits_to_string_one_byte(uint8_t(arg));
  return s.str();
}

} // namespace deeplima

#endif
