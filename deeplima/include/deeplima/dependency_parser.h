// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_DEPENDENCY_PARSER
#define DEEPLIMA_DEPENDENCY_PARSER

#include <vector>
#include <string>
#include <memory>
#include <chrono>
#include <thread>

#include "token_type.h"
#include "utils/str_index.h"
#include "helpers/path_resolver.h"
#include "deeplima/graph_dp.h"
// #include "graph_dp/impl/graph_dp_impl.h"
#include "segmentation/impl/segmentation_decoder.h"
#include "token_sequence_analyzer.h"

#include "graph_dp.h"

namespace deeplima
{

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
      : impl::token_t(), m_classes(num_classes, 0)
    {
    }

    token_with_analysis_t(const token_with_analysis_t& t) = default;
    token_with_analysis_t& operator=(const token_with_analysis_t& t) = default;
    virtual ~token_with_analysis_t() = default;
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

  class enriched_token_t
  {
    friend class DependencyParser;

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
      return flags() & DependencyParser::tokens_with_analysis_t::token_t::token_flags_t::sentence_brk;
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

  class TokenIterator
  {
    const StringIndex& m_stridx;
    const std::vector<token_with_analysis_t>& m_buffer;
    std::shared_ptr< StdMatrix<uint32_t> > m_heads;
    size_t m_current;
    size_t m_offset;
    size_t m_end;

  public:
    TokenIterator(const StringIndex& stridx,
                  const std::vector<token_with_analysis_t>& buffer,
                  std::shared_ptr< StdMatrix<uint32_t> > heads,
                  size_t offset,
                  size_t end)
      : m_stridx(stridx), m_buffer(buffer), m_heads(heads),
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
      throw std::runtime_error("TokenIterator::lemma_idx");
      return 0;
    }

    inline uint32_t head() const
    {
      return m_heads->get(m_current, 0);
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
      const std::string& f = m_stridx.get_str(m_buffer[m_current].m_lemm_idx);
      return f.c_str();
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
      assert(m_offset == 0);
      uint8_t val = m_buffer[m_current].m_classes[cls_idx];
      return val;
    }
  };

  typedef std::function < void (const StringIndex& stridx,
                                const std::vector<token_with_analysis_t>& tokens,
                                std::shared_ptr< StdMatrix<uint32_t> > heads,
                                size_t begin,
                                size_t end) > output_callback_t;

  DependencyParser(const std::string& model_fn,
                   const PathResolver& path_resolver,
                   std::shared_ptr< StringIndex > stridx,
                   const std::vector<std::string>& input_class_names,
                   size_t buffer_size,
                   size_t num_buffers)
    : m_buffer_size(buffer_size),
      m_current_buffer(0),
      m_current_timepoint(0),
      m_stridx_ptr(stridx)//,
      // m_stridx(*stridx)
  {
    assert(m_buffer_size > 0);
    assert(num_buffers > 0);
    m_buffers.reserve(num_buffers);

    m_impl.load(model_fn, path_resolver);
    m_impl.init(1, num_buffers, buffer_size, *m_stridx_ptr, input_class_names);

    for (size_t i = 0; i < num_buffers; ++i)
    {
      m_buffers.emplace_back(m_buffer_size, input_class_names.size());
    }

    m_impl.register_handler([this](
                            std::shared_ptr< StdMatrix<uint32_t> > heads,
                            size_t begin, size_t end, size_t slot_idx) {
       // std::cerr << "handler called (dp): " << slot_idx << std::endl;

       m_output_callback(*m_stridx_ptr,
                         m_buffers[slot_idx],
                         heads,
                         begin,
                         end);

       m_buffers[slot_idx].unlock();
     });

  }

  ~DependencyParser()
  {
    // std::cerr << "~DependencyParser" << std::endl;
  }

  void register_handler(const output_callback_t fn) {
    m_output_callback = fn;
  }

