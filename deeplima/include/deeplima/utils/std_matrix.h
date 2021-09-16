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

  inline uint64_t get(uint64_t time, uint64_t feat) const
  {
    assert(feat < std::numeric_limits<int64_t>::max());
    assert(time < std::numeric_limits<int64_t>::max());

    return m_tensor[feat][time];
  }

  inline uint64_t size() const
  {
    return m_tensor[0].size();
  }

protected:

  const std::vector<std::vector<T>>& m_tensor;
};

#endif
