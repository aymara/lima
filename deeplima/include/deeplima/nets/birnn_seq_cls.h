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

template <typename T>
std::ostream& operator<< (std::ostream& out, const std::vector<T>& v) {
  out << '[';
  if ( !v.empty() ) {
    std::copy (v.begin(), v.end(), std::ostream_iterator<T>(out, ", "));
  }
  out << "]";
  return out;
}

template <class Model, class InputVectorizer/*=TorchMatrix<int64_t>*/, class Out>
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
    uint64_t m_input_begin;
    uint64_t m_input_end;

    // output positions
    uint64_t m_output_begin;
    uint64_t m_output_end;

    slot_flags_t m_flags;

    bool m_work_started;
    std::atomic<uint8_t> m_lock_count;

    slot_t* m_prev;
    slot_t* m_next;

    std::vector<size_t> m_lengths;

    slot_t()
      : m_input_begin(0),
        m_input_end(0),
        m_output_begin(0),
        m_output_end(0),
        m_flags(none),
        m_work_started(false),
        m_lock_count(0),
        m_prev(nullptr),
        m_next(nullptr),
        m_lengths()
    { }
    slot_t(const slot_t& s)
      : m_input_begin(s.m_input_begin),
        m_input_end(s.m_input_end),
        m_output_begin(s.m_output_begin),
        m_output_end(s.m_output_end),
        m_flags(s.m_flags),
        m_work_started(s.m_work_started),
        m_lock_count(0),
        m_prev(nullptr),
        m_next(nullptr),
        m_lengths(s.m_lengths)
    { }
  };

protected:
  uint32_t m_overlap;
  uint32_t m_num_slots;
  uint32_t m_slot_len;

  std::vector<slot_t> m_slots;
  std::vector<std::vector<size_t>> m_lengths;
  std::shared_ptr< StdMatrix<Out> > m_output; // external - classifier id, internal - time position


  inline int32_t prev_slot(uint32_t idx)
  {
    assert(idx < m_num_slots);
    return (idx == 0) ? (m_num_slots - 1) : idx - 1;
  }

public:
  inline int32_t next_slot(uint32_t idx)
  {
    assert(idx < m_num_slots);
    return (idx == m_num_slots - 1) ? 0 : idx + 1;
  }

