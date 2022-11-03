// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_APPS_UTF8_DECODER_H
#define DEEPLIMA_APPS_UTF8_DECODER_H

#include <limits>
#include <cassert>

#include <string.h>
#include <unicode/uchar.h>
#include <unicode/uscript.h>

namespace deeplima
{
namespace segmentation
{
namespace impl
{

#define UTF8_DECODER_CHAR_HISTORY_MASK 0x7FFFFFFFFFFFFFFFull
// 21 * 3 characters = 63 bits accepted

enum uc_channel_t : uint8_t {
  channel_char = 0,
  channel_type = 1,
  channel_len = 2,
  channel_script_change = 3,
  max_channel_value
};

const uint8_t bits_per_position_table[] = {
  21, // Unicode code point
  8,  // Unicode class
  3,  // UTF-8 character len in bytes (1 .. 6)
  1   // Script changed since prev code point (Yes / No)
};

template <class TBufferType = uint64_t, int NUM_CHANNELS = 4, int BYTES_BUFFER_SIZE = 8>
class Utf8Reader
{
protected:
  typedef TBufferType buffer_t;

private:
  uint8_t m_bytes_buffer[BYTES_BUFFER_SIZE];
  uint32_t m_bytes_left_to_parse;

  buffer_t m_buffer[NUM_CHANNELS];

  uint8_t m_prev_script_code;

  inline void push_value(uc_channel_t ch, uint32_t value)
  {
    m_buffer[ch] = (m_buffer[ch] << bits_per_position(ch)) | buffer_t(value);
  }

public:

  Utf8Reader()
    : m_bytes_left_to_parse(0),
      m_prev_script_code(USCRIPT_COMMON)
  {
    assert(USCRIPT_CODE_LIMIT < std::numeric_limits<uint8_t>::max());

    reset();
  }

  void reset()
  {
    m_bytes_left_to_parse = 0;
    m_prev_script_code = USCRIPT_COMMON;
    memset(m_bytes_buffer, 0, BYTES_BUFFER_SIZE);
    memset(m_buffer, 0, sizeof(buffer_t) * NUM_CHANNELS);
  }

  inline uint32_t parse_start(const uint8_t* str, int32_t& pos, int32_t len)
  {
    assert(nullptr != str);
    assert(0 == pos);
    assert(len > 0);
    if (m_bytes_left_to_parse > 0)
    {
      uint8_t bytes_to_copy = len > 4 ? 4 : len;
      memcpy(m_bytes_buffer + m_bytes_left_to_parse, str, bytes_to_copy);

      uint8_t char_len = parse(m_bytes_buffer, pos, m_bytes_left_to_parse + bytes_to_copy);
      assert(char_len > 0);

      pos = pos - m_bytes_left_to_parse;
      uint32_t rv = m_bytes_left_to_parse;
      m_bytes_left_to_parse = 0;

      return rv;
    }

    return 0;
  }

  inline uint8_t parse(const uint8_t* str, int32_t& pos, int32_t len)
  {
    int32_t uch = 0;
    int32_t prev_pos = pos;
    U8_NEXT(str, pos, len, uch);
    if (uch < 0)
    {
      uint32_t bytes_left = len - prev_pos;
      if (bytes_left < 4)
      {
        m_bytes_left_to_parse = bytes_left;
        memcpy(m_bytes_buffer, str + prev_pos, m_bytes_left_to_parse);
        return 0;
        // incomplete sequence
      }
      throw std::runtime_error("Incorrect UTF-8 sequence.");
    }

    push_value(channel_char, uch);

    push_value(channel_type, u_charType(uch));

    UErrorCode err = U_ZERO_ERROR;
#ifdef NDEBUG
    uint8_t script_code = uscript_getScript(uch, &err);
#else
    UScriptCode raw_script_code = uscript_getScript(uch, &err);
    assert(err == U_ZERO_ERROR);
    assert(raw_script_code >= 0);
    assert(raw_script_code < USCRIPT_CODE_LIMIT);
    uint8_t script_code = raw_script_code;
#endif
    push_value(channel_script_change, m_prev_script_code == script_code ? 0x00 : 0x01);
    m_prev_script_code = script_code;

    uint8_t char_len = pos - prev_pos;
    assert(char_len > 0);
    assert(char_len <= 6);
    push_value(channel_len, char_len);

    return char_len;
  }

  inline const buffer_t& get_buffer(uint8_t channel_idx) const
  {
    assert(channel_idx < NUM_CHANNELS);
    return m_buffer[channel_idx];
  }

  inline uint8_t get_len(uint8_t idx) const
  {
    return (m_buffer[channel_len] >> (bits_per_position(channel_len) * idx)) & 0x07;
  }

  inline static uint8_t bits_per_position(uint8_t channel_idx)
  {
    assert(channel_idx < NUM_CHANNELS);
    return bits_per_position_table[channel_idx];
  }
};

} // namespace impl
} // namespace segmentation
} // namespace deeplima

#endif
