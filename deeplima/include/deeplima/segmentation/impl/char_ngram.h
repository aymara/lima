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

#ifndef DEEPLIMA_SEGMENTATION_IMPL_CHAR_NGRAM_H
#define DEEPLIMA_SEGMENTATION_IMPL_CHAR_NGRAM_H

namespace deeplima
{
namespace segmentation
{
namespace impl
{

struct ngram_descr_t
{
  enum ngram_type_t : uint8_t
  {
    char_ngram = 0x00,
    type_ngram = 0x01,
    len_ngram = 0x02,
    script_ngram = 0x03,
    max_ngram_type
  };

  int8_t m_offset;  // from the current character
  uint8_t m_length;
  ngram_type_t m_type;

  ngram_descr_t(int8_t offset = 0, uint8_t length = 0, ngram_type_t type = char_ngram)
    : m_offset(offset), m_length(length), m_type(type) {}

  explicit ngram_descr_t(const std::string& serialized_form)
    : m_offset(0), m_length(0), m_type(char_ngram)
  {
    from_string(serialized_form);
  }

  friend inline bool operator<(const ngram_descr_t& l, const ngram_descr_t& r)
  {
    return std::tie(l.m_type, l.m_length, l.m_offset) < std::tie(r.m_type, r.m_length, r.m_offset);
  }

  inline bool operator==(const ngram_descr_t& other)
  {
    return (other.m_type == m_type) && (other.m_length == m_length) && (other.m_offset == m_offset);
  }

  std::string to_string() const
  {
    std::ostringstream s;
    s << (int)m_type << " " << (int)m_length << " " << (int)m_offset;
    return s.str();
  }

  void from_string(const std::string& str)
  {
    std::istringstream s(str);
    int temp = 0;

    s >> temp;
    assert(temp >= 0);
    assert(temp < ngram_descr_t::max_ngram_type);
    m_type = ngram_type_t(temp);

    s >> temp;
    assert(temp > 0);
    assert(temp < std::numeric_limits<uint8_t>::max());
    m_length = (uint8_t)temp;

    s >> temp;
    assert(temp > std::numeric_limits<int8_t>::min());
    assert(temp < std::numeric_limits<int8_t>::max());
    m_offset = (int8_t)temp;
  }
};

} // namespace impl
} // namespace segmentation
} // namespace deeplima

#endif