  void setStringIndex(std::shared_ptr<deeplima::StringIndex> stringIndexPtr) {
    m_stridx_ptr = stringIndexPtr;
    // m_stridx = StringIndex(*m_stridx_ptr;
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

  void operator()(TokenSequenceAnalyzer<>::TokenIterator& iter)
  {
    // std::cerr << "DependencyParser::operator()" << std::endl;
    if (m_current_timepoint >= m_buffer_size)
    {
      acquire_buffer();
    }

    size_t first_timepoint_idx = 0;
    m_lengths.reserve(256);
    size_t tokens_to_process = count_max_tokens_until_eos(iter, m_lengths);
    // std::cerr << "DependencyParser::operator() tokens_to_process: " << tokens_to_process << std::endl;
    bool insert_root = true;
    while (!iter.end())
    {
      assert(m_current_timepoint < m_buffer_size);
      assert(m_current_buffer < m_buffers.size());

      token_with_analysis_t& token = m_buffers[m_current_buffer][m_current_timepoint];
      // std::cerr << "DependencyParser::operator() processing token: " << iter.form() << std::endl;

      if (insert_root)
      {
        token.m_offset = 0;
        token.m_len = 0;
        token.m_form_idx = m_stridx_ptr->get_idx("<ROOT>");
        // std::cerr << "<ROOT>" << std::endl;
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
        // std::cerr << iter.form() << std::endl;
        // std::cerr << m_stridx_ptr->get_str(token.m_form_idx) << std::endl;
        token.m_flags = iter.flags();
        token.m_lemm_idx = iter.lemma_idx();
        // token.m_head_idx = iter.head();
        // token.m_rel_type = 0; // TODO where is stored the rel type ???
        // TODO lines below were commented out to avoid crash. Does it really work?
        token.m_head_idx = iter.position();
        token.m_rel_type = 0; // TODO where is stored the rel type ???
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
      iter.next();

      if (m_current_timepoint >= m_buffer_size || 0 == tokens_to_process)
      {
        // std::cerr << "DependencyParser::operator() call start_analysis " << m_current_buffer << ", "
        //           << first_timepoint_idx << "," << m_current_timepoint << std::endl;
        start_analysis(m_current_buffer, first_timepoint_idx, m_lengths, m_current_timepoint);
        m_started = true;

        // if we can't wait until next call
        acquire_buffer();

        if (!iter.end())
        {
          m_lengths.clear();
                  tokens_to_process = count_max_tokens_until_eos(iter, m_lengths);
          insert_root = true;
          first_timepoint_idx = m_current_timepoint;
        }
      }
    }

    m_started = true;
  }

  void finalize()
  {
    // std::cerr << "DependencyParser::finalize" << std::endl;
    while (!m_started)
    {
      // std::cerr << "DependencyParser::finalize sleeping" << std::endl;
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    if (m_current_timepoint > 0)
    {
      if (m_current_timepoint < m_buffer_size)
      {
        // std::cerr << "DependencyParser::finalize call start_analysis" << std::endl;
        start_analysis(m_current_buffer, m_current_timepoint, m_lengths, -1);
      }
      else
      {
        //m_cls.no_more_data(m_current_buffer);
      }
    }

    // std::cerr << "DependencyParser::finalize call send_all_results" << std::endl;
    m_impl.send_all_results();
  }

protected:
  void acquire_buffer()
  {
    // std::cerr << "DependencyParser::acquire_buffer" << std::endl;
    size_t next_buffer_idx = (m_current_buffer + 1 < m_buffers.size()) ? (m_current_buffer + 1) : 0;
    const token_buffer_t<token_with_analysis_t>& next_buffer = m_buffers[next_buffer_idx];

    // wait for buffer
    while (next_buffer.locked())
    {
      // std::cerr << "DependencyParser::acquire_buffer locked" << std::endl;
      m_impl.send_next_results();
    }
    assert(!next_buffer.locked());

    m_current_buffer = next_buffer_idx;
    m_current_timepoint = 0;
    // std::cerr << "DependencyParser::acquire_buffer DONE:" << m_current_buffer << std::endl;
  }

  void start_analysis(size_t buffer_idx,
                      size_t first_timepoint_idx,
                      const std::vector<size_t>& lengths,
                      int count = -1)
  {
    // std::cerr << "DependencyParser::start_analysis buffer_idx=" << buffer_idx << ", first_timepoint_idx=" << first_timepoint_idx
    //           << ", lengths=" << lengths << ", count=" << count << std::endl;
    assert(!m_buffers[buffer_idx].locked());
    m_buffers[buffer_idx].lock();

    const tokens_with_analysis_t& current_buffer = m_buffers[buffer_idx];
    m_impl.handle_token_buffer(buffer_idx,
                               first_timepoint_idx,
                               enriched_token_buffer_t(current_buffer, *m_stridx_ptr),
                               lengths,
                               count);
  }

  size_t count_max_tokens_until_eos(TokenSequenceAnalyzer<>::TokenIterator& iter, std::vector<size_t>& lengths)
  {
    size_t current_iter_pos = iter.position();
    size_t tokens_counter = 0;
    size_t this_sentence_tokens = m_current_timepoint > 0 ? 0 : 1;

    // std::cerr << "DependencyParser::count_max_tokens_until_eos m_current_timepoint=" << m_current_timepoint
    //           << "; tokens_counter=" << tokens_counter << "; this_sentence_tokens=" << this_sentence_tokens
    //           << "; m_buffer_size=" << m_buffer_size << std::endl;
    if (m_current_timepoint + tokens_counter + this_sentence_tokens >= m_buffer_size)
    {
      throw std::runtime_error("No place for new tokens in this buffer");
    }

    while (!iter.end())
    {
      this_sentence_tokens++;
      // std::cerr << "DependencyParser::count_max_tokens_until_eos m_current_timepoint=" << m_current_timepoint
      //           << "; tokens_counter=" << tokens_counter << "; this_sentence_tokens=" << this_sentence_tokens
      //           << "; m_buffer_size=" << m_buffer_size
      //           << "; token=" << iter.form() << std::endl;

      if (iter.flags() & segmentation::token_pos::flag_t::sentence_brk ||
          iter.flags() & segmentation::token_pos::flag_t::paragraph_brk)
      {
        break;
        // lengths.push_back(this_sentence_tokens);
        // tokens_counter += this_sentence_tokens;
        // this_sentence_tokens = 1;
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
    if (this_sentence_tokens > 0)
    {
      lengths.push_back(this_sentence_tokens);
      tokens_counter += this_sentence_tokens;
    }

    iter.reset(current_iter_pos);
    // std::cerr << "DependencyParser::count_max_tokens_until_eos lengths=" << lengths
    //           << "; tokens_counter=" << tokens_counter << std::endl;
    return tokens_counter;
  }

  std::vector<std::string> m_class_names;
  std::vector<std::vector<std::string>> m_classes;

  size_t m_buffer_size;
  size_t m_current_buffer;
  size_t m_current_timepoint;
  bool m_started = false;

  std::shared_ptr</*const*/ StringIndex> m_stridx_ptr;
  // /*const*/ StringIndex& m_stridx;
  StringIndex::idx_t m_unk_idx;
  std::vector<tokens_with_analysis_t> m_buffers;
  std::vector<size_t> m_lengths;

#ifdef DP_VECTORIZER_WITH_PRECOMPUTING
  using Vectorizer = FeaturesVectorizerWithPrecomputing<
                                      tagging::impl::EntityTaggingClassifier,
                                      enriched_token_buffer_t,
                                      typename enriched_token_buffer_t::token_t>;
#elif DP_VECTORIZER_WITH_CACHE
  using Vectorizer = FeaturesVectorizerWithCache<
                                      enriched_token_buffer_t,
                                      typename enriched_token_buffer_t::token_t>;
#else
  using Vectorizer = deeplima::graph_dp::eigen_impl::FeaturesVectorizer<
                                      enriched_token_buffer_t,
                                      typename enriched_token_buffer_t::token_t>;
#endif
using Matrix = eigen_wrp::EigenMatrixXf;
class GraphDpImpl: public deeplima::graph_dp::impl::GraphDependencyParser
{
public:

  GraphDpImpl() :
      m_fastText(std::make_shared<FastTextVectorizer<eigen_wrp::EigenMatrixXf::matrix_t, Eigen::Index>>()),
      m_current_slot_timepoints(0),
      m_current_slot_no(-1),
      m_last_completed_slot(-1),
      m_curr_buff_idx(0)
  {}

  GraphDpImpl(
      size_t threads,
      size_t buffer_size_per_thread
    )
    : deeplima::graph_dp::impl::GraphDependencyParser(
        0 /* TODO: FIX ME */, 4, threads * 2, buffer_size_per_thread, threads),
      m_fastText(std::make_shared<FastTextVectorizer<eigen_wrp::EigenMatrixXf::matrix_t, Eigen::Index>>()),
      m_current_timepoint(deeplima::graph_dp::impl::GraphDependencyParser::get_start_timepoint())
  {
  }

  std::shared_ptr<EmbdUInt64Float> convert(const EmbdStrFloat& src)
  {
    auto d = src.get_int_dict<EmbdUInt64Float::value_t>();
    auto t = src.get_tensor().transpose();
    auto p = std::make_shared<EmbdUInt64Float>();
    p->init(d, t);

    return p;
  }

  virtual void load(const std::string& fn, const PathResolver& path_resolver)
  {
    deeplima::graph_dp::impl::GraphDependencyParser::load(fn);

    m_featVectorizers.resize(this->get_input_str_dicts().size());
    for (size_t i = 0; i < this->get_input_str_dicts().size(); ++i)
    {
      //auto d = (this->get_str_dicts()[i]);
      m_featVectorizers[i] = convert(this->get_input_str_dicts()[i]);
    }

    m_fastText->load(path_resolver.resolve("embd", deeplima::graph_dp::impl::GraphDependencyParser::get_embd_fn(0), {"bin", "ftz"}));
  }

  void init(size_t threads,
            size_t num_buffers,
            size_t buffer_size_per_thread,
            StringIndex& stridx,
            const std::vector<std::string>& class_names)
  {
    m_fastText->get_words([&stridx](const std::string& word){ stridx.get_idx(word); });

    std::vector<typename Vectorizer::feature_descr_t> feats;
    feats.reserve(1/* + m_featVectorizers.size()*/);
    feats.emplace_back(Vectorizer::str_feature, "form", m_fastText);
    for (size_t i = 0; i < class_names.size(); ++i)
    {
      if (class_names[i] != deeplima::graph_dp::impl::GraphDependencyParser::get_input_str_dicts_names()[i+1])
      {
        // TODO: skip morph classes that aren't requested by DP
        throw std::logic_error("Input classes missmatch: " + class_names[i] + " != " + deeplima::graph_dp::impl::GraphDependencyParser::get_input_str_dicts_names()[i+1]);
      }

      feats.emplace_back(Vectorizer::int_feature,
                         deeplima::graph_dp::impl::GraphDependencyParser::get_input_str_dicts_names()[i+1],
                         m_featVectorizers[i]);

      m_vectorizer.get_uint_feat_extractor().add_feature(class_names[i], i);
    }
    // for (const auto& class_name: class_names)
    // {
    //   int i = 0;
    //   for (const auto& input_str_dicts_names: deeplima::graph_dp::impl::GraphDependencyParser::get_input_str_dicts_names())
    //   {
    //     if (class_name == input_str_dicts_names)
    //     {
    //       feats.emplace_back(Vectorizer::int_feature,
    //                         class_name,
    //                         m_featVectorizers[i]);
    //
    //       m_vectorizer.get_uint_feat_extractor().add_feature(class_name, i);
    //       break;
    //     }
    //     i++;
    //   }
    // }
    m_vectorizer.init_features(feats);

    m_vectorizer.set_model(this);

    deeplima::graph_dp::impl::GraphDependencyParser::init(m_vectorizer.dim(),
                          0, num_buffers, buffer_size_per_thread, threads,
                          m_vectorizer.is_precomputing());

    m_current_timepoint = deeplima::graph_dp::impl::GraphDependencyParser::get_start_timepoint();
  }

  void precompute_inputs(const typename Vectorizer::dataset_t& buffer)
  {
    m_vectorizer.precompute(buffer);
  }

  typedef std::function < void (std::shared_ptr< StdMatrix<uint32_t> > classes,
                                size_t begin, size_t end, size_t slot_idx) > tagging_callback_t;

  virtual void register_handler(const tagging_callback_t fn)
  {
    m_callback = fn;
  }

  virtual ~GraphDpImpl()
  {
    // std::cerr << "~GraphDpImpl" << std::endl;
  }

protected:

  inline void increment_timepoint(uint64_t& timepoint)
  {
    assert(m_current_slot_timepoints > 0);
    deeplima::graph_dp::impl::GraphDependencyParser::increment_timepoint(timepoint);
    m_current_slot_timepoints--;
  }

  inline void send_results(int32_t slot_idx)
  {
    uint8_t lock_count = deeplima::graph_dp::impl::GraphDependencyParser::get_lock_count(slot_idx);
    // std::cerr << "GraphDpImpl::send_results " << slot_idx+1
    //       << " (lock_count=" << int(lock_count) << ")\n";
    uint64_t from = deeplima::graph_dp::impl::GraphDependencyParser::get_slot_begin(slot_idx);
    const uint64_t to = deeplima::graph_dp::impl::GraphDependencyParser::get_slot_end(slot_idx);
    // std::cerr << "GraphDpImpl::send_results " << slot_idx+1 << ", from=" << from << ", to=" << to << std::endl;

    m_callback(deeplima::graph_dp::impl::GraphDependencyParser::get_output(), from, to, slot_idx);

    deeplima::graph_dp::impl::GraphDependencyParser::decrement_lock_count(slot_idx);
    m_last_completed_slot = slot_idx;
  }

public:
  inline void send_next_results()
  {
    auto slot_idx = m_last_completed_slot;
    // std::cerr << "GraphDpImpl::send_next_results " << slot_idx+1 << std::endl;
    if (-1 == slot_idx)
    {
      slot_idx = 0;
    }
    else
    {
      slot_idx = deeplima::graph_dp::impl::GraphDependencyParser::next_slot(slot_idx);
    }
    // int a=0;
    uint8_t lock_count = deeplima::graph_dp::impl::GraphDependencyParser::get_lock_count(slot_idx);
    // std::cerr << "GraphDpImpl::send_next_results " << slot_idx+1
    //           << " (lock_count=" << int(lock_count) << ")" << std::endl;

    while (lock_count > 1)
    {
      // Worker still uses this slot. Waiting...
      // std::cerr << "GraphDpImpl::send_next_results: waiting for slot " << slot_idx+1
      //      << " (lock_count==" << int(lock_count) << ")\n";
      // deeplima::graph_dp::impl::GraphDependencyParser::pretty_print();
      deeplima::graph_dp::impl::GraphDependencyParser::wait_for_slot(slot_idx);
      lock_count = deeplima::graph_dp::impl::GraphDependencyParser::get_lock_count(slot_idx);
    }
    if (1 == lock_count)
    {
      // Data is ready. We can return it to caller
      send_results(slot_idx);
    }
  }

  inline void send_all_results()
  {
    auto slot_idx = m_last_completed_slot;
    // std::cerr << "GraphDpImpl::send_all_results" << slot_idx << std::endl;
    while (true)
    {
      if (-1 == slot_idx)
      {
        slot_idx = 0;
      }
      else
      {
        slot_idx = deeplima::graph_dp::impl::GraphDependencyParser::next_slot(slot_idx);
      }

      uint8_t lock_count = deeplima::graph_dp::impl::GraphDependencyParser::get_lock_count(slot_idx);
      if (0 == lock_count)
      {
        // std::cerr << "GraphDpImpl::send_all_results DONE" << std::endl;
        return;
      }

      while (lock_count > 1)
      {
        // Worker still uses this slot. Waiting...
        // std::cerr << "GraphDpImpl::send_all_results: waiting for slot " << slot_idx+1
        //      << " (lock_count==" << int(lock_count) << ")\n";
        // deeplima::graph_dp::impl::GraphDependencyParser::pretty_print();
        deeplima::graph_dp::impl::GraphDependencyParser::wait_for_slot(slot_idx);
        lock_count = deeplima::graph_dp::impl::GraphDependencyParser::get_lock_count(slot_idx);
      }
      if (1 == lock_count)
      {
        send_results(slot_idx);
      }
    }
  }

protected:
  inline void send_results_if_available()
  {
    auto slot_idx = m_last_completed_slot;
    if (-1 == slot_idx)
    {
      slot_idx = 0;
    }
    else
    {
      slot_idx = deeplima::graph_dp::impl::GraphDependencyParser::next_slot(slot_idx);
    }

    uint8_t lock_count = deeplima::graph_dp::impl::GraphDependencyParser::get_lock_count(slot_idx);

    if (1 == lock_count)
    {
      // Data is ready. We can return it to caller
      send_results(slot_idx);
    }
  }

  inline void acquire_slot(size_t slot_no)
  {
    // m_current_slot_no = deeplima::graph_dp::impl::GraphDependencyParser::get_slot_idx(m_current_timepoint);
    uint8_t lock_count = deeplima::graph_dp::impl::GraphDependencyParser::get_lock_count(slot_no);
    // std::cerr << "GraphDpImpl::acquiring_slot: " << (slot_no+1) << "; current lock count: " << int(lock_count) << std::endl;
    // deeplima::graph_dp::impl::GraphDependencyParser::pretty_print();

    while (lock_count > 1)
    {
      // Worker still uses this slot. Waiting...
      // std::cerr << "GraphDpImpl::acquiring_slot, waiting for slot " << (slot_no+1)
      //      << " lock_count=" << int(lock_count) << std::endl;
      // deeplima::graph_dp::impl::GraphDependencyParser::pretty_print();
      deeplima::graph_dp::impl::GraphDependencyParser::wait_for_slot(slot_no);
      lock_count = deeplima::graph_dp::impl::GraphDependencyParser::get_lock_count(slot_no);
    }
    if (1 == lock_count)
    {
      // Data is ready. We can return it to caller
      send_results(slot_no);
    }

    lock_count = deeplima::graph_dp::impl::GraphDependencyParser::get_lock_count(slot_no);
    // std::cerr << "GraphDpImpl::acquiring_slot: " << (slot_no+1) << "; before increment_lock_count: " << int(lock_count) << std::endl;
    // deeplima::graph_dp::impl::GraphDependencyParser::pretty_print();
    deeplima::graph_dp::impl::GraphDependencyParser::increment_lock_count(slot_no);
    lock_count = deeplima::graph_dp::impl::GraphDependencyParser::get_lock_count(slot_no);
    // std::cerr << "GraphDpImpl::acquiring_slot: " << (slot_no+1) << "; after increment_lock_count: " << int(lock_count) << std::endl;
    // deeplima::graph_dp::impl::GraphDependencyParser::pretty_print();
  }

public:
  virtual void handle_token_buffer(size_t slot_no,
                                   size_t first_timepoint_idx,
                                   const typename Vectorizer::dataset_t& buffer,
                                   const std::vector<size_t>& lengths,
                                   int timepoints_to_analyze = -1)
  {
    int lock_count = deeplima::graph_dp::impl::GraphDependencyParser::get_lock_count(slot_no);
    // std::cerr << "GraphDpImpl::handle_token_buffer " << (slot_no+1) << ", "
    //           << first_timepoint_idx
    //           << ", lengths=" << lengths
    //           << ", " << timepoints_to_analyze
    //           << "; lock_count=" << lock_count << std::endl;
    send_results_if_available();
    acquire_slot(slot_no);
    // size_t offset = slot_no * buffer.size() + deeplima::graph_dp::impl::GraphDependencyParser::get_start_timepoint();
    size_t count = (timepoints_to_analyze > 0) ? timepoints_to_analyze : buffer.size();
    for (size_t i = 0; i < count; i++)
    {
      m_vectorizer.vectorize_timepoint(eigen_wrp::EigenMatrixXf::get_tensor(), /*offset +*/ i, buffer[i]);
    }

    deeplima::graph_dp::impl::GraphDependencyParser::set_slot_lengths(slot_no, lengths);
    deeplima::graph_dp::impl::GraphDependencyParser::set_slot_begin(slot_no, first_timepoint_idx);
    deeplima::graph_dp::impl::GraphDependencyParser::set_slot_end(slot_no, /*offset +*/ count);

    auto& slot = deeplima::graph_dp::impl::GraphDependencyParser::m_slots[slot_no];

    lock_count = deeplima::graph_dp::impl::GraphDependencyParser::get_lock_count(slot_no);
    // std::cerr << "GraphDpImpl::handle_token_buffer slot " << (slot_no+1) << " retrieved: input="
    //           << slot.m_input_begin << ", " << slot.m_input_end << "; output="
    //           << slot.m_output_begin << ", " << slot.m_output_end << ", lock_count=" << lock_count
    //           << std::endl;

    deeplima::graph_dp::impl::GraphDependencyParser::start_job(slot_no, timepoints_to_analyze > 0);
    // std::cerr << "Slot " << slot_no << " sent to inference engine (graph_dp)" << std::endl;
  }

  inline void no_more_data(size_t slot_no)
  {
    if (!deeplima::graph_dp::impl::GraphDependencyParser::get_slot_started(slot_no))
    {
      while (deeplima::graph_dp::impl::GraphDependencyParser::get_lock_count(slot_no) > 1)
      {
        deeplima::graph_dp::impl::GraphDependencyParser::decrement_lock_count(slot_no);
      }
      deeplima::graph_dp::impl::GraphDependencyParser::start_job(slot_no, true);
    }
  }

protected:
  Vectorizer m_vectorizer;
  std::shared_ptr<FastTextVectorizer<eigen_wrp::EigenMatrixXf::matrix_t, Eigen::Index>> m_fastText;
  std::vector<std::shared_ptr<FeatureVectorizerBase<Eigen::Index>>> m_featVectorizers;

  tagging_callback_t m_callback;

  uint64_t m_current_timepoint;
  uint32_t m_current_slot_timepoints;

  int32_t m_current_slot_no;
  int32_t m_last_completed_slot;

  size_t m_curr_buff_idx;
};

  GraphDpImpl m_impl;

  output_callback_t m_output_callback;
};



} // deeplima

#endif // DEEPLIMA_DEPENDENCY_PARSER
