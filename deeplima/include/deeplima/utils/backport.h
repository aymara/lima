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
