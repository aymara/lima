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

#ifndef DEEPLIMA_SRC_INFERENCE_EIGEN_EMBD_DICT_H
#define DEEPLIMA_SRC_INFERENCE_EIGEN_EMBD_DICT_H

#include <unordered_map>
#include <eigen3/Eigen/Dense>

#include "static_graph/dict_base.h"
#include "static_graph/dict.h"

#include "deeplima/fastText_wrp/fastText_wrp.h"

namespace deeplima
{

// K - dict key
// M - Eigen Matrix (Xf or Xd)
template <class K, class M, class I=Eigen::Index>
class EmbdDict : public FeatureVectorizerToMatrix<M, K, I>
{
public:
  typedef K value_t;

  EmbdDict() {}

  void init(std::shared_ptr<Dict<K>> dict, const M& tensor)
  {
    m_embd = tensor.transpose();
    m_dim = m_embd.rows();
    for ( const auto& it : dict->get_v2i() )
    {
      assert(it.second >= 0);
      assert(it.second < std::numeric_limits<I>::max());
      assert(it.second < m_embd.cols());
      m_index[K(it.first)] = I(it.second);
    }
  }

  virtual I dim() const override
  {
    return m_dim;
  }

  // key - the value we are going to convert
  // target - target matrix
  // timepoint - column number
  // pos - feature position (first row number)
  virtual void get(const K key, M& target, I timepoint, I pos) const override
  {
    get_static(key, target, timepoint, pos);
  }

  inline void get_static(const K key, M& target, I timepoint, I pos) const
  {
    I idx = lookup(key);
    target.block(pos, timepoint, m_dim, 1) = m_embd.col(idx);
  }

  M& get_tensor()
  {
    return m_embd;
  }

protected:
  I m_dim;
  M m_embd;
  std::unordered_map<K, I> m_index;

  inline I lookup(const K key) const
  {
    auto i = m_index.find(key);
    return (m_index.end() == i) ? 0 : i->second;
  }
};

template <class M, class I>
class EmbdDict<std::string, M, I> : public FeatureVectorizerToMatrix<M, const std::string&, I>
{
public:
  typedef std::string value_t;

  EmbdDict() {}

  void init(std::shared_ptr<Dict<value_t>> dict, const M& tensor)
  {
    m_embd = tensor.transpose();
    m_dim = m_embd.rows();
    for ( const auto& it : dict->get_v2i() )
    {
      assert(it.second >= 0);
      assert(it.second < std::numeric_limits<I>::max());
      assert(it.second < m_embd.cols());
      m_index[value_t(it.first)] = I(it.second);
    }
  }

  virtual I dim() const override
  {
    return m_dim;
  }

  // key - the value we are going to convert
  // target - target matrix
  // timepoint - column number
  // pos - feature position (first row number)
  virtual void get(const value_t& key, M& target, I timepoint, I pos) const override
  {
    get_static(key, target, timepoint, pos);
  }

  inline void get_static(const value_t& key, M& target, I timepoint, I pos) const
  {
    I idx = lookup(key);
    target.block(pos, timepoint, m_dim, 1) = m_embd.col(idx);
  }

  M& get_tensor()
  {
    return m_embd;
  }

protected:
  I m_dim;
  M m_embd;
  std::unordered_map<value_t, I> m_index;

  inline I lookup(const value_t& key) const
  {
    auto i = m_index.find(key);
    return (m_index.end() == i) ? 0 : i->second;
  }
};

typedef EmbdDict<uint64_t, Eigen::MatrixXf, Eigen::Index> EmbdUInt64Float;
typedef EmbdDict<std::string, Eigen::MatrixXf, Eigen::Index> EmbdStrFloat;
typedef DictsHolderImpl<EmbdUInt64Float> EmbdUInt64FloatHolder;
typedef DictsHolderImpl<EmbdStrFloat> EmbdStrFloatHolder;

} // namespace deeplima

#endif
