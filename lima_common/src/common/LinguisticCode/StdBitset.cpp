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

namespace Lima
{
std::hash<std::bitset<LC_WIDTH>> LinguisticCode::hash_fn;
const char LinguisticCode::alphabet[] = "0123456789ABCDEF";
static const std::string odds = "13579";
const LinguisticCode LinguisticCode::ZERO;

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
    if (pos >= LC_WIDTH)
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
