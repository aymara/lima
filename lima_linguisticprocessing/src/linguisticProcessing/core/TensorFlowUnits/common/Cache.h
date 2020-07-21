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

#ifndef LIMA_TENSORFLOWUNITS_CACHE_H
#define LIMA_TENSORFLOWUNITS_CACHE_H

#include <list>
#include <unordered_map>
#include <vector>

namespace Lima
{
namespace LinguisticProcessing
{
namespace TensorFlowUnits
{
namespace Common
{

template <class K, class V>
class Cache
{
public:
  virtual bool has(const K& k) const = 0;
  virtual bool get(const K& k, V& v) = 0;
  virtual void put(const K& k, const V& v) = 0;
  virtual size_t size() const = 0;
  virtual void get_keys(std::vector<K>& vec) const = 0;
};

template <class K, class V>
class LRUCache : public Cache<K, V>
{
  std::list<std::pair<K, V>> items;
  std::unordered_map<K, decltype(items.begin())> index;
  size_t max_size;

  virtual void clean()
  {
    if (0 == max_size)
      return;

    while (index.size() > max_size)
    {
      auto it = items.end();
      it--;
      index.erase(it->first);
      items.pop_back();
    }
  }

public:

  LRUCache(size_t max_size_ = 0) : max_size(max_size_) { }

  void set_max_size(size_t max_size_)
  {
    max_size = max_size_;
  }

  virtual bool has(const K& k) const
  {
    const auto it = index.find(k);
    return index.end() != it;
  }

  virtual bool get(const K& k, V& v)
  {
    auto it = index.find(k);
    if (index.end() == it)
    {
      return false;
    }
    items.splice(items.begin(), items, it->second);
    v = it->second->second;
    return true;
  }

  virtual void put(const K& k, const V& v)
  {
    auto it = index.find(k);
    if (index.end() != it)
    {
      items.erase(it->second);
      index.erase(it);
    }
    items.push_front(std::make_pair(k, v));
    index.insert(std::make_pair(k, items.begin()));
    clean();
  }

  virtual size_t size() const
  {
    return index.size();
  }

  virtual void get_keys(std::vector<K>& vec) const
  {
    for ( const auto& item : items )
      vec.push_back(item.first);
  }
};

}
}
}
}

#endif
