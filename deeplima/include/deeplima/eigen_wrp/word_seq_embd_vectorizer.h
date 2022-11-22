// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_EIGEN_WRP_WORD_SEQ_EMBD_VECTORIZER_H
#define DEEPLIMA_EIGEN_WRP_WORD_SEQ_EMBD_VECTORIZER_H

#include <string>
#include <memory>
#include <vector>
#include <boost/functional/hash.hpp>

#include "deeplima/fastText_wrp/fastText_wrp.h"
#include "deeplima/eigen_wrp/eigen_matrix.h"

namespace deeplima
{
namespace vectorizers
{

template <class DataSet, class StrFeatExtractor, class UIntFeatExtractor, class MatrixFloat, class Idx=uint64_t>
class WordSeqEmbdVectorizer
{
public:
    void set_model(void* ) {}
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
    size_t m_feat_id = 0;

    feature_descr_base_t(feature_type_t type, const std::string& name, int dim = 0)
      : m_type(type), m_dim(dim), m_name(name) {}
  };

  struct feature_descr_t : public feature_descr_base_t
  {
    FeatureVectorizerBase<Idx>* m_pvectorizer; // feature extractor

    feature_descr_t(feature_type_t type, const std::string& name, FeatureVectorizerBase<Idx>* pvectorizer)
      : feature_descr_base_t(type, name, 0), m_pvectorizer(pvectorizer) {}
  };

protected:
  std::vector<feature_descr_t> m_features;

  int m_features_size;

  std::vector<uint32_t> m_features_pos;

  typedef FeatureVectorizerToMatrix<MatrixFloat, uint64_t, Idx> uint_vectorizer_t;
  typedef FeatureVectorizerToMatrix<MatrixFloat, const std::string&, Idx> str_vectorizer_t;

  const StrFeatExtractor m_str_feat_extractor;
  UIntFeatExtractor m_uint_feat_extractor;

  std::vector<std::pair<uint_vectorizer_t*, size_t>> m_uint_vectorizers; // pointer to vectorizer, feat_idx
  std::vector<std::pair<str_vectorizer_t*, size_t>> m_str_vectorizers;

public:

  WordSeqEmbdVectorizer()
    : m_features_size(0) { }

  WordSeqEmbdVectorizer(const std::vector<feature_descr_t>& features)
    : m_features_size(0)
  {
    init_features(features);
  }

  WordSeqEmbdVectorizer(const std::vector<feature_descr_t>& features,
                        const StrFeatExtractor& str_feat_extractor)
    : m_features_size(0),
      m_str_feat_extractor(str_feat_extractor)
  {
    init_features(features);
  }

  bool is_precomputing() const
  {
    return false;
  }

  inline void precompute(const DataSet& dataset)
  {
  }

  UIntFeatExtractor& get_uint_feat_extractor()
  {
    return m_uint_feat_extractor;
  }

  void init_features(const std::vector<feature_descr_t>& features)
  {
    m_features = features;
    m_features_size = 0;
    m_features_pos.reserve(m_features.size());

    for (auto& feat_descr : m_features)
    {
      if (str_feature == feat_descr.m_type)
      {
        str_vectorizer_t *pfv = dynamic_cast<str_vectorizer_t*>(feat_descr.m_pvectorizer);
        assert(nullptr != pfv);
        feat_descr.m_feat_id = m_str_feat_extractor.get_feat_id(feat_descr.m_name);
        m_str_vectorizers.emplace_back(std::make_pair(pfv, feat_descr.m_feat_id));

        m_features_pos.push_back(m_features_size);

        if (0 == feat_descr.m_dim)
        {
          m_features_size += feat_descr.m_pvectorizer->dim();
        }
        else
        {
          m_features_size += feat_descr.m_dim;
        }
      }
      else if (int_feature == feat_descr.m_type)
      {
        uint_vectorizer_t *pfv = dynamic_cast<uint_vectorizer_t*>(feat_descr.m_pvectorizer);
        assert(nullptr != pfv);
        feat_descr.m_feat_id = m_uint_feat_extractor.get_feat_id(feat_descr.m_name);
        m_uint_vectorizers.emplace_back(std::make_pair(pfv, feat_descr.m_feat_id));

        m_features_pos.push_back(m_features_size);

        if (0 == feat_descr.m_dim)
        {
          m_features_size += feat_descr.m_pvectorizer->dim();
        }
        else
        {
          m_features_size += feat_descr.m_dim;
        }
      }
    }

    for (const auto feat_descr : m_features)
    {
      if (float_feature == feat_descr.m_type)
      {
        throw std::runtime_error("Unsupported");
      }
    }

    if (m_features.size() != (m_uint_vectorizers.size() + m_str_vectorizers.size()))
    {
      throw std::runtime_error("Unknown vectorizer used");
    }
  }

