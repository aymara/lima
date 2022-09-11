// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_DEPENDENCY_PARSER
#define DEEPLIMA_DEPENDENCY_PARSER

#include <vector>
#include <string>
#include <memory>

#include "token_type.h"
#include "utils/str_index.h"
#include "helpers/path_resolver.h"
#include "graph_dp/impl/graph_dp_impl.h"

#include "graph_dp.h"

namespace deeplima
{

template <class Iterator, class Matrix=eigen_wrp::EigenMatrixXf>
class DependencyParser
{
public:

  struct token_with_analysis_t : public impl::token_t
  {
    uint32_t m_lemm_idx = 0;
    std::vector<uint8_t> m_classes;
    uint16_t m_head_idx = 0; // => max sentence length == 2^16
    uint16_t m_rel_type = 0;

    token_with_analysis_t(size_t num_classes)
      : m_classes(num_classes, 0)
    {
    }
  };

  struct tokens_with_analysis_t : public token_buffer_t<token_with_analysis_t>
  {
    typedef token_buffer_t<token_with_analysis_t> Parent;
    typedef token_with_analysis_t token_t;

    tokens_with_analysis_t(size_t size, size_t num_classes)
      : Parent(size, token_with_analysis_t(num_classes))
    {
    }
  };

  DependencyParser(const std::string& model_fn,
                   const PathResolver& path_resolver,
                   std::shared_ptr<StringIndex> stridx,
                   const std::vector<std::string>& input_class_names,
                   size_t buffer_size,
                   size_t num_buffers)
    : m_buffer_size(buffer_size),
      m_current_buffer(0),
      m_current_timepoint(0),
      m_stridx_ptr(stridx),
      m_stridx(*m_stridx_ptr)
  {
    assert(m_buffer_size > 0);
    assert(num_buffers > 0);
    m_buffers.reserve(num_buffers);

    m_impl.load(model_fn, path_resolver);
    m_impl.init(1, num_buffers, buffer_size, m_stridx, input_class_names);

    for (size_t i = 0; i < num_buffers; ++i)
    {
      m_buffers.emplace_back(m_buffer_size, input_class_names.size());
    }



  }

  void set_classes(size_t idx, const std::string& class_name, const std::vector<std::string>& data)
  {
    m_class_names.push_back(class_name);

    if (idx + 1 > m_classes.size())
    {
      m_classes.resize(idx + 1);
    }
    assert(0 == m_classes[idx].size());
    m_classes[idx] = data;

    if (m_classes.size() > 1)
    {
      //m_has_feats = true;
      for (size_t i = 0; i < m_class_names.size(); ++i)
      {
        const std::string& feat_name = m_class_names[i];
        if (feat_name == "upos" || feat_name == "xpos" || feat_name == "eos")
        {
          continue;
        }
        //m_first_feature_to_print = i;
        break;
      }
    }
  }

  void operator()(Iterator& iter)
  {
    if (m_current_timepoint >= m_buffer_size)
    {
      acquire_buffer();
    }

    size_t first_timepoint_idx = 0;
    std::vector<size_t> lengths;
    lengths.reserve(256);
    size_t tokens_to_process = count_max_tokens_until_eos(iter, lengths);
    bool insert_root = true;
    while (!iter.end())
    {
      assert(m_current_timepoint < m_buffer_size);
      assert(m_current_buffer < m_buffers.size());

      token_with_analysis_t& token = m_buffers[m_current_buffer][m_current_timepoint];

      if (insert_root)
      {
        token.m_offset = 0;
        token.m_len = 0;
        token.m_form_idx = m_stridx.get_idx("<ROOT>");
        token.m_flags = impl::token_t::token_flags_t(segmentation::token_pos::flag_t::none);
        token.m_lemm_idx = token.m_form_idx;
        insert_root = false;
        tokens_to_process--;
        m_current_timepoint++;
        continue;
      }
      else
      {
        token.m_offset = iter.token_offset();
        token.m_len = iter.token_len();
        token.m_form_idx = iter.form_idx();
        std::cerr << m_stridx.get_str(token.m_form_idx) << std::endl;
        token.m_flags = impl::token_t::token_flags_t(iter.flags());
        token.m_lemm_idx = iter.lemma_idx();
        for (size_t i = 0; i < m_classes.size(); ++i)
        {
          token.m_classes[i] = iter.token_class(i);
        }

        if (iter.flags() & segmentation::token_pos::flag_t::sentence_brk ||
            iter.flags() & segmentation::token_pos::flag_t::paragraph_brk)
        {
          insert_root = true;
        }
      }

      m_current_timepoint++;
      tokens_to_process--;

      if (m_current_timepoint >= m_buffer_size || 0 == tokens_to_process)
      {
        start_analysis(m_current_buffer, first_timepoint_idx, lengths, m_current_timepoint);
        if (!iter.end())
        {
          // if we can't wait until next call
          acquire_buffer();
          lengths.clear();
          tokens_to_process = count_max_tokens_until_eos(iter, lengths);
          insert_root = true;
          first_timepoint_idx = m_current_timepoint;
        }
      }

      iter.next();
    }
  }

protected:
  void acquire_buffer()
  {
    size_t next_buffer_idx = (m_current_buffer + 1 < m_buffers.size()) ? (m_current_buffer + 1) : 0;
    const token_buffer_t<token_with_analysis_t>& next_buffer = m_buffers[next_buffer_idx];

    // wait for buffer
    while (next_buffer.locked())
    {
      //m_cls.send_next_results();
    }
    assert(!next_buffer.locked());

    m_current_buffer = next_buffer_idx;
    m_current_timepoint = 0;
  }

