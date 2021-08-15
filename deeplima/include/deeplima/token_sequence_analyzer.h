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

#ifndef DEEPLIMA_TOKEN_SEQUENCE_ANALYZER
#define DEEPLIMA_TOKEN_SEQUENCE_ANALYZER

#include "segmentation.h"
#include "utils/str_index.h"
#include "token_type.h"
#include "ner.h"
#include "dumper_conllu.h"

namespace deeplima
{

template <class Matrix>
class TokenSequenceAnalyzer
{
  class TokenIterator
  {
    const StringIndex& m_stridx;
    const token_buffer_t& m_buffer;
    const StdMatrix<uint8_t>& m_classes;
    size_t m_current;
    size_t m_offset;
    size_t m_end;

  public:
    TokenIterator(const StringIndex& stridx, const token_buffer_t& buffer,
                  const StdMatrix<uint8_t>& classes, size_t offset, size_t end)
      : m_stridx(stridx), m_buffer(buffer), m_classes(classes), m_current(0), m_offset(offset), m_end(end - offset)
    {
      assert(end > offset + 1);
    }

    inline bool end() const
    {
      return m_current >= m_end;
    }

    impl::token_t::token_flags_t flags() const
    {
      assert(! end());
      return m_buffer[m_current].m_flags;
    }

    const char* form() const
    {
      assert(! end());
      const string& f = m_stridx.get_str(m_buffer[m_current].m_form_idx);
      return f.c_str();
    }

    void next()
    {
      m_current++;
    }

    const uint8_t token_class(size_t cls_idx) const
    {
      uint8_t val = m_classes.get(m_current + m_offset, cls_idx);
      return val;
    }
  };

  class enriched_token_t
  {
    friend class TokenSequenceAnalyzer;

  protected:
    const StringIndex& m_stridx;
    const token_buffer_t::token_t* m_ptoken;

    inline void set_token(const token_buffer_t::token_t* p)
    {
      m_ptoken = p;
    }
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

  typedef tagging::impl::FeaturesVectorizer<enriched_token_buffer_t, typename enriched_token_buffer_t::token_t> FeaturesVectorizer;

  typedef tagging::impl::TaggingImpl< tagging::impl::EntityTaggingClassifier,
                                      FeaturesVectorizer,
                                      Matrix > EntityTaggingModule;

public:
  TokenSequenceAnalyzer(const std::string& model_fn, size_t buffer_size, size_t num_buffers)
    : m_buffer_size(buffer_size),
      m_current_buffer(0),
      m_current_timepoint(0)
  {
    assert(m_buffer_size > 0);
    assert(num_buffers > 0);
    m_buffers.resize(num_buffers);
    for ( token_buffer_t& b : m_buffers ) b.resize(m_buffer_size);

    m_cls.load(model_fn);
    m_cls.init(1, num_buffers, buffer_size);

    for (size_t i = 0; i < m_cls.get_classes().size(); ++i)
    {
      m_dumper.set_classes(i, m_cls.get_class_names()[i], m_cls.get_classes()[i]);
    }

    m_cls.register_handler([this](
                           const typename EntityTaggingModule::OutputMatrix& classes,
                           size_t begin, size_t end, size_t slot_idx){
      std::cerr << "handler called: " << slot_idx << std::endl;
      TokenIterator ti(m_stridx, m_buffers[slot_idx], classes, /*slot_idx * m_buffer_size +*/ begin, end);
      m_dumper(ti);
      m_buffers[slot_idx].unlock();
    });
  }

  ~TokenSequenceAnalyzer()
  {
    std::cerr << "~TokenSequenceAnalyzer" << std::endl;
  }

  uint64_t get_token_counter() const
  {
    return m_dumper.get_token_counter();
  }

  void finalize()
  {
    if (m_current_timepoint > 0)
    {
      if (m_current_timepoint < m_buffer_size)
      {
        cerr << "Starting ..." << endl;
        start_analysis(m_current_buffer, m_current_timepoint);
      }
      else
      {
        m_cls.no_more_data(m_current_buffer);
      }
    }

    m_cls.send_all_results();
  }

  void stop()
  {
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
    const token_buffer_t& next_buffer = m_buffers[next_buffer_idx];

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
    assert(!m_buffers[buffer_idx].locked());
    m_buffers[buffer_idx].lock();

    const token_buffer_t& current_buffer = m_buffers[buffer_idx];
    m_cls.handle_token_buffer(buffer_idx, enriched_token_buffer_t(current_buffer, m_stridx), count);
  }

protected:

  inline bool wait_for_new_job(size_t& job)
  {
    std::unique_lock<std::mutex> l(m_mutex_new_job);
    m_cv_new_job.wait(l, [this](){ return !m_jobs.empty() || m_stop; });

    if (!m_jobs.empty())
    {
      job = m_jobs.front();
      m_jobs.pop();

      return true;
    }

    return false;
  }

  void process_buffer(size_t buffer_idx)
  {
    const token_buffer_t& current_buffer = m_buffers[buffer_idx];
    m_cls.handle_token_buffer(buffer_idx, enriched_token_buffer_t(current_buffer, m_stridx));
  }

  size_t m_buffer_size;
  size_t m_current_buffer;
  size_t m_current_timepoint;

  std::atomic<bool> m_stop;
  std::queue<size_t> m_jobs;

  std::thread m_worker_thread;

  std::mutex m_mutex_new_job;
  std::condition_variable m_cv_new_job;

  std::mutex m_mutex_notify;
  std::condition_variable m_cv_notify;

  StringIndex m_stridx;
  std::vector<token_buffer_t> m_buffers;

  EntityTaggingModule m_cls;

  // Dumper
  dumper::AnalysisToConllU<TokenIterator> m_dumper;
};

} // namespace deeplima

#endif
