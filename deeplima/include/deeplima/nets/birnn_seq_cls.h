// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_SRC_INFERENCE_RNN_SEQ_CLS_H
#define DEEPLIMA_SRC_INFERENCE_RNN_SEQ_CLS_H

#include <vector>
#include <chrono>
#include <cstdlib>

#include "deeplima/utils/thread_pool.h"

#include "deeplima/utils/std_matrix.h"

namespace deeplima
{

template <class Model, class InputVectorizer/*=TorchMatrix<int64_t>*/, class Out=uint8_t>
class RnnSequenceClassifier : public InputVectorizer,
                              public ThreadPool< RnnSequenceClassifier<Model, InputVectorizer, Out> >,
                              public Model
{
  typedef RnnSequenceClassifier<Model, InputVectorizer, Out> ThisClass;
  typedef ThreadPool< RnnSequenceClassifier<Model, InputVectorizer, Out> > ThreadPoolParent;
  friend ThreadPoolParent;

  enum slot_flags_t : uint8_t
  {
    none = 0x00,
    left_overlap = 0x01,
    right_overlap = 0x02,
    max_flags
  };

  struct slot_t
  {
    // input positions
    int64_t m_input_begin;
    int64_t m_input_end;

    // output positions
    int64_t m_output_begin;
    int64_t m_output_end;

    slot_flags_t m_flags;

    bool m_work_started;
    std::atomic<uint8_t> m_lock_count;

    slot_t* m_prev;
    slot_t* m_next;

    slot_t()
      : m_input_begin(0),
        m_input_end(0),
        m_output_begin(0),
        m_output_end(0),
        m_work_started(false),
        m_flags(none),
        m_lock_count(0),
        m_prev(nullptr),
        m_next(nullptr)
    { }
  };

protected:
  uint32_t m_overlap;
  uint32_t m_num_slots;
  uint32_t m_slot_len;

  slot_t* m_slots;
  std::vector<std::vector<size_t>> m_lengths;
  std::vector<std::vector<Out>> m_output; // external - classifier id, internal - time position


  inline int32_t prev_slot(int32_t idx)
  {
    assert(idx >= 0);
    assert(idx < m_num_slots);
    return (idx == 0) ? (m_num_slots - 1) : idx - 1;
  }

public:
  inline int32_t next_slot(int32_t idx)
  {
    assert(idx >= 0);
    assert(idx < m_num_slots);
    return (idx == m_num_slots - 1) ? 0 : idx + 1;
  }

protected:
  inline void clear_slot(int32_t idx)
  {
    assert(idx >= 0);
    assert(idx < m_num_slots);
    slot_t& slot = m_slots[idx];

    slot.m_work_started = false;
    //slot.m_done = false;
    // workaround after first use of the first slot
    if (0 == idx /*&& 0 == (slot.m_flags & left_overlap)*/)
    {
      assert(slot.m_output_begin >= slot.m_input_begin);
      //slot.m_flags = slot_flags_t(left_overlap | right_overlap);
      slot.m_input_begin = slot.m_output_begin - m_overlap;
    }
  }

  inline void start_job_impl(int32_t idx)
  {
    assert(idx >= 0);
    assert(idx < m_num_slots);
    slot_t& slot = m_slots[idx];

    if (! slot.m_work_started)
    {
      slot.m_work_started = true;
      ThreadPoolParent::push(&slot);
    }
  }

  inline static void run_one_job(ThisClass* this_ptr, size_t worker_id, void* p)
  {
    slot_t& slot = *((slot_t*)p);

    //std::cerr << "Starting work on slot with lock_count==" << slot.m_lock_count << std::endl;

    this_ptr->predict(worker_id,
                      this_ptr->get_tensor(),
                      slot.m_input_begin, slot.m_input_end,
                      slot.m_output_begin, slot.m_output_end,
                      this_ptr->m_output,
                      this_ptr->m_lengths[worker_id],
                      {"tokens"});

    assert(slot.m_lock_count > 0);
    slot.m_lock_count--;
    if (slot.m_flags & left_overlap)
    {
      assert(slot.m_prev->m_lock_count > 0);
      slot.m_prev->m_lock_count--;
    }
    if (slot.m_flags & right_overlap)
    {
      assert(slot.m_next->m_lock_count > 0);
      slot.m_next->m_lock_count--;
    }
  }

public:

  typedef StdMatrix<Out> OutputMatrix;

  const OutputMatrix get_output() const
  {
    return OutputMatrix(m_output);
  }

  RnnSequenceClassifier()
    : m_overlap(0),
      m_num_slots(0),
      m_slot_len(0),
      m_slots(nullptr)
  {}

  RnnSequenceClassifier(uint32_t max_feat,
            uint32_t overlap,
            uint32_t num_slots,
            uint32_t slot_len,
            uint32_t num_threads)
    : m_overlap(0),
      m_num_slots(0),
      m_slot_len(0),
      m_slots(nullptr)
  {
    init(max_feat, overlap, num_slots, slot_len, num_threads);
  }

  void init(uint32_t max_feat,
            uint32_t overlap,
            uint32_t num_slots,
            uint32_t slot_len,
            uint32_t num_threads,
            bool precomputed_input=false)
  {
    m_num_slots = num_slots;
    m_overlap = overlap;
    m_slot_len = slot_len;

    InputVectorizer::init(/*Model::get_dicts(),*/ m_num_slots * m_slot_len + m_overlap * 2, max_feat);
    //InputVectorizer::set_dicts(Model::get_dicts());
    for (size_t i = 0; i < num_threads; i++)
    {
      Model::init_new_worker(m_slot_len + m_overlap * 2, precomputed_input); // skip id - all workers are identical
    }
    ThreadPoolParent::init(num_threads);

    m_slots = new slot_t[m_num_slots];
    for (size_t i = 0; i < m_num_slots; i++)
    {
      slot_t& slot = m_slots[i];

      slot.m_output_begin = m_overlap + i * m_slot_len;
      slot.m_output_end = slot.m_output_begin + m_slot_len;
      slot.m_input_begin = slot.m_output_begin - m_overlap;
      slot.m_input_end = slot.m_output_end + m_overlap;

      if (m_overlap > 0)
      {
        slot.m_flags = (slot_flags_t)(left_overlap | right_overlap);

        uint32_t prev_idx = prev_slot(i);
        slot.m_prev = &(m_slots[prev_idx]);

        uint32_t next_idx = next_slot(i);
        slot.m_next = &(m_slots[next_idx]);
      }

      if (0 == i)
      {
        slot.m_input_begin = slot.m_output_begin;
        slot.m_flags = right_overlap;
      }

      if (m_num_slots - 1 == i)
      {
        slot.m_flags = left_overlap;
      }
    }

    m_lengths.resize(m_num_slots);

    // Vector for calculation results
    m_output.resize(Model::get_output_str_dicts_names().size());
    assert(m_output.size() > 0);
    for (auto& v : m_output)
    {
      v.resize(InputVectorizer::size());
      assert(v.size() > 0);
    }
  }

  void load(const std::string& fn)
  {
    Model::load(fn);
    //InputVectorizer::set_dicts(Model::get_dicts());
  }

  void get_classes_from_fn(const std::string& fn, std::vector<std::string>& classes_names, std::vector<std::vector<std::string>>& classes){
      Model::get_classes_from_fn(fn, classes_names, classes);
  }

  virtual ~RnnSequenceClassifier()
  {
    std::cerr << "-> ~RnnSequenceClassifier" << std::endl;
    ThreadPoolParent::stop();
    if (nullptr != m_slots)
    {
      delete[] m_slots;
    }
    std::cerr << "<- ~RnnSequenceClassifier" << std::endl;
  }

  inline uint8_t get_output(uint64_t pos, uint8_t cls)
  {
    assert(cls < m_output.size());
    uint32_t idx = get_slot_idx(pos);
    assert(m_slots[idx].m_lock_count == 1);
    assert(m_slots[idx].m_work_started);
    return m_output[cls][pos];
  }

  inline uint64_t get_slot_begin(int32_t idx) const
  {
    assert(idx >= 0);
    assert(idx < m_num_slots);
    return m_slots[idx].m_output_begin;
  }

  inline bool get_slot_started(int32_t idx) const
  {
    assert(idx >= 0);
    assert(idx < m_num_slots);
    return m_slots[idx].m_work_started;
  }

  inline uint64_t get_slot_end(int32_t idx) const
  {
    assert(idx >= 0);
    assert(idx < m_num_slots);
    return m_slots[idx].m_output_end;
  }

  inline uint8_t get_lock_count(int32_t idx) const
  {
    assert(idx >= 0);
    assert(idx < m_num_slots);
    return m_slots[idx].m_lock_count;
  }

  inline void increment_lock_count(int32_t idx, uint8_t v = 1)
  {
    assert(idx >= 0);
    assert(idx < m_num_slots);
    m_slots[idx].m_lock_count += v;
    std::cerr << "Lock for slot " << idx
              << " set to " << int(m_slots[idx].m_lock_count) << std::endl;
  }

  inline void decrement_lock_count(int32_t idx)
  {
    assert(idx >= 0);
    assert(idx < m_num_slots);
    assert(m_slots[idx].m_lock_count > 0);

    m_slots[idx].m_lock_count--;
    if (0 == m_slots[idx].m_lock_count)
    {
      clear_slot(idx);
    }
  }

  inline uint64_t get_start_timepoint() const
  {
    return m_overlap;
  }

  inline void increment_timepoint(uint64_t& timepoint)
  {
    timepoint++;
    if (timepoint >= InputVectorizer::size() - m_overlap)
    {
      timepoint = get_start_timepoint();
    }
  }

  inline uint32_t get_num_slots() const
  {
    return m_num_slots;
  }

  inline uint32_t get_slot_size() const
  {
    return m_slot_len;
  }

  inline int32_t get_slot_idx(uint64_t timepoint) const
  {
    assert(timepoint < m_num_slots * m_slot_len);
    uint32_t rv = timepoint / m_slot_len;
    assert(rv < m_num_slots);
    return rv;
  }

  inline void set_slot_lengths(int32_t idx, const std::vector<size_t>& lengths)
  {
    assert(idx >= 0);
    assert(idx < m_num_slots);
    assert(idx < m_lengths.size());

    m_lengths[idx] = lengths;
  }

  // used in graph-based dependency parser
  inline void set_slot_begin(int32_t idx, uint64_t slot_begin)
  {
    assert(idx >= 0);
    assert(idx < m_num_slots);

    slot_t& slot = m_slots[idx];

    slot.m_output_begin = slot_begin;
    slot.m_input_begin = slot_begin;
  }

  inline void set_slot_end(int32_t idx, uint64_t slot_end)
  {
    assert(idx >= 0);
    assert(idx < m_num_slots);

    slot_t& slot = m_slots[idx];

    assert(slot_end > slot.m_output_begin);
    assert(slot_end <= slot.m_output_end);

    slot.m_output_end = slot_end;
    slot.m_input_end = slot_end;
  }

  inline void start_job(int32_t idx, bool no_more_data=false)
  {
    assert(idx >= 0);
    assert(idx < m_num_slots);
    slot_t& slot = m_slots[idx];

    if (no_more_data)
    {
      slot.m_flags = (slot_flags_t)(slot.m_flags & (~right_overlap));
    }

    increment_lock_count(idx, 1
                         + ((slot.m_flags & left_overlap) > 0 ? 1 : 0)
                         + ((slot.m_flags & right_overlap) > 0 ? 1 : 0));

    if (slot.m_flags & left_overlap)
    {
      uint32_t prev_idx = prev_slot(idx);
      start_job_impl(prev_idx);
    }

    if (0 == m_overlap || (0 == (slot.m_flags & right_overlap)))
    {
      start_job_impl(idx);
    }
  }

  inline void wait_for_slot(int32_t idx)
  {
    assert(idx >= 0);
    assert(idx < m_num_slots);
    const slot_t& slot = m_slots[idx];
    assert(slot.m_work_started);
    while (slot.m_lock_count > 1)
    {
      ThreadPoolParent::wait_for_any_job_notification([&slot](){
        return 1 == slot.m_lock_count;
      });
    }
  }

  void pretty_print()
  {
    std::cerr << "SLOTS: ";
    for (size_t i = 0; i < m_num_slots; i++)
    {
      std::cerr << " | " << int(m_slots[i].m_lock_count);
    }
    std::cerr << " |" << std::endl;
  }
};

} // namespace deeplima

#endif
