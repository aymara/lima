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

#ifndef DEEPLIMA_SEGMENTATION_IMPL_H
#define DEEPLIMA_SEGMENTATION_IMPL_H

#include "utils/locked_buffer.h"

namespace deeplima
{
namespace segmentation
{

class ISegmentation
{
public:
  typedef std::function < bool (const uint8_t* buffer,
                                uint32_t& read,
                                uint32_t max) > read_callback_t;

  virtual void parse_from_stream(const read_callback_t fn) = 0;
  virtual void register_handler(const segmentation_callback_t fn) = 0;
  virtual ~ISegmentation() { }
};

namespace impl {

template <class InferenceEngine, class InputEncoder, class OutputDecoder>
class SegmentationImpl: public ISegmentation, public InferenceEngine
{
public:

  SegmentationImpl()
    : m_decoder(InferenceEngine::get_output(), m_char_len),
      m_current_slot_no(-1),
      m_current_slot_timepoints(0),
      m_last_completed_slot(-1),
      m_curr_buff_idx(0)
  {}

  SegmentationImpl(
      const std::vector<ngram_descr_t>& ngram_descr,
      size_t threads,
      size_t buffer_size_per_thread
    )
    : InferenceEngine(
        ngram_descr.size() * 2, 4, threads * 2, buffer_size_per_thread, threads),
      m_input_encoder(ngram_descr),
      m_decoder(InferenceEngine::get_output(), m_char_len),
      m_current_timepoint(InferenceEngine::get_start_timepoint()),
      m_buff_set(InferenceEngine::get_num_threads() * 2, InferenceEngine::get_slot_size() * 4)
  {
    m_char_len.resize(InferenceEngine::size());
  }

  virtual void load(const std::string& fn)
  {
    InferenceEngine::load(fn);
    m_input_encoder.init(InferenceEngine::get_ngram_descr());
  }

  void init(size_t threads, size_t buffer_size_per_thread)
  {
    InferenceEngine::set_dicts(InferenceEngine::get_dicts()); // This line needs refactoring.
    InferenceEngine::init(InferenceEngine::get_ngram_descr().size(),
                          4, threads * 2 + 2, buffer_size_per_thread, threads);
    m_char_len.resize(InferenceEngine::size());
    m_current_timepoint = InferenceEngine::get_start_timepoint();
    m_buff_set.init(InferenceEngine::get_num_threads() * 2 + 2,
                    InferenceEngine::get_num_threads() * InferenceEngine::get_slot_size() * 4);
  }

  virtual void parse_from_stream(const read_callback_t fn)
  {
    size_t n = 0;
    bool just_started = true;
    bool continue_reading = true;
    uint64_t counter = 0;

    do
    {
      locked_buffer_t& buff = m_buff_set.get(n);

      while (buff.locked())
      {
        send_next_results();
      }

      uint32_t bytes_read = 0;
      continue_reading = fn(buff.m_data, bytes_read, m_buff_set.max_buff_size());
      if (0 == bytes_read)
      {
        break;
      }
      counter += bytes_read;
      cerr << "Reading callback: " << bytes_read << " bytes, continue_reading="
           << continue_reading << " counter=" << counter
           << endl;
      buff.m_char_aligned_data = (const char*)(buff.m_data);
      buff.m_len = bytes_read;
      buff.lock();

      int32_t pos = 0;
      uint8_t* p = buff.m_data;
      if (!just_started && 0 == n)
      {
        memcpy(p - 8, m_buff_set.get(m_buff_set.size() - 1).m_data + m_buff_set.max_buff_size() - 8, 8);
      }

      // Warming up is required in the beginning of the text
      while (! m_input_encoder.ready_to_generate())
      {
        m_input_encoder.warmup(p, pos, bytes_read);
      }

      uint32_t lookbehind = 0;
      if (0 == pos)
      {
        // parse_start can handle bytes remaining from the previous buffer
        if ((lookbehind = m_input_encoder.parse_start(p, pos, bytes_read)) > 0)
        {
          assert(lookbehind <= 6);
          handle_timepoint();
        }
      }

      while (pos < bytes_read)
      {
        if (m_input_encoder.parse(p, pos, bytes_read) > 0)
        {
          handle_timepoint();
        }
      }

      n = m_buff_set.next(n);
      just_started = false;
    } while (continue_reading);

    char final_spaces[] = " ";
    for (size_t i = 0; i < m_input_encoder.get_lookahead(); i++)
    {
      int32_t pos = 0;
      if (m_input_encoder.parse((uint8_t*)final_spaces, pos, 1) > 0)
      {
        handle_timepoint();
      }
      else
      {
        throw std::runtime_error("Something wrong.");
      }
    }

    no_more_data();

    for (size_t i = 0; i < m_buff_set.size(); i++)
    {
      locked_buffer_t& buff = m_buff_set.get(n);

      while (buff.locked())
      {
        send_next_results();
      }

      m_buff_set.pretty_print();
      InferenceEngine::pretty_print();

      n = m_buff_set.next(n);
    }
  }

