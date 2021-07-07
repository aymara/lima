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

#ifndef DEEPLIMA_SRC_STATIC_GRAPH_DICT_HOLDER_ADAPTER_H
#define DEEPLIMA_SRC_STATIC_GRAPH_DICT_HOLDER_ADAPTER_H

#include "dict.h"
#include "torch_wrp/torch_matrix.h"

namespace deeplima
{

template <class DictType, class Matrix>
class DictHolderAdapter : public DictAdapter<DictType, Matrix>
{
public:
  void set_dicts(DictsHolder& dicts)
  {
    m_original_dicts = dicts;
    this->m_dicts.clear();
    this->m_dicts.resize(m_original_dicts.size());
    for (size_t i = 0; i < m_original_dicts.size(); i++)
    {
      DictType* p = dynamic_cast<DictType*>(m_original_dicts[i].get());
      this->m_dicts[i] = p;
    }
  }

protected:
  DictsHolder m_original_dicts;
};

} // namespace deeplima

#endif
