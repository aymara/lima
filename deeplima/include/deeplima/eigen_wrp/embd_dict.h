// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
  typedef M tensor_t;

  EmbdDict() = default;
  virtual ~EmbdDict() {}

  template <class T>
  void init(std::shared_ptr<Dict<T>> dict, const M& tensor)
  {
    m_embd = tensor.transpose();
    m_dim = m_embd.rows();
    for ( const auto& it : dict->get_v2i() )
    {
      assert(it.second >= 0);
      assert(it.second < std::numeric_limits<I>::max());
      assert(m_embd.cols() >=0 && it.second < (unsigned long)(m_embd.cols()));
      m_index[static_cast<K>(it.first)] = I(it.second);
      m_reverse_index[static_cast<I>(it.second)] = static_cast<K>(it.first);
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

  inline void get_direct(const I idx, M& target, I timepoint, I pos) const
  {
    target.block(pos, timepoint, m_dim, 1) = m_embd.col(idx);
  }

  inline void get_static(const K key, M& target, I timepoint, I pos) const
  {
    I idx = lookup(key);
    target.block(pos, timepoint, m_dim, 1) = m_embd.col(idx);
  }

  const M& get_tensor() const
  {
    return m_embd;
  }

  inline K decode(const I idx) const
  {
    auto it = m_reverse_index.find(idx);
    return it->second;
  }

protected:
  I m_dim;
  M m_embd;
  std::unordered_map<K, I> m_index;
  std::unordered_map<I, K> m_reverse_index;

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
  ~EmbdDict() {}

  void init(std::shared_ptr<Dict<value_t>> dict, const M& tensor)
  {
    m_embd = tensor.transpose();
    m_dim = m_embd.rows();
    for ( const auto& it : dict->get_v2i() )
    {
      assert(it.second >= 0);
      assert(it.second < std::numeric_limits<I>::max());
      assert(m_embd.cols() >= 0 && it.second < Dict<value_t>::key_t(m_embd.cols()));
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

  const M& get_tensor() const
  {
    return m_embd;
  }

  template <class K>
  std::shared_ptr<Dict<K>> get_int_dict() const
  {
    std::vector<value_t> vk;
    vk.reserve(m_index.size());
    for ( const auto& it : m_index )
    {
      vk.push_back(it.first);
    }
    std::sort(vk.begin(), vk.end());

    std::vector<K> vi(m_index.size());
    for (size_t i = 0; i < vk.size(); ++i)
    {
      vi[i] = lookup(vk[i]);
    }

    std::shared_ptr<Dict<K>> rv = std::make_shared<Dict<K>>(vi);
    return rv;
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
