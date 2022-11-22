// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_TOKEN_SEQUENCE_ANALYZER
#define DEEPLIMA_TOKEN_SEQUENCE_ANALYZER

#include <memory>

#include "segmentation.h"
#include "helpers/path_resolver.h"
#include "utils/str_index.h"
#include "token_type.h"
#include "ner.h"
#include "lemmatization.h"

namespace deeplima
{

template <class Matrix=eigen_wrp::EigenMatrixXf>
class TokenSequenceAnalyzer
{
public:
  class TokenIterator
  {
    const StringIndex& m_stridx;
    const token_buffer_t<>& m_buffer;
    const std::vector<StringIndex::idx_t> m_lemm_buffer;
    std::shared_ptr< StdMatrix<uint8_t> > m_classes;
    size_t m_current;
    size_t m_offset;
    size_t m_end;

  public:
    TokenIterator(const StringIndex& stridx, const token_buffer_t<>& buffer,
                  const std::vector<StringIndex::idx_t>& lemm_buffer,
                  std::shared_ptr< StdMatrix<uint8_t> > classes, size_t offset, size_t end)
      : m_stridx(stridx), m_buffer(buffer), m_lemm_buffer(lemm_buffer), m_classes(classes),
        m_current(0), m_offset(offset), m_end(end - offset)
    {
      assert(end > offset + 1);
    }

    inline bool end() const
    {
      return m_current >= m_end;
    }

    inline impl::token_t::token_flags_t flags() const
    {
      assert(! end());
      return m_buffer[m_current].m_flags;
    }

    inline uint16_t token_offset() const
    {
      return m_buffer[m_current].m_offset;
    }

    inline uint16_t token_len() const
    {
      return m_buffer[m_current].m_len;
    }

    inline uint32_t form_idx() const
    {
      return m_buffer[m_current].m_form_idx;
    }

    inline uint32_t lemma_idx() const
    {
      return m_lemm_buffer[m_current];
    }

    inline const char* form() const
    {
      assert(! end());
      const std::string& f = m_stridx.get_str(m_buffer[m_current].m_form_idx);
      return f.c_str();
    }

    inline const char* lemma() const
    {
      assert(! end());
      const std::string& f = m_stridx.get_str(m_lemm_buffer[m_current]);
      return f.c_str();
    }

    inline uint32_t head() const
    {
      throw std::runtime_error("TokenSequenceAnalyzer<T> does not implement head");
      return 0;
    }

    inline void next()
    {
      m_current++;
    }

    inline void reset(size_t position = 0)
    {
      m_current = position;
    }

    inline size_t position() const
    {
      return m_current;
    }

    inline uint8_t token_class(size_t cls_idx) const
    {
        // std::cerr << "time: " << m_current + m_offset << "\n";
        // std::cerr << "cls_idx: " << cls_idx << "\n";
      uint8_t val = m_classes->get(m_current + m_offset, cls_idx);
      return val;
    }
  };

  typedef TokenIterator output_iterator_t;

protected:

  class enriched_token_t
  {
    friend class TokenSequenceAnalyzer;

  protected:
    const StringIndex& m_stridx;
    const token_buffer_t<>::token_t* m_ptoken;

    inline void set_token(const token_buffer_t<>::token_t* p)
    {
      m_ptoken = p;
    }
  public:

    enriched_token_t(const StringIndex& stridx)
      : m_stridx(stridx),
        m_ptoken(nullptr)
    { }

    inline token_buffer_t<>::token_t::token_flags_t flags() const
    {
      assert(nullptr != m_ptoken);
      return m_ptoken->m_flags;
    }

    inline bool eos() const
    {
      assert(nullptr != m_ptoken);
      return flags() & token_buffer_t<>::token_t::token_flags_t::sentence_brk;
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
    const token_buffer_t<>& m_data;
    mutable enriched_token_t m_token; // WARNING: only one iterator is supported

  public:
    typedef enriched_token_t token_t;

    enriched_token_buffer_t(const token_buffer_t<>& data, const StringIndex& stridx)
      : m_data(data), m_token(stridx) { }

    inline token_buffer_t<>::size_type size() const
    {
      return m_data.size();
    }

    inline const enriched_token_t& operator[](size_t idx) const
    {
      m_token.set_token(m_data.data() + idx);
      return m_token;
    }
  };

  //typedef tagging::impl::FeaturesVectorizer<
  //                                    enriched_token_buffer_t,
  //                                    typename enriched_token_buffer_t::token_t> FeaturesVectorizer;

  //typedef tagging::impl::FeaturesVectorizerWithCache<
  //                                    enriched_token_buffer_t,
  //                                    typename enriched_token_buffer_t::token_t> FeaturesVectorizer;