  int dim() const
  {
    return m_features_size;
  }

  inline void vectorize_timepoint(MatrixFloat& target, uint64_t timepoint, const typename DataSet::token_t& token) const
  {
    for (size_t feat_idx = 0; feat_idx < m_features.size(); ++feat_idx)
    {
      const auto& feat_descr = m_features[feat_idx];

      if (feature_type_t::int_feature == m_features[feat_idx].m_type)
      {
        uint_vectorizer_t *pfv = dynamic_cast<uint_vectorizer_t*>(feat_descr.m_pvectorizer);
        const float feat_val = m_uint_feat_extractor.feat_value(token, feat_descr.m_feat_id);
        pfv->get(feat_val, target, timepoint, m_features_pos[feat_idx]);
      }
      else if (feature_type_t::str_feature == m_features[feat_idx].m_type)
      {
        str_vectorizer_t *pfv = dynamic_cast<str_vectorizer_t*>(feat_descr.m_pvectorizer);
        const std::string& feat_val = m_str_feat_extractor.feat_value(token, feat_descr.m_feat_id);
        pfv->get(feat_val, target, timepoint, m_features_pos[feat_idx]);
      }
    }
  }
};

template <class DataSet,
          class StrFeatExtractor,
          class UIntFeatExtractor,
          class MatrixFloat,
          class Idx=uint64_t,
          size_t NUM_BUCKETS=32,
          size_t BUCKET_SIZE=1024*64>
class WordSeqEmbdVectorizerWithCache
    : public WordSeqEmbdVectorizer<DataSet, StrFeatExtractor, UIntFeatExtractor, MatrixFloat, Idx>
{
protected:
  typedef WordSeqEmbdVectorizer<DataSet, StrFeatExtractor, UIntFeatExtractor, MatrixFloat, Idx> Parent;

  struct timepoint_features_hash;
  class timepoint_features_t
  {
  protected:
    std::vector<uint64_t> m_uint_feats;
    std::string m_str_feats; // concatenated string features with \0 as separator

  public:
    void set_model(void* ) {}

    timepoint_features_t(size_t num_uint_feats)
      : m_uint_feats(num_uint_feats, 0) { }

    inline void set_uint_feat(size_t idx, uint64_t val)
    {
      assert(idx < m_uint_feats.size());
      m_uint_feats[idx] = val;
    }

    inline uint64_t get_uint_feat(size_t idx) const
    {
      assert(idx < m_uint_feats.size());
      return m_uint_feats[idx];
    }

    inline void append_str_feat(const std::string& val)
    {
      if (!m_str_feats.empty())
      {
        m_str_feats.push_back(0);
      }
      m_str_feats.append(val);
    }

    inline void append_str_feat(char val)
    {
      m_str_feats.push_back(val);
    }

    inline const std::string& get_str_feat() const
    {
      return m_str_feats;
    }

    inline bool operator==(const timepoint_features_t& other) const
    {
      assert(m_uint_feats.size() == other.m_uint_feats.size());
      return (m_uint_feats == other.m_uint_feats) && (m_str_feats == other.m_str_feats);
    }

    friend struct timepoint_features_hash;
  };

  struct timepoint_features_hash
  {
    std::size_t operator()(timepoint_features_t const& a) const noexcept
    {
      std::size_t h = 0;
      if (!a.m_uint_feats.empty())
      {
        h = boost::hash_range(a.m_uint_feats.begin(), a.m_uint_feats.end());
      }
      boost::hash_combine(h, a.m_str_feats);
      return h;
    }
  };

  std::unordered_map<timepoint_features_t, Idx, timepoint_features_hash> m_precomputed_index;
  std::vector<eigen_wrp::EigenMatrixXf::matrix_t> m_precomputed_vectors;
  Idx m_bucket_size;
  Idx m_next_free_idx;
  Idx m_curr_bucket_id;
  eigen_wrp::EigenMatrixXf::matrix_t m_temp_vectors;

  Idx get_vector(const timepoint_features_t& key, eigen_wrp::EigenMatrixXf::matrix_t** matrix)
  {
    auto it = m_precomputed_index.find(key);
    if (m_precomputed_index.end() != it)
    {
      Idx i = it->second;
      Idx bucket_id = i / m_bucket_size;
      assert(bucket_id < m_precomputed_vectors.size());
      i = i % m_bucket_size;
      *matrix = &(m_precomputed_vectors[bucket_id]);
      return i;
    }

    if (m_curr_bucket_id < m_precomputed_vectors.size())
    {
      if (m_next_free_idx >= m_bucket_size)
      {
        m_curr_bucket_id++;
        if (m_curr_bucket_id >= m_precomputed_vectors.size())
        {
          m_precomputed_vectors.resize(m_curr_bucket_id + 1);
        }
        m_next_free_idx = 0;
      }

      if (0 == m_precomputed_vectors[m_curr_bucket_id].cols())
      {
        init_curr_bucket(Parent::dim());
      }

      compute_vector(key, m_precomputed_vectors[m_curr_bucket_id], m_next_free_idx);
      m_precomputed_index[key] = m_curr_bucket_id * m_bucket_size + m_next_free_idx;
      *matrix = &(m_precomputed_vectors[m_curr_bucket_id]);
      Idx key_idx = m_next_free_idx;
      m_next_free_idx++;

      return key_idx;
    }

    compute_vector(key, m_temp_vectors, 0);
    *matrix = &m_temp_vectors;
    return 0;
  }

  void init_curr_bucket(size_t dim)
  {
    assert(dim > 0);
    m_precomputed_vectors[m_curr_bucket_id]
        = eigen_wrp::EigenMatrixXf::matrix_t::Zero(dim, m_bucket_size);
  }

  void compute_vector(const timepoint_features_t& key, eigen_wrp::EigenMatrixXf::matrix_t& matrix, Idx idx)
  {
    size_t feat_idx = 0;
    for (size_t i = 0; i < Parent::m_uint_vectorizers.size(); ++i)
    {
      Parent::m_uint_vectorizers[i].first->get(float(key.get_uint_feat(i)), matrix, idx, Parent::m_features_pos[feat_idx]);
      feat_idx++;
    }

    const std::string& str = key.get_str_feat();
    const char* p = str.data();
    for (size_t i = 0; i < Parent::m_str_vectorizers.size(); ++i)
    {
      Parent::m_str_vectorizers[i].first->get(p, matrix, idx, Parent::m_features_pos[feat_idx]);
      feat_idx++;
      p = strchr(p, 0) + 1;
      assert(nullptr != p);
    }
  }

  virtual void init_cache(size_t dim)
  {
    if (m_precomputed_vectors.empty())
    {

      if (dim > 0)
      {
        m_temp_vectors
            = eigen_wrp::EigenMatrixXf::matrix_t::Zero(dim, 2);
      }
    }
  }

public:
  typedef typename Parent::feature_descr_t feature_descr_t;

  WordSeqEmbdVectorizerWithCache(size_t bucket_size = BUCKET_SIZE)
    : Parent(),
      m_bucket_size(bucket_size),
      m_next_free_idx(0),
      m_curr_bucket_id(0)
  {
    m_precomputed_vectors.resize(NUM_BUCKETS);
  }

  WordSeqEmbdVectorizerWithCache(const std::vector<feature_descr_t>& features, size_t bucket_size = BUCKET_SIZE)
    : Parent(),
      m_bucket_size(bucket_size),
      m_next_free_idx(0),
      m_curr_bucket_id(0)
  {
    init_features(features);
    m_precomputed_vectors.resize(NUM_BUCKETS);
  }

  bool is_precomputing() const
  {
    return false;
  }


  inline void precompute(const DataSet& dataset)
  {
  }

  virtual void init_features(const std::vector<feature_descr_t>& features)
  {
    Parent::init_features(features);
  }

  inline void create_key(timepoint_features_t& timepoint_features, const typename DataSet::token_t& token)
  {
    size_t feat_idx = 0;
    for (size_t i = 0; i < Parent::m_uint_vectorizers.size(); ++i)
    {
      const uint64_t feat_val = Parent::m_uint_feat_extractor.feat_value(token, Parent::m_uint_vectorizers[i].second);
      timepoint_features.set_uint_feat(i, feat_val);
      feat_idx++;
    }

    for (size_t i = 0; i < Parent::m_str_vectorizers.size(); ++i)
    {
      const std::string& feat_val = Parent::m_str_feat_extractor.feat_value(token, Parent::m_str_vectorizers[i].second);
      timepoint_features.append_str_feat(feat_val);
      feat_idx++;
    }
    timepoint_features.append_str_feat(0);
  }

  inline void vectorize_timepoint(MatrixFloat& target, uint64_t timepoint, const typename DataSet::token_t& token)
  {
    timepoint_features_t timepoint_features(Parent::m_uint_vectorizers.size());

    create_key(timepoint_features, token);

    eigen_wrp::EigenMatrixXf::matrix_t* p_matrix = nullptr;
    Idx idx = get_vector(timepoint_features, &p_matrix);
    assert(nullptr != p_matrix);
    target.col(timepoint) = p_matrix->col(idx);
  }
};

template <class Model,
          class DataSet,
          class StrFeatExtractor,
          class UIntFeatExtractor,
          class MatrixFloat,
          class Idx=uint64_t,
          size_t NUM_BUCKETS=32,
          size_t BUCKET_SIZE=1024*64>
class WordSeqEmbdVectorizerWithPrecomputing
    : public WordSeqEmbdVectorizerWithCache<DataSet,
                                            StrFeatExtractor,
                                            UIntFeatExtractor,
                                            MatrixFloat,
                                            Idx,
                                            NUM_BUCKETS,
                                            BUCKET_SIZE>
{
protected:
  typedef WordSeqEmbdVectorizerWithCache<DataSet,
                                         StrFeatExtractor,
                                         UIntFeatExtractor,
                                         MatrixFloat,
                                         Idx,
                                         NUM_BUCKETS,
                                         BUCKET_SIZE> Parent;

  Model* m_pModel;
  size_t m_precomputed_dim;

  virtual void init_cache(size_t dim)
  {
    Parent::m_precomputed_vectors.resize(NUM_BUCKETS);
    Parent::m_curr_bucket_id = 0;
    Parent::m_temp_vectors
        = eigen_wrp::EigenMatrixXf::matrix_t::Zero(dim, 2);
  }

public:

  WordSeqEmbdVectorizerWithPrecomputing(size_t bucket_size = BUCKET_SIZE)
    : Parent(bucket_size),
      m_pModel(nullptr),
      m_precomputed_dim(0)
  {
  }

  WordSeqEmbdVectorizerWithPrecomputing(const std::vector<typename Parent::feature_descr_t>& features, size_t bucket_size = 1024*64)
    : Parent(features, bucket_size),
      m_pModel(nullptr),
      m_precomputed_dim(0)
  {
  }

  bool is_precomputing() const
  {
    return true;
  }

  void set_model(Model* pModel)
  {
    assert(nullptr != pModel);
    m_pModel = pModel;
    m_precomputed_dim = pModel->get_precomputed_dim();
  }

  inline size_t dim()
  {
    return m_precomputed_dim;
  }

  inline void precompute(const DataSet& dataset)
  {
    assert(nullptr != m_pModel);

    eigen_wrp::EigenMatrixXf::matrix_t input
        = eigen_wrp::EigenMatrixXf::matrix_t::Zero(Parent::dim(), Parent::m_bucket_size);
    Parent::m_temp_vectors
        = eigen_wrp::EigenMatrixXf::matrix_t::Zero(Parent::dim(), 1);

    uint64_t curr = 0;
    while (curr < dataset.size())
    {
      if (Parent::m_curr_bucket_id > 2) break;

      for (long int i = 0; i < Parent::m_bucket_size && curr < dataset.size(); ++i)
      {
        const typename DataSet::token_t& token = dataset[curr];
        typename Parent::timepoint_features_t timepoint_features(Parent::m_uint_vectorizers.size());
        Parent::create_key(timepoint_features, token);
        Parent::compute_vector(timepoint_features, input, i);
        Parent::m_precomputed_index[timepoint_features] = curr;

        curr++;
      }

      if (0 == Parent::m_precomputed_vectors[Parent::m_curr_bucket_id].cols())
      {
        Parent::init_curr_bucket(m_precomputed_dim);
      }
      m_pModel->precompute_inputs(input, Parent::m_precomputed_vectors[Parent::m_curr_bucket_id], 0);
      Parent::m_curr_bucket_id++;

      if (Parent::m_curr_bucket_id >= Parent::m_precomputed_vectors.size())
      {
        Parent::m_precomputed_vectors.resize(Parent::m_curr_bucket_id + 1);
      }
    }
  }

  inline Idx get_vector(const typename Parent::timepoint_features_t& key, eigen_wrp::EigenMatrixXf::matrix_t** matrix)
  {
    auto it = Parent::m_precomputed_index.find(key);
    if (Parent::m_precomputed_index.end() != it)
    {
      Idx i = it->second;
      auto bucket_id = i / Parent::m_bucket_size;
      assert(size_t(bucket_id) < Parent::m_precomputed_vectors.size());
      i = i % Parent::m_bucket_size;
      *matrix = &(Parent::m_precomputed_vectors[bucket_id]);
      return i;
    }

    if (size_t(Parent::m_curr_bucket_id) < Parent::m_precomputed_vectors.size())
    {
      if (Parent::m_next_free_idx >= Parent::m_bucket_size)
      {
        Parent::m_curr_bucket_id++;
        if (size_t(Parent::m_curr_bucket_id) >= Parent::m_precomputed_vectors.size())
        {
          Parent::m_precomputed_vectors.resize(Parent::m_curr_bucket_id + 1);
        }
        Parent::m_next_free_idx = 0;
      }

      if (0 == Parent::m_precomputed_vectors[Parent::m_curr_bucket_id].cols())
      {
        Parent::init_curr_bucket(m_precomputed_dim);
      }

      Parent::compute_vector(key, Parent::m_temp_vectors, 0);
      m_pModel->precompute_inputs(Parent::m_temp_vectors,
                                  Parent::m_precomputed_vectors[Parent::m_curr_bucket_id],
                                  Parent::m_next_free_idx);
      Parent::m_precomputed_index[key] = Parent::m_curr_bucket_id * Parent::m_bucket_size + Parent::m_next_free_idx;
      *matrix = &(Parent::m_precomputed_vectors[Parent::m_curr_bucket_id]);
      Idx key_idx = Parent::m_next_free_idx;
      Parent::m_next_free_idx++;

      return key_idx;
    }

    Parent::compute_vector(key, Parent::m_temp_vectors, 0);
    *matrix = &(Parent::m_temp_vectors);
    return 0;
  }

  inline void vectorize_timepoint(MatrixFloat& target, uint64_t timepoint, const typename DataSet::token_t& token)
  {
    typename Parent::timepoint_features_t timepoint_features(Parent::m_uint_vectorizers.size());

    Parent::create_key(timepoint_features, token);

    eigen_wrp::EigenMatrixXf::matrix_t* p_matrix = nullptr;
    Idx idx = get_vector(timepoint_features, &p_matrix);
    assert(nullptr != p_matrix);
    target.col(timepoint) = p_matrix->col(idx);
  }
};

} // namespace vectorizers
} // namespace deeplima

#endif
