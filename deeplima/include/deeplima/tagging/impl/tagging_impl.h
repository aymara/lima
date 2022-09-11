// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_TAGGING_IMPL_H
#define DEEPLIMA_TAGGING_IMPL_H

#include "deeplima/token_type.h"
#include "deeplima/fastText_wrp/fastText_wrp.h"
#include "deeplima/eigen_wrp/embd_dict.h"
#include "deeplima/utils/str_index.h"

#include "helpers/path_resolver.h"

namespace deeplima
{
namespace tagging
{

class ITagging
{
public:
};

namespace impl {

template <class InferenceEngine, class Vectorizer, class Matrix>
class TaggingImpl: public ITagging, public InferenceEngine
{
public:

  TaggingImpl()
    : m_current_slot_no(-1),
      m_current_slot_timepoints(0),
      m_last_completed_slot(-1),
      m_curr_buff_idx(0)
  {}

  TaggingImpl(
      size_t threads,
      size_t buffer_size_per_thread
    )
    : InferenceEngine(
        0 /* TODO: FIX ME */, 4, threads * 2, buffer_size_per_thread, threads),
      m_current_timepoint(InferenceEngine::get_start_timepoint())
  {
  }

  virtual void load(const std::string& fn, const PathResolver& path_resolver)
  {
    InferenceEngine::load(fn);

    if (this->get_input_str_dicts().size())
    {
      auto z = *(this->get_input_str_dicts().begin());
      std::shared_ptr<FeatureVectorizerBase<Eigen::Index>> p
          = std::shared_ptr<EmbdStrFloat>( new EmbdStrFloat(z) );
    }

    m_fastText.load(path_resolver.resolve("embd", InferenceEngine::get_embd_fn(0), {"bin", "ftz"}));
  }

  void init(size_t threads, size_t num_buffers, size_t buffer_size_per_thread, StringIndex& stridx)
  {
    m_fastText.get_words([&stridx](const std::string& word){ stridx.get_idx(word); });

    m_vectorizer.init_features({
                                 { Vectorizer::str_feature, "form", &m_fastText }
                               });

    m_vectorizer.set_model(this);

    InferenceEngine::init(m_vectorizer.dim(),
                          16, num_buffers, buffer_size_per_thread, threads,
                          m_vectorizer.is_precomputing());

    m_current_timepoint = InferenceEngine::get_start_timepoint();
  }

  void precompute_inputs(const typename Vectorizer::dataset_t& buffer)
  {
    m_vectorizer.precompute(buffer);
  }

  typedef typename InferenceEngine::OutputMatrix OutputMatrix;
  typedef std::function < void (const OutputMatrix& classes,
                                size_t begin, size_t end, size_t slot_idx) > tagging_callback_t;

  virtual void register_handler(const tagging_callback_t fn)
  {
    m_callback = fn;
  }

  virtual ~TaggingImpl()
  {
    std::cerr << "~TaggingImpl" << std::endl;
  }

protected:

  inline void increment_timepoint(uint64_t& timepoint)
  {
    assert(m_current_slot_timepoints > 0);
    InferenceEngine::increment_timepoint(timepoint);
    m_current_slot_timepoints--;
  }

  inline void send_results(int32_t slot_idx)
  {
    uint64_t from = InferenceEngine::get_slot_begin(slot_idx);
    const uint64_t to = InferenceEngine::get_slot_end(slot_idx);

    m_callback(InferenceEngine::get_output(), from, to, slot_idx);

    InferenceEngine::decrement_lock_count(slot_idx);
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
      slot_idx = InferenceEngine::next_slot(slot_idx);
    }

    uint8_t lock_count = InferenceEngine::get_lock_count(slot_idx);

    while (lock_count > 1)
    {
      // Worker still uses this slot. Waiting...
      std::cerr << "send_next_results: waiting for slot " << slot_idx
           << " (lock_count==" << int(lock_count) << ")\n";
      InferenceEngine::pretty_print();
      InferenceEngine::wait_for_slot(slot_idx);
      lock_count = InferenceEngine::get_lock_count(slot_idx);
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
        slot_idx = InferenceEngine::next_slot(slot_idx);
      }

      uint8_t lock_count = InferenceEngine::get_lock_count(slot_idx);
      if (0 == lock_count)
      {
        return;
      }

      while (lock_count > 1)
      {
        // Worker still uses this slot. Waiting...
        std::cerr << "send_next_results: waiting for slot " << slot_idx
             << " (lock_count==" << int(lock_count) << ")\n";
        InferenceEngine::pretty_print();
        InferenceEngine::wait_for_slot(slot_idx);
        lock_count = InferenceEngine::get_lock_count(slot_idx);
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
    int32_t slot_idx = m_last_completed_slot;
    if (-1 == slot_idx)
    {
      slot_idx = 0;
    }
    else
    {
      slot_idx = InferenceEngine::next_slot(slot_idx);
    }

    uint8_t lock_count = InferenceEngine::get_lock_count(slot_idx);

    if (1 == lock_count)
    {
      // Data is ready. We can return it to caller
      send_results(slot_idx);
    }
  }

  inline void acquire_slot(size_t slot_no)
  {
    // m_current_slot_no = InferenceEngine::get_slot_idx(m_current_timepoint);
    std::cerr << "tagging acquiring_slot: " << slot_no << std::endl;
    uint8_t lock_count = InferenceEngine::get_lock_count(slot_no);

    while (lock_count > 1)
    {
      // Worker still uses this slot. Waiting...
      std::cerr << "tagging handle_timepoint, waiting for slot " << slot_no
           << " lock_count=" << int(lock_count) << std::endl;
      InferenceEngine::pretty_print();
      InferenceEngine::wait_for_slot(slot_no);
      lock_count = InferenceEngine::get_lock_count(slot_no);
    }
    if (1 == lock_count)
    {
      // Data is ready. We can return it to caller
      send_results(slot_no);
    }

    InferenceEngine::increment_lock_count(slot_no);
  }

public:
  virtual void handle_token_buffer(size_t slot_no, const typename Vectorizer::dataset_t& buffer, int timepoints_to_analyze = -1)
  {
    send_results_if_available();
    acquire_slot(slot_no);
    size_t offset = slot_no * buffer.size() + InferenceEngine::get_start_timepoint();
    size_t count = (timepoints_to_analyze > 0) ? timepoints_to_analyze : buffer.size();
    for (size_t i = 0; i < count; i++)
    {
      m_vectorizer.vectorize_timepoint(Matrix::get_tensor(), offset + i, buffer[i]);
    }

    InferenceEngine::set_slot_end(slot_no, offset + count);
    InferenceEngine::start_job(slot_no, timepoints_to_analyze > 0);
    std::cerr << "Slot " << slot_no << " sent to inference engine (tagging)" << std::endl;
  }

  inline void no_more_data(size_t slot_no)
  {
    if (!InferenceEngine::get_slot_started(slot_no))
    {
      while (InferenceEngine::get_lock_count(slot_no) > 1)
      {
        InferenceEngine::decrement_lock_count(slot_no);
      }
      InferenceEngine::start_job(slot_no, true);
    }
  }

protected:
  Vectorizer m_vectorizer;
  FastTextVectorizer<typename Matrix::matrix_t, Eigen::Index> m_fastText;

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
