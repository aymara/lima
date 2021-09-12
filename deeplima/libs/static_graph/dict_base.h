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

#ifndef DEEPLIMA_SRC_STATIC_GRAPH_DICT_BASE_H
#define DEEPLIMA_SRC_STATIC_GRAPH_DICT_BASE_H

#include <ATen/core/List.h>

namespace deeplima
{

class DictBase
{
public:
  virtual uint64_t size() const = 0;
  virtual c10::IValue toIValue() const = 0;
  virtual void fromIValue(const c10::IValue& v) = 0;
  virtual const std::string& get_class_id() const = 0;
};

template <class D=std::shared_ptr<DictBase>>
class DictsHolderImpl : public std::vector<D>
{
public:
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
