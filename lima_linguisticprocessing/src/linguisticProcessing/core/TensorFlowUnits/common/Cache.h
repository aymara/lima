// Copyright 2002-2020 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