  typedef tagging::impl::FeaturesVectorizerWithPrecomputing<
                                      tagging::impl::EntityTaggingClassifier,
                                      enriched_token_buffer_t,
                                      typename enriched_token_buffer_t::token_t> FeaturesVectorizer;

  typedef tagging::impl::TaggingImpl< tagging::impl::EntityTaggingClassifier,
                                      FeaturesVectorizer,
                                      Matrix > EntityTaggingModule;

  typedef DictEmbdVectorizer<EmbdUInt64FloatHolder, EmbdUInt64Float, eigen_wrp::EigenMatrixXf> EmbdVectorizer;
  typedef lemmatization::impl::LemmatizationImpl< lemmatization::impl::Lemmatizer, EmbdVectorizer, Matrix> LemmatizationModule;

public:
  TokenSequenceAnalyzer() :
      m_buffer_size(0),
      m_current_buffer(0),
      m_current_timepoint(0),
      m_stridx_ptr(std::make_shared<StringIndex>()),
      m_stridx(*m_stridx_ptr),
      m_classes(std::make_shared<StdMatrix<uint8_t>>())
  {
  }

  TokenSequenceAnalyzer(const std::string& model_fn, const std::string& lemm_model_fn,
                        const PathResolver& path_resolver, size_t buffer_size, size_t num_buffers)
    : m_buffer_size(buffer_size),
      m_current_buffer(0),
      m_current_timepoint(0),
      m_stridx_ptr(std::make_shared<StringIndex>()),
      m_stridx(*m_stridx_ptr),
      m_classes(std::make_shared<StdMatrix<uint8_t>>())
  {
    assert(m_buffer_size > 0);
    assert(num_buffers > 0);
    m_buffers.resize(num_buffers);
    m_lemm_buffers.resize(num_buffers);
    for ( token_buffer_t<>& b : m_buffers ) b.resize(m_buffer_size);
    for ( auto& b : m_lemm_buffers ) b.resize(m_buffer_size);

    m_cls.load(model_fn, path_resolver);
    m_cls.init(1, num_buffers, buffer_size, m_stridx);

    m_unk_idx = m_stridx.get_idx("_");
    for ( auto& b : m_lemm_buffers )
      std::fill(b.begin(), b.end(), m_unk_idx);

    {
      token_buffer_t<> buff(m_stridx.size());
      for (uint32_t i = 0; i < buff.size(); ++i)
      {
        buff[i].m_form_idx = i;
      }

      m_cls.precompute_inputs(enriched_token_buffer_t(buff, m_stridx));
    }

    if (lemm_model_fn.size() > 0)
    {
      m_lemm.load(lemm_model_fn, path_resolver);
      m_lemm.init(128, m_cls.get_output_str_dicts_names(), m_cls.get_output_str_dicts());

      m_cls.register_handler([this](
                             std::shared_ptr< StdMatrix<uint8_t> > classes,
                             size_t begin, size_t end, size_t slot_idx){
        std::cerr << "handler called: " << slot_idx << std::endl;

        lemmatize(m_buffers[slot_idx], m_lemm_buffers[slot_idx], classes, begin, end);

        m_output_callback(m_stridx_ptr,
                          m_buffers[slot_idx],
                          m_lemm_buffers[slot_idx],
                          classes,
                          begin,
                          end);

        m_buffers[slot_idx].unlock();
      });
    }
    else
    {
      m_cls.register_handler([this](
                             std::shared_ptr< StdMatrix<uint8_t> > classes,
                             size_t begin, size_t end, size_t slot_idx){
        std::cerr << "handler called: " << slot_idx << std::endl;
        m_classes = classes;
        m_output_callback(m_stridx_ptr,
                          m_buffers[slot_idx],
                          m_lemm_buffers[slot_idx],
                          m_classes,
                          begin,
                          end);

        m_buffers[slot_idx].unlock();
      });
    }
  }

    void get_classes_from_fn(const std::string& fn, std::vector<std::string>& classes_names, std::vector<std::vector<std::string>>& classes){
       m_cls.get_classes_from_fn(fn, classes_names, classes);
    }

  ~TokenSequenceAnalyzer()
  {
    std::cerr << "~TokenSequenceAnalyzer" << std::endl;
  }

  typedef std::function < void (std::shared_ptr< StringIndex > stridx,
                                const token_buffer_t<>& tokens,
                                const std::vector<StringIndex::idx_t>& lemmata,
                                std::shared_ptr< StdMatrix<uint8_t> > classes,
                                size_t begin,
                                size_t end) > output_callback_t;

