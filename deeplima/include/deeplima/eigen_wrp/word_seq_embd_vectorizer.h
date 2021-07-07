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

#ifndef DEEPLIMA_EIGEN_WRP_WORD_SEQ_EMBD_VECTORIZER_H
#define DEEPLIMA_EIGEN_WRP_WORD_SEQ_EMBD_VECTORIZER_H

#include <string>
#include <memory>
#include <vector>

#include "fastText_wrp/fastText_wrp.h"

namespace deeplima
{
namespace tagging
{

template <class DataSet, class StrFeatExtractor, class UIntFeatExtractor, class MatrixFloat, class Idx=uint64_t>
class WordSeqEmbdVectorizer
{
public:
  typedef DataSet dataset_t;
  typedef typename DataSet::token_t token_t;

  enum feature_type_t : unsigned char
  {
    int_feature = 0,
    str_feature = 1,
    float_feature = 2,
    max_feature
  };

  struct feature_descr_base_t
  {
    feature_type_t m_type;
    int m_dim; // 0 means "ask feature extractor"
    std::string m_name;

    feature_descr_base_t(feature_type_t type, const std::string& name, int dim = 0)
      : m_type(type), m_dim(dim), m_name(name) {}
  };

  struct feature_descr_t : public feature_descr_base_t
  {
    std::shared_ptr<FeatureVectorizerBase<Idx>> m_vectorizer; // feature extractor

    feature_descr_t(feature_type_t type, const std::string& name, std::shared_ptr<FeatureVectorizerBase<Idx>> vectorizer)
      : feature_descr_base_t(type, name, 0), m_vectorizer(vectorizer) {}
  };

protected:
  std::vector<feature_descr_t> m_features;

  int m_features_size;

  std::vector<uint32_t> m_features_pos;

public:

  WordSeqEmbdVectorizer()
    : m_features_size(0) { }

  WordSeqEmbdVectorizer(const std::vector<feature_descr_t>& features)
    : m_features_size(0)
  {
    init_features(features);
  }

  void init_features(const std::vector<feature_descr_t>& features)
  {
    m_features = features;
    m_features_size = 0;
    m_features_pos.reserve(m_features.size());

    for (const auto feat_descr : m_features)
    {
      m_features_pos.push_back(m_features_size);

      if (0 == feat_descr.m_dim)
      {
        m_features_size += feat_descr.m_vectorizer->dim();
      }
      else
      {
        m_features_size += feat_descr.m_dim;
      }
    }
  }

  int dim() const
  {
    return m_features_size;
  }

  inline void vectorize_timepoint(MatrixFloat& target, uint64_t timepoint, const typename DataSet::token_t& token)
  {
    for (size_t i = 0; i < m_features.size(); i++)
    {
      const feature_descr_t& feat_descr = m_features[i];
      switch (feat_descr.m_type)
      {
      case int_feature:
      {
        size_t ifeat = UIntFeatExtractor::get_feat_id(feat_descr.m_name);
        const float feat_val = UIntFeatExtractor::feat_value(token, ifeat);
        std::shared_ptr<FeatureVectorizerToMatrix<MatrixFloat, uint64_t, Idx>> fv
            = std::dynamic_pointer_cast<FeatureVectorizerToMatrix<MatrixFloat, uint64_t, Idx>,
                                        FeatureVectorizerBase<Idx>>(feat_descr.m_vectorizer);
        assert(nullptr != fv.get());
        fv->get(feat_val, target, timepoint, m_features_pos[i]);
      }
        break;
      case float_feature:
        throw std::runtime_error("Unsupported");
        break;
      case str_feature:
      {
        size_t ifeat = StrFeatExtractor::get_feat_id(feat_descr.m_name);
        const std::string& feat_val = StrFeatExtractor::feat_value(token, ifeat);
        std::shared_ptr<FeatureVectorizerToMatrix<MatrixFloat, const std::string&, Idx>> fv
            = std::dynamic_pointer_cast<FeatureVectorizerToMatrix<MatrixFloat, const std::string&, Idx>,
                                        FeatureVectorizerBase<Idx>>(feat_descr.m_vectorizer);
        assert(nullptr != fv.get());
        fv->get(feat_val, target, timepoint, m_features_pos[i]);
      }
        break;
      default:
        throw std::runtime_error("Unknown argument type");
      }
    }
  }
};

} // namespace tagging
} // namespace deeplima

#endif
