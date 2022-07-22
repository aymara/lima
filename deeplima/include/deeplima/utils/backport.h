// Copyright 2002-2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_INCLUDE_BACKPORT_H
#define DEEPLIMA_INCLUDE_BACKPORT_H

#include <unordered_map>

namespace deeplima
{
namespace utils
{
namespace backport
{

// backport from C++ 20
template<class K, class V, class H, class A, class P>
typename std::unordered_map<K, V, H, A>::size_type erase_if(std::unordered_map<K, V, H, A>& c, P pred)
{
  auto old_size = c.size();
  for (auto i = c.begin(), last = c.end(); i != last;) {
    if (pred(*i)) {
      i = c.erase(i);
    } else {
      ++i;
    }
  }
  return old_size - c.size();
}

} // namespace backport
} // namespace utils
} // namespace deeplima

#endif
