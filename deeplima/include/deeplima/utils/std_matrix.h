// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_APPS_STD_MATRIX_H
#define DEEPLIMA_APPS_STD_MATRIX_H

#include <memory>

template <class T>
class StdVector
{
public:
  StdVector(const std::vector<T>& input)
    : m_vector(input) { }

  inline T get(size_t idx) const
  {
    assert(idx < m_vector.size());
    return m_vector[idx];
  }

protected:
  const std::vector<T>& m_vector;
};

template <class T>
class StdMatrix
{
public:

  StdMatrix() : m_tensor()
  {
  }

  StdMatrix(std::shared_ptr< std::vector< std::vector<T> > > input) : m_tensor(input)
  {
  }

  StdMatrix(const StdMatrix& m) : m_tensor(m.m_tensor) {
  }

  StdMatrix& operator=(const StdMatrix& m){
    m_tensor = m.m_tensor;
  }

  ~StdMatrix(){
      std::cerr<< "Matrix Destroyed!\n";
  }

  inline uint64_t get(uint64_t time, uint64_t feat) const
  {
    assert(feat < std::numeric_limits<int64_t>::max());
    assert(time < std::numeric_limits<int64_t>::max());

    return m_tensor[feat][time];
  }

  void copy(const StdMatrix<T>& m){
        m_tensor = m.m_tensor;
    }

  inline uint64_t size() const
  {
    return m_tensor[0].size();
  }

  std::vector<std::vector<T>> m_tensor;
protected:

};

#endif
