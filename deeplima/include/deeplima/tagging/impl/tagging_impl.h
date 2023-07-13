// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_TAGGING_IMPL_H
#define DEEPLIMA_TAGGING_IMPL_H

#include "deeplima/ner.h"
#include "deeplima/token_type.h"
#include "deeplima/fastText_wrp/fastText_wrp.h"
#include "deeplima/eigen_wrp/embd_dict.h"
#include "deeplima/utils/str_index.h"

#include "helpers/path_resolver.h"

namespace deeplima
{
namespace tagging
{

namespace impl {

class enriched_token_t
{
  friend class TokenSequenceAnalyzer;

protected:
  const StringIndex& m_stridx;
  const token_buffer_t<>::token_t* m_ptoken;

public:
  inline void set_token(const token_buffer_t<>::token_t* p)
  {
    m_ptoken = p;
  }

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

template <typename TaggingAuxScalar>
class TaggingImpl: public EntityTaggingClassifier<TaggingAuxScalar>
{
  using Classifier = EntityTaggingClassifier<TaggingAuxScalar> ;
  using Vectorizer = FeaturesVectorizerWithPrecomputing<
                                      Classifier,
                                      enriched_token_buffer_t,
                                      typename enriched_token_buffer_t::token_t> ;
public:

  TaggingImpl() :
      m_fastText(std::make_shared<FastTextVectorizer<typename eigen_wrp::EigenMatrixXf::matrix_t, Eigen::Index>>()),
      m_current_slot_timepoints(0),
      m_current_slot_no(-1),
      m_last_completed_slot(-1),
      m_curr_buff_idx(0)
  {}

  TaggingImpl(
      size_t threads,
      size_t buffer_size_per_thread
    )
    : Classifier(0 /* TODO: FIX ME */, 4, threads * 2, buffer_size_per_thread, threads),
      m_fastText(std::make_shared<FastTextVectorizer<typename eigen_wrp::EigenMatrixXf::matrix_t, Eigen::Index>>()),
      m_current_timepoint(Classifier::get_start_timepoint())
  {
  }

  virtual void load(const std::string& fn, const PathResolver& path_resolver)
  {
    Classifier::load(fn);

    if (this->get_input_str_dicts().size())
    {
      auto z = *(this->get_input_str_dicts().begin());
      auto p = std::make_shared<EmbdStrFloat>(z);
    }

    m_fastText->load(path_resolver.resolve("embd", Classifier::get_embd_fn(0), {"bin", "ftz"}));
  }

  void init(size_t threads, size_t num_buffers, size_t buffer_size_per_thread, StringIndex& stridx)
  {
    m_fastText->get_words([&stridx](const std::string& word){ stridx.get_idx(word); });

    m_vectorizer.init_features({
                                 { Vectorizer::str_feature, "form", m_fastText }
                               });

    m_vectorizer.set_model(this);

    Classifier::init(m_vectorizer.dim(),
                          16, num_buffers, buffer_size_per_thread, threads,
                          m_vectorizer.is_precomputing());

    m_current_timepoint = Classifier::get_start_timepoint();
  }

  void precompute_inputs(const typename Vectorizer::dataset_t& buffer)
  {
    m_vectorizer.precompute(buffer);
  }

  typedef std::function < void (std::shared_ptr< StdMatrix<uint8_t> > classes,
                                size_t begin, size_t end, size_t slot_idx) > tagging_callback_t;

  virtual void register_handler(const tagging_callback_t fn)
  {
    m_callback = fn;
  }

  virtual ~TaggingImpl()
  {
    // std::cerr << "~TaggingImpl" << std::endl;
  }

protected:

  inline void increment_timepoint(uint64_t& timepoint)
  {
    assert(m_current_slot_timepoints > 0);
    Classifier::increment_timepoint(timepoint);
    m_current_slot_timepoints--;
  }

