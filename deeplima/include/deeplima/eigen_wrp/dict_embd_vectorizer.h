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

#ifndef DEEPLIMA_SRC_INFERENCE_EIGEN_DICT_EMBD_VECTORIZER_H
#define DEEPLIMA_SRC_INFERENCE_EIGEN_DICT_EMBD_VECTORIZER_H

#include <vector>
#include <cassert>

namespace deeplima
{

template <class DH, class D, class Matrix>
class DictEmbdVectorizer : public Matrix
{
public:

  DictEmbdVectorizer() {}

  // must be called before init
  void set_dicts(const DH& dicts)
  {
    m_dicts = dicts;
  }

  void init(const DH& dicts, int64_t max_time, int64_t max_feat)
  {
    assert(dicts.size() == max_feat);
    m_dicts = dicts;
    m_pos.reserve(max_feat);
    Eigen::Index dim = 0;
    for ( const auto& d : m_dicts )
    {
      m_pos.push_back(dim);
      dim += d.dim();
    }
    Matrix::init(max_time, dim);
  }

  void init(int64_t max_time, int64_t max_feat)
  {
    assert(m_dicts.size() > 0);
    assert(m_dicts.size() == max_feat);
    m_pos.reserve(max_feat);
    Eigen::Index dim = 0;
    for ( const auto& d : m_dicts )
    {
      m_pos.push_back(dim);
      dim += d.dim();
    }
    Matrix::init(max_time, dim);
  }

  inline void set(uint64_t time, uint64_t feat, typename D::value_t value)
  {
    assert(m_dicts.size() == m_pos.size());
    assert(feat < m_dicts.size());
    m_dicts[feat].get(value, Matrix::get_tensor(), time, m_pos[feat]);
  }

protected:
  DH m_dicts;
  std::vector<Eigen::Index> m_pos;
};

} // namespace deeplima

#endif
