// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_SRC_STATIC_GRAPH_DICT_BASE_H
#define DEEPLIMA_SRC_STATIC_GRAPH_DICT_BASE_H

#include <ATen/core/List.h>

namespace deeplima
{

class DictBase
{
public:
  virtual ~DictBase() = default;
  virtual uint64_t size() const = 0;
  virtual c10::IValue toIValue() const = 0;
  virtual void fromIValue(const c10::IValue& v) = 0;
  virtual const std::string& get_class_id() const = 0;
};

template <class D=std::shared_ptr<DictBase>>
class DictsHolderImpl : public std::vector<D>
{
public:
  virtual ~DictsHolderImpl<D>() = default;
  std::vector<uint32_t> get_counters() const
  {
    std::vector<uint32_t> v(this->size());
    for (size_t i = 0; i < this->size(); ++i)
    {
      v[i] = (*this)[i]->size();
    }

    return v;
  }

};

typedef DictsHolderImpl<> DictsHolder;

} // namespace deeplima

#endif
