// Copyright 2002-2023 CEA LIST
// SPDX-FileCopyrightText: 2023 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "deeplima/segmentation/impl/segmentation_impl.h"

namespace deeplima::segmentation::impl {

SegmentationImpl::SegmentationImpl()
  : m_decoder(SegmentationClassifier::get_output(), m_char_len),
    m_current_slot_timepoints(0),
    m_current_slot_no(-1),
    m_last_completed_slot(-1),
    m_buff_set(SegmentationClassifier::get_num_threads() * 2, SegmentationClassifier::get_slot_size() * 4),
    m_curr_buff_idx(0)
{}

SegmentationImpl::SegmentationImpl(
    const std::vector<ngram_descr_t>& ngram_descr,
    size_t threads,
    size_t buffer_size_per_thread
  )
  : SegmentationClassifier(
      ngram_descr.size() * 2, 4, threads * 2, buffer_size_per_thread, threads),
    m_input_encoder(ngram_descr),
    m_decoder(SegmentationClassifier::get_output(), m_char_len),
    m_current_timepoint(SegmentationClassifier::get_start_timepoint()),
    m_buff_set(SegmentationClassifier::get_num_threads() * 2, SegmentationClassifier::get_slot_size() * 4)
{
  m_char_len.resize(SegmentationClassifier::size());
}

void SegmentationImpl::load(const std::string& fn)
{
  try
  {
    SegmentationClassifier::load(fn);
  }
  catch (std::runtime_error& e)
  {
    std::cerr << "SegmentationImpl exception while trying to load model  " << fn << ":" << std::endl
              << e.what();
    throw;
  }
  m_input_encoder.init(SegmentationClassifier::get_ngram_descr());
}

void SegmentationImpl::init(size_t threads, size_t buffer_size_per_thread)
{
  SegmentationClassifier::set_dicts(SegmentationClassifier::get_dicts()); // This line needs refactoring.
  SegmentationClassifier::init(SegmentationClassifier::get_ngram_descr().size(),
                        4, threads * 2 + 2, buffer_size_per_thread, threads);
  m_char_len.resize(SegmentationClassifier::size());
  m_current_timepoint = SegmentationClassifier::get_start_timepoint();
  m_buff_set.init(SegmentationClassifier::get_num_threads() * 2 + 2,
                  SegmentationClassifier::get_num_threads() * SegmentationClassifier::get_slot_size() * 4);
}

void SegmentationImpl::parse_from_stream(const read_callback_t fn)
{
  std::cerr << "SegmentationImpl::parse_from_stream" << std::endl;
  size_t n = 0;
  bool just_started = true;
  bool continue_reading = true;
  uint64_t counter = 0;
  reset(); // reset internal slots for (re)use
  m_current_timepoint = SegmentationClassifier::get_start_timepoint();
  m_buff_set.init(SegmentationClassifier::get_num_threads() * 2 + 2,
                  SegmentationClassifier::get_num_threads() * SegmentationClassifier::get_slot_size() * 4);

  do
  {
    locked_buffer_t& buff = m_buff_set.get(n);

    while (buff.locked())
    {
      send_next_results();
    }

    int32_t bytes_read = 0;
    continue_reading = fn(buff.m_data, bytes_read, m_buff_set.max_buff_size());
    if (0 == bytes_read)
    {
      break;
    }
    counter += bytes_read;
    std::cerr << "SegmentationImpl::parse_from_stream Reading callback: "
              << bytes_read << " bytes, continue_reading="
              << continue_reading << " counter=" << counter << std::endl;
    buff.m_char_aligned_data = (const char*)(buff.m_data);
    buff.m_len = bytes_read;
    std::cerr << "SegmentationImpl::parse_from_stream locking (m_buff_set) buff "
              << n << std::endl;
    buff.lock();

    int32_t pos = 0;
    uint8_t* p = buff.m_data;
    if (!just_started && 0 == n)
    {
      memcpy(p - 8,
             m_buff_set.get(m_buff_set.size() - 1).m_data
                + m_buff_set.max_buff_size() - 8,
             8);
    }

    // Warming up is required in the beginning of the text
    while (! m_input_encoder.ready_to_generate())
    {
      m_input_encoder.warmup(p, &pos, bytes_read);
    }

    uint32_t lookbehind = 0;
    if (0 == pos)
    {
      // parse_start can handle bytes remaining from the previous buffer
      if ((lookbehind = m_input_encoder.parse_start(p, &pos, bytes_read)) > 0)
      {
        assert(lookbehind <= 6);
        handle_timepoint();
      }
    }

    while (pos < bytes_read)
    {
      if (m_input_encoder.parse(p, &pos, bytes_read) > 0)
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
    if (m_input_encoder.parse((uint8_t*)final_spaces, &pos, 1) > 0)
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
    SegmentationClassifier::pretty_print();

    n = m_buff_set.next(n);
  }
}

void SegmentationImpl::register_handler(const segmentation_callback_t fn)
{
  m_decoder.register_handler(fn);
}

void SegmentationImpl::vectorize_timepoint(uint64_t timepoint)
{
  for (size_t i = 0; i < m_input_encoder.size(); i++)
  {
    SegmentationClassifier::set(timepoint, i, m_input_encoder.get_feat(i));
  }

  m_char_len[m_current_timepoint] = m_input_encoder.get_len();
}

void SegmentationImpl::increment_timepoint(uint64_t& timepoint)
{
  assert(m_current_slot_timepoints > 0);
  SegmentationClassifier::increment_timepoint(timepoint);
  m_current_slot_timepoints--;
}

void SegmentationImpl::send_results(int32_t slot_idx)
{
  uint64_t from = SegmentationClassifier::get_slot_begin(slot_idx);
  const uint64_t to = SegmentationClassifier::get_slot_end(slot_idx);

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

  assert(from == SegmentationClassifier::get_slot_end(slot_idx));

  SegmentationClassifier::decrement_lock_count(slot_idx);
  m_last_completed_slot = slot_idx;
}

void SegmentationImpl::send_next_results()
{
  int32_t slot_idx = m_last_completed_slot;
  if (-1 == slot_idx)
  {
    slot_idx = 0;
  }
  else
  {
    slot_idx = SegmentationClassifier::next_slot(slot_idx);
  }

  // We are in send_next_results
  // Note, use get_lock_count from
  // using SegmentationClassifier = RnnSequenceClassifier<eigen_impl::Model, eigen_impl::EmbdVectorizer, uint8_t> ;
  // This one accesses its m_slots[idx].m_lock_count (std::vector<slot_t> member of RnnSequenceClassifier)
  // while send_results (above but called below) do m_buff_set.get(i).unlock() (a
  // locked_buffer_set_t), a member of SegmentationImpl
  // Should we use SegmentationClassifier::decrement_lock_count in send_results too?

  uint8_t lock_count = SegmentationClassifier::get_lock_count(slot_idx);

  while (lock_count > 1)
  {
    // Worker still uses this slot. Waiting...
    // std::cerr << "SegmentationImpl::send_next_results: waiting for slot " << slot_idx+1
    //      << " (lock_count==" << int(lock_count) << ")\n";
    // m_buff_set.pretty_print();
    // SegmentationClassifier::pretty_print();
    SegmentationClassifier::wait_for_slot(slot_idx);
    lock_count = SegmentationClassifier::get_lock_count(slot_idx);
  }
  if (1 == lock_count)
  {
    // Data is ready. We can return it to caller
    send_results(slot_idx);
  }
}

void SegmentationImpl::acquire_slot()
{
  if (0 == m_current_slot_timepoints || m_current_slot_no < 0)
  {
    m_current_slot_no = SegmentationClassifier::get_slot_idx(m_current_timepoint);
    // std::cerr << "SegmentationImpl::acquire_slot: got " << m_current_slot_no << " for timepoint "
    //           << m_current_timepoint << std::endl;
    uint8_t lock_count = SegmentationClassifier::get_lock_count(m_current_slot_no);

    while (lock_count > 1)
    {
      // Worker still uses this slot. Waiting...
      // std::cerr << "handle_timepoint, waiting for slot " << m_current_slot_no
      //      << " lock_count=" << lock_count << std::endl;
      SegmentationClassifier::wait_for_slot(m_current_slot_no);
      lock_count = SegmentationClassifier::get_lock_count(m_current_slot_no);
    }
    if (1 == lock_count)
    {
      // Data is ready. We can return it to caller
      send_results(m_current_slot_no);
    }

    SegmentationClassifier::increment_lock_count(m_current_slot_no);
    m_current_slot_timepoints = SegmentationClassifier::get_slot_size();
  }
}

void SegmentationImpl::handle_timepoint()
{
  // std::cerr << "SegmentationImpl::handle_timepoint " << std::endl;
  acquire_slot();

  vectorize_timepoint(m_current_timepoint);
  m_char_len[m_current_timepoint] = m_input_encoder.get_len();

  increment_timepoint(m_current_timepoint);
  if (0 == m_current_slot_timepoints)
  {
    SegmentationClassifier::start_job(m_current_slot_no);
    // std::cerr << "Slot " << m_current_slot_no << " sent to inference engine (segmentation)" << std::endl;
    acquire_slot();
  }
}

void SegmentationImpl::no_more_data()
{
  if (m_current_slot_timepoints < SegmentationClassifier::get_slot_size())
  {
    SegmentationClassifier::set_slot_end(m_current_slot_no, m_current_timepoint);
    SegmentationClassifier::start_job(m_current_slot_no, true);
  }
}

} // namespace impl
 // namespace lemmatization
 // namespace deeplima

