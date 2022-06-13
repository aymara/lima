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
    class TokenIterator
    {
        const StringIndex& m_stridx;
        const token_buffer_t& m_buffer;
        const std::vector<StringIndex::idx_t> m_lemm_buffer;
        const StdMatrix<uint8_t>& m_classes;
        size_t m_current;
        size_t m_offset;
        size_t m_end;

    public:
        TokenIterator(const StringIndex& stridx, const token_buffer_t& buffer,
                      const std::vector<StringIndex::idx_t>& lemm_buffer,
                      const StdMatrix<uint8_t>& classes, size_t offset, size_t end)
                : m_stridx(stridx), m_buffer(buffer), m_lemm_buffer(lemm_buffer), m_classes(classes),
                  m_current(0), m_offset(offset), m_end(end - offset)
        {
            assert(end > offset + 1);
        }

        inline bool end() const
        {
            return m_current >= m_end;
        }

        token_buffer_t::token_t::token_flags_t flags() const
        {
            assert(! end());
            return m_buffer[m_current].m_flags;
        }

        const char* form() const
        {
            assert(! end());
            const std::string& f = m_stridx.get_str(m_buffer[m_current].m_form_idx);
            return f.c_str();
        }

        const char* lemma() const
        {
            assert(! end());
            const std::string& f = m_stridx.get_str(m_lemm_buffer[m_current]);
            return f.c_str();
        }

        void next()
        {
            m_current++;
        }

        uint8_t token_class(size_t cls_idx) const
        {
            uint8_t val = m_classes.get(m_current + m_offset, cls_idx);
            return val;
        }
    };
    class enriched_token_t
    {
    protected:
        const StringIndex& m_stridx;
        const token_buffer_t::token_t* m_ptoken;


    public:

        enriched_token_t(const StringIndex& stridx)
                : m_stridx(stridx),
                  m_ptoken(nullptr)
        { }

        inline token_buffer_t::token_t::token_flags_t flags() const
        {
            assert(nullptr != m_ptoken);
            return m_ptoken->m_flags;
        }

        inline void set_token(const token_buffer_t::token_t* p)
        {
            m_ptoken = p;
        }

        inline bool eos() const
        {
            assert(nullptr != m_ptoken);
            return flags() & token_buffer_t::token_t::token_flags_t::sentence_brk;
        }

        inline const std::string& form() const
        {
            assert(nullptr != m_ptoken);
            const std::string& f = m_stridx.get_str(m_ptoken->m_form_idx);
            return f;
        }
    };
    class enriched_token_buffer_t
    {
        const token_buffer_t& m_data;
        mutable enriched_token_t m_token; // WARNING: only one iterator is supported

    public:
        typedef enriched_token_t token_t;

        enriched_token_buffer_t(const token_buffer_t& data, const StringIndex& stridx)
                : m_data(data), m_token(stridx) { }

        inline token_buffer_t::size_type size() const
        {
            return m_data.size();
        }

        inline const enriched_token_t& operator[](size_t idx) const
        {
            m_token.set_token(m_data.data() + idx);
            return m_token;
        }
    };

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

  typedef impl::TaggingImpl< EntityTaggingClassifier,
                             FeaturesVectorizer<enriched_token_buffer_t, typename enriched_token_buffer_t::token_t>,
                             BaseMatrix> EntityTaggingModule;

} // namespace impl

//typedef impl::EntityTaggingModule EntityTagger;

} // namespace tagging
} // namespace deeplima

#endif
