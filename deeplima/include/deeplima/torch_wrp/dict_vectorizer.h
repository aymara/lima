// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_SRC_INFERENCE_TORCH_DICT_VECTORIZER_H
#define DEEPLIMA_SRC_INFERENCE_TORCH_DICT_VECTORIZER_H

#include <vector>
#include <cassert>

namespace deeplima
{

template <class DH, class D, class Matrix>
class DictVectorizer : public Matrix
{
public:

  DictVectorizer() {}

  DictVectorizer(int64_t max_time, int64_t max_feat)
    : Matrix(max_time, max_feat)
  {
    std::cerr << "DictVectorizer::DictVectorizer()" << std::endl;
  }

  void init(const DH& dicts, int64_t max_time, int64_t max_feat)
  {
    set_dicts(dicts);
    Matrix::init(max_time, max_feat);
  }

  void set_dicts(const DH& dicts)
  {
    assert(Matrix::get_max_feat() == dicts.size());
    m_dicts.reserve(dicts.size());
    for (const auto& ptr : dicts)
    {
      m_dicts.emplace_back(dynamic_cast<const D*>(ptr.get()));
    }
  }

  inline void set(uint64_t time, uint64_t feat, typename D::value_t value)
  {
    assert(feat < m_dicts.size());
    key_t k = m_dicts[feat]->get_idx(value);
    Matrix::set(time, feat, k);
  }

protected:
  std::vector<const D*> m_dicts;
};

} // namespace deeplima

#endif