  virtual void register_handler(const segmentation_callback_t fn)
  {
    m_decoder.register_handler(fn);
  }

  virtual ~SegmentationImpl()
  {
    cerr << "~SimpleTextSegmentation" << endl;
  }

protected:

  inline void vectorize_timepoint(uint64_t timepoint)
  {
    for (size_t i = 0; i < m_input_encoder.size(); i++)
    {
      InferenceEngine::set(timepoint, i, m_input_encoder.get_feat(i));
    }

    m_char_len[m_current_timepoint] = m_input_encoder.get_len();
  }

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

    while (from < to)
    {
      const char* pch = m_buff_set.get(m_curr_buff_idx).m_char_aligned_data;
      uint32_t max = m_buff_set.bytes_available(m_curr_buff_idx);

      if (0 == max || nullptr == pch)
      {
        break;
      }

      from = m_decoder.decode(&pch, max, from, to);
      assert(nullptr != pch);

      for (size_t i = m_curr_buff_idx; i < m_buff_set.size(); i++)
      {
        uint8_t* buff_end = m_buff_set.get(i).end();
        if (nullptr == buff_end)
        {
          break;
        }
        if ((uint8_t*)pch >= buff_end)
        {
          m_buff_set.get(i).unlock();
          m_curr_buff_idx = m_buff_set.next(i);
        }
        else
        {
          m_buff_set.get(i).set_read_start(pch);
          m_curr_buff_idx = i;
          break;
        }
      }
    }

    assert(from == InferenceEngine::get_slot_end(slot_idx));

    InferenceEngine::decrement_lock_count(slot_idx);
    m_last_completed_slot = slot_idx;
  }

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
      cerr << "send_next_results: waiting for slot " << slot_idx
           << " (lock_count==" << int(lock_count) << ")\n";
      m_buff_set.pretty_print();
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

  inline void acquire_slot()
  {
    if (0 == m_current_slot_timepoints || m_current_slot_no < 0)
    {
      m_current_slot_no = InferenceEngine::get_slot_idx(m_current_timepoint);
      cerr << "acquire_slot: got " << m_current_slot_no << " for timepoint " << m_current_timepoint << endl;
      uint8_t lock_count = InferenceEngine::get_lock_count(m_current_slot_no);

      while (lock_count > 1)
      {
        // Worker still uses this slot. Waiting...
        cerr << "handle_timepoint, waiting for slot " << m_current_slot_no
             << " lock_count=" << lock_count << endl;
        InferenceEngine::wait_for_slot(m_current_slot_no);
        lock_count = InferenceEngine::get_lock_count(m_current_slot_no);
      }
      if (1 == lock_count)
      {
        // Data is ready. We can return it to caller
        send_results(m_current_slot_no);
      }

      InferenceEngine::increment_lock_count(m_current_slot_no);
      m_current_slot_timepoints = InferenceEngine::get_slot_size();
    }
  }

  inline void handle_timepoint()
  {
    acquire_slot();

    vectorize_timepoint(m_current_timepoint);
    m_char_len[m_current_timepoint] = m_input_encoder.get_len();

    increment_timepoint(m_current_timepoint);
    if (0 == m_current_slot_timepoints)
    {
      InferenceEngine::start_job(m_current_slot_no);
      cerr << "Slot " << m_current_slot_no << " sent to inference engine" << endl;
      acquire_slot();
    }
  }

  inline void no_more_data()
  {
    if (m_current_slot_timepoints < InferenceEngine::get_slot_size())
    {
      InferenceEngine::set_slot_end(m_current_slot_no, m_current_timepoint);
      InferenceEngine::start_job(m_current_slot_no, true);
    }
  }

protected:
  std::vector<uint8_t> m_char_len;

  InputEncoder m_input_encoder;
  OutputDecoder m_decoder;

  uint64_t m_current_timepoint;
  uint32_t m_current_slot_timepoints;

  int32_t m_current_slot_no;
  int32_t m_last_completed_slot;

  locked_buffer_set_t m_buff_set;
  size_t m_curr_buff_idx;
};

} // namespace impl
} // namespace segmentation
} // namespace deeplima

#endif
