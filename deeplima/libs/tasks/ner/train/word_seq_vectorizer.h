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

#ifndef DEEPLIMA_LIBS_TASKS_NER_TRAIN_WORD_SEQ_VECTORIZER_H
#define DEEPLIMA_LIBS_TASKS_NER_TRAIN_WORD_SEQ_VECTORIZER_H

#include <memory>

#include "static_graph/dict_base.h"
#include "static_graph/dict.h"

#include "nn/birnn_seq_cls/birnn_seq_classifier.h"
#include "eigen_wrp/word_seq_embd_vectorizer.h"

namespace deeplima
{
namespace tagging
{
namespace train
{

template <class DataSet, class StrFeatExtractor, class UIntFeatExtractor, class MatrixInt, class MatrixFloat>
class WordSeqVectorizerImpl : public tagging::WordSeqEmbdVectorizer<DataSet, StrFeatExtractor, UIntFeatExtractor, MatrixFloat>
{
public:
  typedef tagging::WordSeqEmbdVectorizer<DataSet, StrFeatExtractor, UIntFeatExtractor, MatrixFloat> Parent;

  struct embeddable_feature_descr_t : public Parent::feature_descr_base_t
  {
    std::shared_ptr<DictBase> m_dict; // feature extractor

    embeddable_feature_descr_t(typename Parent::feature_type_t type, const std::string& name, int dim, std::shared_ptr<DictBase> dict)
      : Parent::feature_descr_base_t(type, name, dim), m_dict(dict) {}
  };

protected:
  std::vector<embeddable_feature_descr_t> m_embeddable_features;

  int m_embeddable_size;

public:

  WordSeqVectorizerImpl(const std::vector<typename Parent::feature_descr_t>& features,
                        const std::vector<embeddable_feature_descr_t>& embeddable_features)
    : Parent(features),
      m_embeddable_features(embeddable_features),
      m_embeddable_size(0)
  {
    m_embeddable_size = m_embeddable_features.size();
  }

  const std::vector<deeplima::nets::embd_descr_t> get_embd_descr() const
  {
    std::vector<deeplima::nets::embd_descr_t> d;
    for (const embeddable_feature_descr_t& feat_descr : m_embeddable_features )
    {
      assert(feat_descr.m_dim > 0);
      assert(!feat_descr.m_name.empty());
      d.emplace_back(feat_descr.m_name, feat_descr.m_dim);
    }
    return d;
  }

  typedef std::pair<std::shared_ptr<MatrixInt>, std::shared_ptr<MatrixFloat>> vectorization_t;

  vectorization_t process(const DataSet& src)
  {
    int64_t len = 0;

    typename DataSet::const_iterator i = src.begin();
    while (src.end() != i)
    {
      if ((*i).is_word())
      {
        len++;
      }
      i++;
    }

    std::shared_ptr<MatrixFloat> frozen_features(new MatrixFloat(len, Parent::m_features_size));
    std::shared_ptr<MatrixInt> embeddable_features(new MatrixInt(len, m_embeddable_size));
    vectorization_t rv(embeddable_features, frozen_features);

    typename DataSet::const_iterator it = src.begin();
    uint64_t current_timepoint = 0;
    while (src.end() != it)
    {
      while(!(*it).is_word() && src.end() != it)
      {
        it++;
      }
      if (src.end() == it) break;

      vectorize_timepoint(*frozen_features, *embeddable_features, current_timepoint, *it);

      current_timepoint++;
      if (current_timepoint == std::numeric_limits<int64_t>::max())
      {
        throw std::overflow_error("Too much words in the dataset.");
      }

      it++;
    }

    return rv;
  }

  inline void vectorize_timepoint(MatrixFloat& frozen_features, MatrixInt& embeddable_features,
                                  uint64_t timepoint, const typename DataSet::token_t& token)
  {
    Parent::vectorize_timepoint(frozen_features, timepoint, token);

    for (size_t i = 0; i < m_embeddable_features.size(); i++)
    {
      const embeddable_feature_descr_t& feat_descr = m_embeddable_features[i];
      switch (feat_descr.m_type)
      {
      case Parent::int_feature:
        throw std::runtime_error("Unsupported");
        break;
      case Parent::float_feature:
        throw std::runtime_error("Unsupported");
        break;
      case Parent::str_feature:
      {
        size_t ifeat = StrFeatExtractor::get_feat_id(feat_descr.m_name);
        const std::string& feat_val = StrFeatExtractor::feat_value(/*src.data(),*/ token, ifeat);
        std::shared_ptr<StringDict> dict
            = std::dynamic_pointer_cast<StringDict, DictBase>(feat_descr.m_dict);
        uint64_t idx = dict->get_idx(feat_val);
        embeddable_features.set(timepoint, i, idx);
      }
        break;
      default:
        throw std::runtime_error("Unknown argument type");
      }
    }
  }
};

} // namespace train
} // namespace tagging
} // namespace deeplima

#endif
