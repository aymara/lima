// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
