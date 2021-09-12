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