  void start_analysis(size_t buffer_idx,
                      size_t first_timepoint_idx,
                      const std::vector<size_t>& lengths,
                      int count = -1)
  {
    assert(!m_buffers[buffer_idx].locked());
    m_buffers[buffer_idx].lock();

    const tokens_with_analysis_t& current_buffer = m_buffers[buffer_idx];
    m_impl.handle_token_buffer(buffer_idx,
                               first_timepoint_idx,
                               enriched_token_buffer_t(current_buffer, m_stridx),
                               lengths,
                               count);
  }

  size_t count_max_tokens_until_eos(Iterator& iter, std::vector<size_t>& lengths)
  {
    size_t current_iter_pos = iter.position();
    size_t tokens_counter = 0;
    size_t this_sentence_tokens = m_current_timepoint > 0 ? 0 : 1;

    if (m_current_timepoint + tokens_counter + this_sentence_tokens >= m_buffer_size)
    {
      throw std::runtime_error("No place for new tokens in this buffer");
    }

    while (!iter.end())
    {
      this_sentence_tokens++;

      if (iter.flags() & segmentation::token_pos::flag_t::sentence_brk ||
          iter.flags() & segmentation::token_pos::flag_t::paragraph_brk)
      {
        lengths.push_back(this_sentence_tokens);
        tokens_counter += this_sentence_tokens;
        this_sentence_tokens = 1;
      }

      if (m_current_timepoint + tokens_counter + this_sentence_tokens == m_buffer_size)
      {
        break;
      }
      if (m_current_timepoint + tokens_counter + this_sentence_tokens > m_buffer_size)
      {
        throw std::runtime_error("Too many tokens for one buffer");
      }

      iter.next();
    }

    iter.reset(current_iter_pos);
    return tokens_counter;
  }

  class enriched_token_t
  {
    friend class DependencyParser<Iterator, Matrix>;

  protected:
    const StringIndex& m_stridx;
    const typename tokens_with_analysis_t::token_t* m_ptoken;

    inline void set_token(const typename tokens_with_analysis_t::token_t* p)
    {
      m_ptoken = p;
    }
  public:

    enriched_token_t(const StringIndex& stridx)
      : m_stridx(stridx),
        m_ptoken(nullptr)
    { }

    inline typename tokens_with_analysis_t::token_t::token_flags_t flags() const
    {
      assert(nullptr != m_ptoken);
      return m_ptoken->m_flags;
    }

    inline bool eos() const
    {
      assert(nullptr != m_ptoken);
      return flags() & DependencyParser<Iterator, Matrix>::tokens_with_analysis_t::token_t::token_flags_t::sentence_brk;
    }

    inline uint32_t cls(size_t idx) const
    {
      assert(nullptr != m_ptoken);
      return m_ptoken->m_classes[idx];
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
    const tokens_with_analysis_t& m_data;
    mutable enriched_token_t m_token; // WARNING: only one iterator is supported

  public:
    typedef enriched_token_t token_t;

    enriched_token_buffer_t(const tokens_with_analysis_t& data, const StringIndex& stridx)
      : m_data(data), m_token(stridx) { }

    inline typename tokens_with_analysis_t::size_type size() const
    {
      return m_data.size();
    }

    inline const enriched_token_t& operator[](size_t idx) const
    {
      m_token.set_token(m_data.data() + idx);
      return m_token;
    }
  };

#ifdef DP_VECTORIZER_WITH_PRECOMPUTING
  typedef graph_dp::impl::FeaturesVectorizerWithPrecomputing<
                                      tagging::impl::EntityTaggingClassifier,
                                      enriched_token_buffer_t,
                                      typename enriched_token_buffer_t::token_t> FeaturesVectorizer;
#elif DP_VECTORIZER_WITH_CACHE
  typedef graph_dp::impl::FeaturesVectorizerWithCache<
                                      enriched_token_buffer_t,
                                      typename enriched_token_buffer_t::token_t> FeaturesVectorizer;
#else
  typedef graph_dp::impl::FeaturesVectorizer<
                                      enriched_token_buffer_t,
                                      typename enriched_token_buffer_t::token_t> FeaturesVectorizer;
#endif

  typedef graph_dp::impl::GraphDpImpl< graph_dp::impl::GraphDependencyParser,
                                       FeaturesVectorizer,
                                       Matrix > DependencyParsingModule;

  std::vector<std::string> m_class_names;
  std::vector<std::vector<std::string>> m_classes;

  size_t m_buffer_size;
  size_t m_current_buffer;
  size_t m_current_timepoint;

  std::shared_ptr</*const*/ StringIndex> m_stridx_ptr;
  /*const*/ StringIndex& m_stridx;
  StringIndex::idx_t m_unk_idx;
  std::vector<tokens_with_analysis_t> m_buffers;

  DependencyParsingModule m_impl;
};

} // deeplima

#endif // DEEPLIMA_DEPENDENCY_PARSER