  inline void send_results(int32_t slot_idx)
  {
    uint64_t from = Classifier::get_slot_begin(slot_idx);
    const uint64_t to = Classifier::get_slot_end(slot_idx);

    m_callback(Classifier::get_output(), from, to, slot_idx);

    Classifier::decrement_lock_count(slot_idx);
    m_last_completed_slot = slot_idx;
  }

public:
  inline void send_next_results()
  {
    int32_t slot_idx = m_last_completed_slot;
    if (-1 == slot_idx)
    {
      slot_idx = 0;
    }
    else
    {
      slot_idx = Classifier::next_slot(slot_idx);
    }

    uint8_t lock_count = Classifier::get_lock_count(slot_idx);

    while (lock_count > 1)
    {
      // Worker still uses this slot. Waiting...
      // std::cerr << "TaggingImpl::send_next_results: waiting for slot " << slot_idx+1
      //      << " (lock_count==" << int(lock_count) << ")\n";
      // Classifier::pretty_print();
      Classifier::wait_for_slot(slot_idx);
      lock_count = Classifier::get_lock_count(slot_idx);
    }
    if (1 == lock_count)
    {
      // Data is ready. We can return it to caller
      send_results(slot_idx);
    }
  }

  inline void send_all_results()
  {
    int32_t slot_idx = m_last_completed_slot;

    while (true)
    {
      if (-1 == slot_idx)
      {
        slot_idx = 0;
      }
      else
      {
        slot_idx = Classifier::next_slot(slot_idx);
      }

      uint8_t lock_count = Classifier::get_lock_count(slot_idx);
      if (0 == lock_count)
      {
        return;
      }

      while (lock_count > 1)
      {
        // Worker still uses this slot. Waiting...
        // std::cerr << "TaggingImpl::send_all_results: waiting for slot " << slot_idx+1
        //      << " (lock_count==" << int(lock_count) << ")\n";
        // Classifier::pretty_print();
        Classifier::wait_for_slot(slot_idx);
        lock_count = Classifier::get_lock_count(slot_idx);
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
      slot_idx = Classifier::next_slot(slot_idx);
    }

    uint8_t lock_count = Classifier::get_lock_count(slot_idx);

    if (1 == lock_count)
    {
      // Data is ready. We can return it to caller
      send_results(slot_idx);
    }
  }

  inline void acquire_slot(size_t slot_no)
  {
    // m_current_slot_no = Classifier::get_slot_idx(m_current_timepoint);
    // std::cerr << "tagging acquiring_slot: " << slot_no << std::endl;
    uint8_t lock_count = Classifier::get_lock_count(slot_no);

    while (lock_count > 1)
    {
      // Worker still uses this slot. Waiting...
      // std::cerr << "tagging handle_timepoint, waiting for slot " << slot_no
      //      << " lock_count=" << int(lock_count) << std::endl;
      // Classifier::pretty_print();
      Classifier::wait_for_slot(slot_no);
      lock_count = Classifier::get_lock_count(slot_no);
    }
    if (1 == lock_count)
    {
      // Data is ready. We can return it to caller
      send_results(slot_no);
    }

    Classifier::increment_lock_count(slot_no);
  }

public:
  virtual void handle_token_buffer(size_t slot_no, const typename Vectorizer::dataset_t& buffer, int timepoints_to_analyze = -1)
  {
    // std::cerr << "TaggingImpl::handle_token_buffer " << slot_no << ", " << timepoints_to_analyze << std::endl;
    send_results_if_available();
    acquire_slot(slot_no);
    size_t offset = slot_no * buffer.size() + Classifier::get_start_timepoint();
    size_t count = (timepoints_to_analyze > 0) ? timepoints_to_analyze : buffer.size();
    for (size_t i = 0; i < count; i++)
    {
      m_vectorizer.vectorize_timepoint(eigen_wrp::EigenMatrixXf::get_tensor(), offset + i, buffer[i]);
    }

    Classifier::set_slot_end(slot_no, offset + count);
    Classifier::start_job(slot_no, timepoints_to_analyze > 0);
    // std::cerr << "Slot " << slot_no << " sent to inference engine (tagging)" << std::endl;
  }

  inline void no_more_data(size_t slot_no)
  {
    if (!Classifier::get_slot_started(slot_no))
    {
      while (Classifier::get_lock_count(slot_no) > 1)
      {
        Classifier::decrement_lock_count(slot_no);
      }
      Classifier::start_job(slot_no, true);
    }
  }

protected:
  Vectorizer m_vectorizer;
  std::shared_ptr<FastTextVectorizer<typename eigen_wrp::EigenMatrixXf::matrix_t, Eigen::Index>> m_fastText;

  tagging_callback_t m_callback;

  uint64_t m_current_timepoint;
  uint32_t m_current_slot_timepoints;

  int32_t m_current_slot_no;
  int32_t m_last_completed_slot;

  size_t m_curr_buff_idx;
};

} // namespace impl
} // namespace tagging
} // namespace deeplima

#endif