protected:
  inline void clear_slot(uint32_t idx)
  {
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

  inline void start_job_impl(uint32_t idx)
  {
    assert(idx < m_num_slots);
    slot_t& slot = m_slots[idx];
    // std::cerr << "RnnSequenceClassifier::start_job_impl slot id=" << (idx+1)
    //           << ", slot begin=" << slot.m_input_begin
    //           << ", slot end=" << slot.m_input_end
    //           << ", m_lengths=" << m_lengths
    //           << std::endl;

    if (! slot.m_work_started)
    {
      slot.m_work_started = true;
      ThreadPoolParent::push(&slot);
    }
  }

  inline static void run_one_job(ThisClass* this_ptr, size_t worker_id, void* p)
  {
    slot_t& slot = *((slot_t*)p);

    // std::cerr << "RnnSequenceClassifier::run_one_job worker " << worker_id
    //           << ". slot lock_count=" << int(slot.m_lock_count)
    //           << "; begin= " << slot.m_input_begin
    //           << "; end=" << slot.m_input_end
    //           << "; flags= " << int(slot.m_flags)
    //           << "; prev=" << (void*)slot.m_prev
    //           << "; next=" << (void*)slot.m_next
    //           << "; lengths=" << slot.m_lengths
    //           << ". lengths=" << this_ptr->m_lengths
    //           << std::endl;
    // this_ptr->pretty_print();

    this_ptr->predict(worker_id,
                      this_ptr->get_tensor(),
                      slot.m_input_begin, slot.m_input_end,
                      slot.m_output_begin, slot.m_output_end,
                      this_ptr->m_output,
                      slot.m_lengths,
                      // this_ptr->m_lengths[worker_id],
                      {"tokens"});

    // std::cerr << "RnnSequenceClassifier::run_one_job after predict worker " << worker_id
    //           << ". slot lock_count=" << int(slot.m_lock_count)
    //           << "; begin= " << slot.m_input_begin
    //           << "; end=" << slot.m_input_end
    //           << "; flags= " << int(slot.m_flags)
    //           << "; prev=" << (void*)slot.m_prev
    //           << "; next=" << (void*)slot.m_next
    //           // << "; output=" << (*(this_ptr->m_output))[0]
    //           << std::endl;
    // this_ptr->pretty_print();

    assert(slot.m_lock_count > 0);
    slot.m_lock_count--;
    if (slot.m_flags & left_overlap && slot.m_prev != nullptr)
    {
      assert(slot.m_prev->m_lock_count > 0);
      // slot.m_prev->m_lock_count--;
      slot.m_prev->m_lock_count--;
    }
    if (slot.m_flags & right_overlap && slot.m_next != nullptr)
    {
      assert(slot.m_next->m_lock_count > 0);
      slot.m_next->m_lock_count--;
    }
  }

public:

  std::shared_ptr< StdMatrix<Out> > get_output()
  {
    return m_output;
  }

  RnnSequenceClassifier()
    : m_overlap(0),
      m_num_slots(0),
      m_slot_len(0),
      m_slots(),
      m_lengths(),
      m_output(std::make_shared< StdMatrix<Out> >())
  {}

  RnnSequenceClassifier(uint32_t max_feat,
            uint32_t overlap,
            uint32_t num_slots,
            uint32_t slot_len,
            uint32_t num_threads)
    : m_overlap(0),
      m_num_slots(0),
      m_slot_len(0),
      m_slots(),
      m_lengths(),
      m_output(std::make_shared< StdMatrix<Out> >())
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
    // RnnSequenceClassifier::init 7, 4, 18, 16384, 8, false
    // RnnSequenceClassifier::init 1024, 16, 8, 1024, 1, true
    // RnnSequenceClassifier::init 464, 0, 8, 1024, 1, false

    // std::cerr << "RnnSequenceClassifier::init max_feat=" << max_feat << ", overlap=" << overlap
    //           << ", num_slots=" << num_slots
    //           << ", slot_len=" << slot_len
    //           << ", num_threads=" << num_threads
    //           << ", precomputed_input=" << precomputed_input << std::endl;
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

    m_slots.clear();
    m_slots.resize(m_num_slots);
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
      // std::cerr << "RnnSequenceClassifier::init slot " << (i+1)
      //           << " input begin=" << slot.m_input_begin << ", end=" << slot.m_input_end
      //           << " output begin=" << slot.m_output_begin << ", end=" << slot.m_output_end
      //           << std::endl;
    }

    m_lengths.resize(m_num_slots);

    // Vector for calculation results
    m_output->resize(Model::get_output_str_dicts_names().size());
    assert(m_output->size() > 0);
    for (auto& v : m_output->m_tensor)
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
    // std::cerr << "-> ~RnnSequenceClassifier" << std::endl;
    ThreadPoolParent::stop();
    // std::cerr << "<- ~RnnSequenceClassifier" << std::endl;
  }

  inline uint8_t get_output(uint64_t pos, uint8_t cls)
  {
    assert(cls < m_output->size());
    uint32_t idx = get_slot_idx(pos);
    assert(m_slots[idx].m_lock_count == 1);
    assert(m_slots[idx].m_work_started);
    return (*m_output)[cls][pos];
  }

  inline uint64_t get_slot_begin(uint32_t idx) const
  {
    assert(idx < m_num_slots);
    return m_slots[idx].m_output_begin;
  }

  inline bool get_slot_started(uint32_t idx) const
  {
    assert(idx < m_num_slots);
    return m_slots[idx].m_work_started;
  }

  inline uint64_t get_slot_end(uint32_t idx) const
  {
    assert(idx < m_num_slots);
    return m_slots[idx].m_output_end;
  }

  inline uint8_t get_lock_count(uint32_t idx) const
  {
    assert(idx < m_num_slots);
    return m_slots[idx].m_lock_count;
  }

  inline void increment_lock_count(uint32_t idx, uint8_t v = 1)
  {
    assert(idx < m_num_slots);
    m_slots[idx].m_lock_count += v;
    // std::cerr << "RnnSequenceClassifier::increment_lock_count by " << int(v) << " for slot " << int(idx+1)
    //           << ". it is now: " << int(m_slots[idx].m_lock_count) << std::endl;
    // pretty_print();
  }

  inline void decrement_lock_count(uint32_t idx)
  {
    assert(idx < m_num_slots);
    assert(m_slots[idx].m_lock_count > 0);

    m_slots[idx].m_lock_count--;
    if (0 == m_slots[idx].m_lock_count)
    {
      clear_slot(idx);
    }
    // std::cerr << "RnnSequenceClassifier::decrement_lock_count Lock for slot " << int(idx+1)
    //           << " set to " << int(m_slots[idx].m_lock_count) << std::endl;
    // pretty_print();
  }

  inline uint64_t get_start_timepoint() const
  {
    // std::cerr << "RnnSequenceClassifier::get_start_timepoint return " << m_overlap << std::endl;
    // pretty_print();
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

  inline void set_slot_lengths(uint32_t idx, const std::vector<size_t>& lengths)
  {
    // std::cerr << "RnnSequenceClassifier::set_slot_lengths slot id=" << (idx+1) << ", lengths=" << lengths << std::endl;
    assert(idx < m_num_slots);
    assert(idx < m_lengths.size());

    m_lengths[idx] = lengths;
    auto& slot = m_slots[idx];
    slot.m_lengths = lengths;
  }

  // used in graph-based dependency parser
  inline void set_slot_begin(uint32_t idx, uint64_t slot_begin)
  {
    // std::cerr << "RnnSequenceClassifier::set_slot_begin slot id=" << (idx+1) << ", slot_begin=" << slot_begin << std::endl;
    assert(idx < m_num_slots);

    slot_t& slot = m_slots[idx];

    slot.m_output_begin = slot_begin;
    slot.m_input_begin = slot_begin;
  }

  inline void set_slot_end(uint32_t idx, uint64_t slot_end)
  {
    // std::cerr << "RnnSequenceClassifier::set_slot_end slot id=" << (idx+1) << ", slot_end=" << slot_end << std::endl;
    assert(idx < m_num_slots);

    slot_t& slot = m_slots[idx];
    // std::cerr << "RnnSequenceClassifier::set_slot_end slot output begin=" << slot.m_output_begin
    //           << ", end=" << slot.m_output_end << std::endl;

    assert(slot_end > slot.m_output_begin);
    assert(slot_end <= slot.m_output_end);

    slot.m_output_end = slot_end;
    slot.m_input_end = slot_end;
  }

  inline void start_job(uint32_t idx, bool no_more_data=false)
  {
    assert(idx < m_num_slots);
    slot_t& slot = m_slots[idx];
    // std::cerr << "RnnSequenceClassifier::start_job " << int(idx+1) << ", " << no_more_data
    //           << "; lock count=" << int(slot.m_lock_count)
    //           << "; flags= " << int(slot.m_flags)
    //           << "; overlap= " << int(m_overlap)
    //           << std::endl;
    if (no_more_data)
    {
      // Must not take into account flags if overlap is zero (parsing)
      slot.m_flags = m_overlap == 0 ? none : (slot_flags_t)(slot.m_flags & (~right_overlap)) ;
      // std::cerr << "RnnSequenceClassifier::start_job after reverse" << int(idx+1) << ", " << no_more_data
      //           << "; lock count=" << int(slot.m_lock_count)
      //           << "; flags= " << int(slot.m_flags)
      //           << std::endl;
    }

    // increment_lock_count(idx);
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

  inline void wait_for_slot(uint32_t idx)
  {
    assert(idx < m_num_slots);
    const slot_t& slot = m_slots[idx];
    assert(slot.m_work_started);
    // std::cerr << "RnnSequenceClassifier::wait_for_slot " << (idx+1) << "/" << m_num_slots
    //           << "; lock count=" << int(slot.m_lock_count) << std::endl;
    // pretty_print();
    while (slot.m_lock_count > 1)
    {
      // std::cerr << "RnnSequenceClassifier::wait_for_slot in while lock_count=" << int(slot.m_lock_count) << std::endl;
      // pretty_print();
      ThreadPoolParent::wait_for_any_job_notification([&slot]() {
          return 1 == slot.m_lock_count;
        }
      );
    }
  }

  void pretty_print() const
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