  void register_handler(const output_callback_t fn) {
    m_output_callback = fn;
  }

  const std::vector<std::vector<std::string>>& get_classes() const
  {
    return m_cls.get_output_str_dicts();
  }

  const std::vector<std::string>& get_class_names() const
  {
    return m_cls.get_output_str_dicts_names();
  }

  std::shared_ptr<StringIndex> get_stridx() const
  {
    return m_stridx_ptr;
  }

  void finalize()
  {
    std::cerr << "TokenSequenceAnalyzer::finalize" << std::endl;
    if (m_current_timepoint > 0)
    {
      if (m_current_timepoint < m_buffer_size)
      {
        std::cerr << "TokenSequenceAnalyzer::finalize call start_analysis" << std::endl;
        start_analysis(m_current_buffer, m_current_timepoint);
      }
      else
      {
        m_cls.no_more_data(m_current_buffer);
      }
    }

    m_cls.send_all_results();
  }

  inline void operator()(const std::vector<deeplima::segmentation::token_pos>& tokens, uint32_t len)
  {
    if (m_current_timepoint >= m_buffer_size)
    {
      acquire_buffer();
    }

    for (size_t i = 0; i < len; i++)
    {
      assert(m_current_timepoint < m_buffer_size);
      assert(m_current_buffer < m_buffers.size());

      impl::token_t& token = m_buffers[m_current_buffer][m_current_timepoint];
      const segmentation::token_pos& src = tokens[i];
      token.m_offset = src.m_offset;
      token.m_len = src.m_len;
      token.m_form_idx = m_stridx.get_idx(src.m_pch, src.m_len);
      token.m_flags = impl::token_t::token_flags_t(src.m_flags);

      m_current_timepoint++;
      if (m_current_timepoint >= m_buffer_size)
      {
        start_analysis(m_current_buffer);
        if (i < len - 1)
        {
          // if we can't wait until next call
          acquire_buffer();
        }
      }
    }
  }

  void acquire_buffer()
  {
    size_t next_buffer_idx = (m_current_buffer + 1 < m_buffers.size()) ? (m_current_buffer + 1) : 0;
    const token_buffer_t<>& next_buffer = m_buffers[next_buffer_idx];

    // wait for buffer
    while (next_buffer.locked())
    {
      m_cls.send_next_results();
    }
    assert(!next_buffer.locked());

    m_current_buffer = next_buffer_idx;
    m_current_timepoint = 0;
  }

  void start_analysis(size_t buffer_idx, int count = -1)
  {
    std::cerr << "TokenSequenceAnalyzer::start_analysis " << buffer_idx << ", " << count << std::endl;
    assert(!m_buffers[buffer_idx].locked());
    m_buffers[buffer_idx].lock();

    const token_buffer_t<>& current_buffer = m_buffers[buffer_idx];
    m_cls.handle_token_buffer(buffer_idx, enriched_token_buffer_t(current_buffer, m_stridx), count);
  }

protected:

  void process_buffer(size_t buffer_idx)
  {
    const token_buffer_t<>& current_buffer = m_buffers[buffer_idx];
    m_cls.handle_token_buffer(buffer_idx, enriched_token_buffer_t(current_buffer, m_stridx));
  }

  void lemmatize(const token_buffer_t<>& buffer,
                 std::vector<StringIndex::idx_t>& lemm_buffer,
                 std::shared_ptr< StdMatrix<uint8_t> > classes, size_t offset, size_t end)
  {
    std::u32string target;
    for (size_t i = 0; i < end - offset; ++i)
    {
      if (m_lemm.is_fixed(classes, i + offset))
      {
        lemm_buffer[i] = buffer[i].m_form_idx;
      }
      else
      {
        const std::u32string& f = m_stridx.get_ustr(buffer[i].m_form_idx);
        m_lemm.predict(f, classes, i + offset, target);

        lemm_buffer[i] = m_stridx.get_idx(target);
      }
    }
  }

  size_t m_buffer_size;
  size_t m_current_buffer;
  size_t m_current_timepoint;

  std::shared_ptr<StringIndex> m_stridx_ptr;
  StringIndex& m_stridx;
  StringIndex::idx_t m_unk_idx;
  std::vector<token_buffer_t<>> m_buffers;
  std::vector<std::vector<StringIndex::idx_t>> m_lemm_buffers; // access control is sync with m_buffers

  EntityTaggingModule m_cls;
  LemmatizationModule m_lemm;
  std::shared_ptr< StdMatrix<uint8_t> > m_classes;

  output_callback_t m_output_callback;
};

} // namespace deeplima

#endif
