// Copyright 2002-2022 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_INCLUDE_SECURE_CAST_H
#define DEEPLIMA_INCLUDE_SECURE_CAST_H

#include <limits>
#include <stdexcept>

namespace deeplima
{
namespace utils
{

template <typename To, typename From>
inline To cast_to_signed(const From arg)
{
  static_assert(std::numeric_limits<To>::min() < 0,
                "cast_to_signed used in incorrect context (To, min)");

  static_assert(std::numeric_limits<From>::min() == 0,
                "cast_to_signed used in incorrect context (From, min)");

  static_assert(std::numeric_limits<To>::max() < std::numeric_limits<From>::max(),
                "cast_to_signed used in incorrect context (max)");

  if (arg <= static_cast<From>(std::numeric_limits<To>::max()))
  {
    return static_cast<To>(arg);
  }

  throw std::overflow_error("cast_to_signed: argument is too big");
}

} // namespace utils
} // namespace deeplima

#endif
