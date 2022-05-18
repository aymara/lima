// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_SRC_STATIC_GRAPH_DICT_HOLDER_ADAPTER_H
#define DEEPLIMA_SRC_STATIC_GRAPH_DICT_HOLDER_ADAPTER_H

#include "dict.h"
#include "deeplima/torch_wrp/torch_matrix.h"

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
