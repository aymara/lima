// Copyright 2002-2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_INCLUDE_SPLIT_STR_H
#define DEEPLIMA_INCLUDE_SPLIT_STR_H

#include <string>
#include <vector>

namespace deeplima
{
namespace utils
{

inline std::vector<std::string> split(const std::string &str, char delim)
{
  size_t start;
  size_t end = 0;
  std::vector<std::string> parts;

  while ((start = str.find_first_not_of(delim, end)) != std::string::npos)
  {
    end = str.find(delim, start);
    parts.push_back(str.substr(start, end - start));
  }

  return parts;
}

template< class InputIt, class Pred >
inline std::string join(InputIt begin, InputIt end, Pred f)
{
  std::string s;
  for (auto i = begin; i != end; ++i)
  {
    if (s.size() > 0)
    {
      s += " ";
    }
    s += f(*i);
  }
  return s;
}

} // namespace utils
} // namespace deeplima

#endif
