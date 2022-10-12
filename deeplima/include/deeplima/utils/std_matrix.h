// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_APPS_STD_MATRIX_H
#define DEEPLIMA_APPS_STD_MATRIX_H

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

  StdMatrix(const std::vector<std::vector<T>>& input)
    : m_tensor(input)
  {
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
        this->m_tensor = std::vector<std::vector<T>>(m.m_tensor);
    }

  inline uint64_t size() const
  {
    return m_tensor[0].size();
  }

protected:

  const std::vector<std::vector<T>>& m_tensor;
};

#endif
