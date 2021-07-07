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

#ifndef DEEPLIMA_FASTTEXT_WRP_H
#define DEEPLIMA_FASTTEXT_WRP_H

#include "fastText/src/fasttext.h"
#include <eigen3/Eigen/Dense>

namespace deeplima
{

template <class Idx=uint64_t>
class FeatureVectorizerBase
{
public:
  virtual Idx dim() const = 0;
};

template <class Matrix, class Type, class Idx=uint64_t>
class FeatureVectorizerToMatrix : public FeatureVectorizerBase<Idx>
{
public:
  virtual void get(const Type value, Matrix& target, Idx time, Idx pos) const = 0;

};

template <class Matrix, class Idx=uint64_t>
class DirectDict : public deeplima::FeatureVectorizerToMatrix<Matrix, uint64_t, Idx>
{
public:
  DirectDict(size_t n_values) {}

  virtual Idx dim() const override
  {
    return 1;
  }

  virtual void get(const uint64_t value, Matrix& target, Idx time, Idx feat) const override
  {
    target.set(time, feat, value);
  }
};

template <>
class DirectDict<Eigen::MatrixXf, Eigen::Index> : public deeplima::FeatureVectorizerToMatrix<Eigen::MatrixXf, uint64_t, Eigen::Index>
{
public:
  DirectDict(size_t n_values) {}

  virtual Eigen::Index dim() const override
  {
    return 1;
  }

  virtual void get(const uint64_t value, Eigen::MatrixXf& target, Eigen::Index time, Eigen::Index feat) const override
  {
    target.block<1,1>(feat, time) << value;
    //target.set(time, feat, value);
  }
};

template <class Matrix, class Idx=uint64_t>
class FastTextVectorizer : public FeatureVectorizerToMatrix<Matrix, const std::string&, Idx>
{
protected:
  fasttext::FastText m_fasttext;
public:

  FastTextVectorizer(const std::string& fn)
  {
    load(fn);
  }

  virtual void load(const std::string& fn)
  {
    m_fasttext.loadModel(fn);
  }

  virtual Idx dim() const override
  {
    return m_fasttext.getDimension();
  }

  virtual void get(const std::string& value, Matrix& target, Idx time, Idx pos) const override
  {
    fasttext::Vector vec(m_fasttext.getDimension());
    m_fasttext.getWordVector(vec, value);
    for (size_t i = 0; i < vec.size(); i++)
      target.set(time, pos + i, vec[i]);
  }
};

template <>
class FastTextVectorizer<Eigen::MatrixXf, Eigen::Index>
    : public FeatureVectorizerToMatrix<Eigen::MatrixXf, const std::string&, Eigen::Index>
{
protected:
  fasttext::FastText m_fasttext;
public:

  FastTextVectorizer(const std::string& fn)
  {
    load(fn);
  }

  virtual void load(const std::string& fn)
  {
    m_fasttext.loadModel(fn);
  }

  virtual Eigen::Index dim() const override
  {
    return m_fasttext.getDimension();
  }

  virtual void get(const std::string& value, Eigen::MatrixXf& target, Eigen::Index time, Eigen::Index pos) const override
  {
    int dim = m_fasttext.getDimension();
    fasttext::Vector vec(dim);
    m_fasttext.getWordVector(vec, value);
    auto blk = target.block(pos, time, dim, 1);
    for (size_t i = 0; i < vec.size(); i++)
    {
      blk(i, 0) = vec[i];
    }
  }
};

}

#endif
