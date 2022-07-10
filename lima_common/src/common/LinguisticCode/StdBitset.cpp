// Copyright 2002-2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "common/LimaCommon.h"

namespace Lima
{
std::hash<std::bitset<LC_WIDTH>> LinguisticCode::hash_fn;
LIMA_COMMON_EXPORT const char LinguisticCode::alphabet[] = "0123456789ABCDEF";
static const std::string odds = "13579";
LIMA_COMMON_EXPORT const LinguisticCode LinguisticCode::ZERO;

LIMA_COMMON_EXPORT std::string LinguisticCode::toDecString() const
{
  std::string s;
  LinguisticCode x = *this;
  while (x.base.any())
  {
    unsigned char r = 0;
    LinguisticCode t;
    size_t i = LC_WIDTH - 1;
    while (i > 0 && !x.base.test(i))
    {
      i--;
    }
    while (true)
    {
      r <<= 1;
      r |= x.base.test(i) ? 1 : 0;
      if (r >= 10)
      {
        t.base[i] = true;
        r -= 10;
      }
      if (0 == i)
      {
        break;
      }
      i--;
    }
    s += alphabet[r];
    x = t;
  }
  if (s.size() == 0)
  {
    return std::string("0");
  }
  reverse(s.begin(), s.end());
  return s;
}

LIMA_COMMON_EXPORT LinguisticCode LinguisticCode::fromDecString(const std::string& str)
{
  std::string num = str;
  LinguisticCode x;
  size_t pos = 0;
  while (num.size() > 0 && num != std::string("0"))
  {
    char last = num[num.size() - 1];
    if (std::string::npos != odds.find(last))
    {
      x.base[pos] = true;
    }

    std::string new_num;
    char add = 0;
    for (size_t i = 0; i < num.size(); i++)
    {
      char ch = (num[i] - '0') / 2 + add;
      new_num += (ch + '0');
      if (std::string::npos != odds.find(num[i]))
      {
        add = 5;
      }
      else
      {
        add = 0;
      }
    }

    size_t first_non_zero = 0;
    while (first_non_zero < new_num.size() - 1 && new_num[first_non_zero] == '0')
    {
      first_non_zero++;
    }

    if (first_non_zero > 0 && first_non_zero <= new_num.size() - 1)
    {
      new_num = new_num.substr(first_non_zero);
    }

    num = new_num;

    pos++;
    if (pos >= LC_WIDTH && num.size() > 0 && num != std::string("0"))
    {
      throw std::overflow_error(std::string("LinguisticCode::fromDecString: input string is too long: ") + str);
    }
  }

  return x;
}

LIMA_COMMON_EXPORT QDebug& operator<<(QDebug& os, const LinguisticCode& value)
{
  std::stringstream ss;
  ss << value.toHexString();
  return os << ss.str();
}

LIMA_COMMON_EXPORT QTextStream& operator<<(QTextStream& os, const LinguisticCode& value)
{
  std::stringstream ss;
  ss << value.toHexString();
  return os << ss.str().c_str();
}

}
