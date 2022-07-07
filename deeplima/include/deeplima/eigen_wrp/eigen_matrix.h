// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_SRC_INFERENCE_EIGEN_EIGEN_MATRIX_H
#define DEEPLIMA_SRC_INFERENCE_EIGEN_EIGEN_MATRIX_H

#include <eigen3/Eigen/Dense>

namespace deeplima
{
namespace eigen_wrp
{

template <class M>
class EigenMatrix
{
public:
  typedef M matrix_t;
  typedef typename M::Scalar value_t;

  EigenMatrix()
    : m_start_time(0),
      m_max_time(0)
  {}

  EigenMatrix(int64_t max_time, int64_t max_feat)
    : m_start_time(0),
      m_max_time(max_time)
  {
    assert(m_start_time >= 0);
    assert(m_max_time > m_start_time);
    assert(max_feat > 0);
    assert(max_feat < std::numeric_limits<int64_t>::max());

    m_tensor.resize(max_feat, m_max_time - m_start_time);
  }

  void init(int64_t max_time, int64_t max_feat)
  {
    assert(m_start_time >= 0);
    assert(max_time > m_start_time);
    assert(max_feat > 0);
    assert(max_feat < std::numeric_limits<int64_t>::max());

    m_max_time = max_time;
    m_tensor = M::Zero(max_feat, m_max_time - m_start_time);
  }

  inline void set(uint64_t time, uint64_t feat, value_t value)
  {
    assert(feat < std::numeric_limits<int64_t>::max());
    assert(time < std::numeric_limits<int64_t>::max() - m_start_time);
    assert(time < m_tensor.cols());

    m_tensor(feat, m_start_time + time) = value;
  }

  inline void set(uint64_t time, EigenMatrix<M>& src, uint64_t src_time)
  {
    m_tensor.col(time) = src.m_tensor.col(src_time);
  }

  inline value_t get(uint64_t time, uint64_t feat)
  {
    assert(feat < std::numeric_limits<int64_t>::max());
    assert(time < std::numeric_limits<int64_t>::max() - m_start_time);

    return m_tensor(feat, m_start_time + time);
  }

  inline uint64_t size() const
  {
    return m_max_time - m_start_time;
  }

  inline uint64_t get_max_feat() const
  {
    return m_tensor.rows();
  }

  inline const M& get_tensor() const
  {
    return m_tensor;
  }

protected:

  M& get_tensor()
  {
    return m_tensor;
  }
  int64_t m_start_time;
  int64_t m_max_time;

  M m_tensor;
};

typedef EigenMatrix<Eigen::MatrixXf> EigenMatrixXf;
typedef EigenMatrix<Eigen::MatrixXi> EigenMatrixXi;

} // namespace eigen_wrp
} // namespace deeplima

#endif
