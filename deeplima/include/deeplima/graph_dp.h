// Copyright 2022 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_GRAPH_DP_H
#define DEEPLIMA_GRAPH_DP_H

#include "config.h"

#include "nets/birnn_seq_cls.h"

#include "eigen_wrp/eigen_matrix.h"
#include "eigen_wrp/embd_dict.h"
#include "eigen_wrp/dict_embd_vectorizer.h"
#include "eigen_wrp/graph_dp_eigen_inference_impl.h"

#include "graph_dp/impl/graph_dp_wrapper.h"
#include "eigen_wrp/word_seq_embd_vectorizer.h"
#include "feat_extractors.h"

namespace deeplima::graph_dp
{

namespace eigen_impl
{
  typedef impl::GraphDpInferenceWrapper<BiRnnAndDeepBiaffineAttentionEigenInference> Model;
  typedef DictEmbdVectorizer<EmbdUInt64FloatHolder, EmbdUInt64Float, eigen_wrp::EigenMatrixXf> EmbdVectorizer;

  template <class TokenVector, class Token>
  class FeaturesVectorizer:
      public vectorizers::WordSeqEmbdVectorizer<TokenVector,
                                                TokenStrFeatExtractor<Token>,
                                                TokenUIntClsFeatExtractor<Token>,
                                                eigen_wrp::EigenMatrixXf::matrix_t,
                                                Eigen::Index>
  {};

  template <class TokenVector, class Token>
  class FeaturesVectorizerWithCache:
      public vectorizers::WordSeqEmbdVectorizerWithCache<TokenVector,
                                                         TokenStrFeatExtractor<Token>,
                                                         TokenUIntClsFeatExtractor<Token>,
                                                         eigen_wrp::EigenMatrixXf::matrix_t,
                                                         Eigen::Index>
  {};

  template <class Model, class TokenVector, class Token>
  class FeaturesVectorizerWithPrecomputing:
      public vectorizers::WordSeqEmbdVectorizerWithPrecomputing<Model,
                                                                TokenVector,
                                                                TokenStrFeatExtractor<Token>,
                                                                TokenUIntClsFeatExtractor<Token>,
                                                                eigen_wrp::EigenMatrixXf::matrix_t,
                                                                Eigen::Index>
  {};

} // namespace eigen_impl


namespace impl
{

  typedef eigen_impl::Model Model;

  template <class TokenVector, class Token>
  class FeaturesVectorizer: public eigen_impl::FeaturesVectorizer<TokenVector, Token> {};

  template <class TokenVector, class Token>
  class FeaturesVectorizerWithCache: public eigen_impl::FeaturesVectorizerWithCache<TokenVector, Token> {};

  template <class Model, class TokenVector, class Token>
  class FeaturesVectorizerWithPrecomputing: public eigen_impl::FeaturesVectorizerWithPrecomputing<Model, TokenVector, Token> {};

  typedef eigen_wrp::EigenMatrixXf BaseMatrix;


  typedef RnnSequenceClassifier<Model, BaseMatrix, uint32_t> GraphDependencyParser;

} // namespace impl

} // namespace graph_dp
 // namespace deeplima

#endif
