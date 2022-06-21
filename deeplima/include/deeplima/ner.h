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

#ifndef DEEPLIMA_NER_H
#define DEEPLIMA_NER_H

#include "config.h"

#include "nets/birnn_seq_cls.h"

#if DEEPLIMA_INFERENCE_ENGINE == IE_TORCH

//#include "torch_wrp/dict_vectorizer.h"
//#include "static_graph/dict.h"

#endif

#if DEEPLIMA_INFERENCE_ENGINE == IE_EIGEN

#include "eigen_wrp/eigen_matrix.h"
#include "eigen_wrp/embd_dict.h"
#include "eigen_wrp/dict_embd_vectorizer.h"
#include "eigen_wrp/tagging_eigen_inference_impl.h"

#endif

#include "tagging/impl/tagging_impl.h"
#include "tagging/impl/tagging_wrapper.h"
#include "eigen_wrp/word_seq_embd_vectorizer.h"
#include "feat_extractors.h"

namespace deeplima
{
namespace tagging
{

#if DEEPLIMA_INFERENCE_ENGINE == IE_TORCH

namespace torch_impl
{
  //typedef impl::SegmentationInferenceWrapper<train::BiRnnClassifierForSegmentationImpl> Model;
  //typedef DictVectorizer<DictsHolder, UInt64Dict, TorchMatrix<int64_t>> EmbdVectorizer;

} // namespace torch_impl

#elif DEEPLIMA_INFERENCE_ENGINE == IE_EIGEN

namespace eigen_impl
{
  typedef impl::TaggingInferenceWrapper<BiRnnEigenInferenceForTaggingF> Model;
  typedef DictEmbdVectorizer<EmbdUInt64FloatHolder, EmbdUInt64Float, eigen_wrp::EigenMatrixXf> EmbdVectorizer;
  /*typedef WordSeqEmbdVectorizer<token_buffer_t,
                                TokenStrFeatExtractor<token_buffer_t::token_t>,
                                TokenUIntFeatExtractor<token_buffer_t::token_t>,
                                eigen_wrp::EigenMatrixXf> FeaturesVectorizer;*/

  template <class TokenVector, class Token>
  class FeaturesVectorizer: public WordSeqEmbdVectorizer<TokenVector,
                                                     TokenStrFeatExtractor<Token>,
                                                     TokenUIntFeatExtractor<Token>,
                                                     eigen_wrp::EigenMatrixXf::matrix_t,
                                                     Eigen::Index>
  {};

  template <class TokenVector, class Token>
  class FeaturesVectorizerWithCache: public WordSeqEmbdVectorizerWithCache<TokenVector,
                                                     TokenStrFeatExtractor<Token>,
                                                     TokenUIntFeatExtractor<Token>,
                                                     eigen_wrp::EigenMatrixXf::matrix_t,
                                                     Eigen::Index>
  {};

  template <class Model, class TokenVector, class Token>
  class FeaturesVectorizerWithPrecomputing: public WordSeqEmbdVectorizerWithPrecomputing<
                                                     Model,
                                                     TokenVector,
                                                     TokenStrFeatExtractor<Token>,
                                                     TokenUIntFeatExtractor<Token>,
                                                     eigen_wrp::EigenMatrixXf::matrix_t,
                                                     Eigen::Index>
  {};

} // namespace eigen_impl

#else
#error Unknown inference engine
#endif

namespace impl
{
#if DEEPLIMA_INFERENCE_ENGINE == IE_TORCH

  //typedef torch_impl::Model Model;
  //typedef torch_impl::EmbdVectorizer EmbdVectorizer;

#elif DEEPLIMA_INFERENCE_ENGINE == IE_EIGEN

  typedef eigen_impl::Model Model;

  template <class TokenVector, class Token>
  class FeaturesVectorizer: public eigen_impl::FeaturesVectorizer<TokenVector, Token> {};

  template <class TokenVector, class Token>
  class FeaturesVectorizerWithCache: public eigen_impl::FeaturesVectorizerWithCache<TokenVector, Token> {};

  template <class Model, class TokenVector, class Token>
  class FeaturesVectorizerWithPrecomputing: public eigen_impl::FeaturesVectorizerWithPrecomputing<Model, TokenVector, Token> {};

  typedef eigen_wrp::EigenMatrixXf BaseMatrix;

#else
#error Unknown inference engine
#endif

  typedef RnnSequenceClassifier<Model, BaseMatrix> EntityTaggingClassifier;

  //typedef impl::TaggingImpl< EntityTaggingClassifier, int
  //                           //impl::SegmentationDecoder<SegmentationClassifier::OutputMatrix>,
  //                           FeaturesVectorizer > EntityTaggingModule;

} // namespace impl

//typedef impl::EntityTaggingModule EntityTagger;

} // namespace tagging
} // namespace deeplima

#endif
