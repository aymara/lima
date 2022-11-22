// Copyright 2002-2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_INCLUDE_SPLIT_STR_H
#define DEEPLIMA_INCLUDE_SPLIT_STR_H

#include <string>
#include <vector>
#include <cstdlib>
#include <stdexcept>
#include <limits>

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

inline std::vector<size_t> split_list_of_nums(const std::string &str, char delim)
{
  std::vector<std::string> temp = split(str, delim);
  std::vector<size_t> res;
  res.reserve(temp.size());

  for (const std::string& s : temp)
  {
    char *p_end = nullptr;
    long int t = strtol(s.c_str(), &p_end, 10);
    if (p_end != s.data() + s.size())
    {
      throw std::range_error("Can't parse \"" + str + "\"");
    }
    if (t <= 0)
    {
      throw std::range_error("Can't parse \"" + str + "\": value " + std::to_string(t) + " must be > 0");
    }
    if (size_t(t) >= std::numeric_limits<size_t>::max())
    {
      throw std::range_error("Can't parse \"" + str + "\": value " + std::to_string(t) + " is too high");
    }
    res.push_back(size_t(t));
  }

  return res;
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
