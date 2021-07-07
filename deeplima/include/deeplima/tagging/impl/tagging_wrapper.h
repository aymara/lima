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

#ifndef DEEPLIMA_TAGGING_IMPL_TAGGING_WRAPPER_H
#define DEEPLIMA_TAGGING_IMPL_TAGGING_WRAPPER_H

#include <string>

namespace deeplima
{
namespace tagging
{
namespace impl
{

// This class does nothing.
// It only defines the interface of the inference module

template <class M>
class TaggingInferenceWrapper
{
public:
  TaggingInferenceWrapper()
  {
  }

  inline void load(const std::string& fn)
  {
    m_impl.load(fn);
  }

  inline size_t init_new_worker(size_t input_len)
  {
    return m_impl.init_new_worker(input_len);
  }

  inline void predict(
      size_t worker_id,
      const typename M::tensor_t& inputs,
      int64_t input_begin,
      int64_t input_end,
      int64_t output_begin,
      int64_t output_end,
      std::vector<std::vector<uint8_t>>& output,
      const std::vector<std::string>& output_names
      )
  {
    m_impl.predict(worker_id, inputs,
                   input_begin, input_end,
                   output_begin, output_end,
                   output,
                   output_names);
  }

  inline const typename M::uint_dicts_holder_t& get_uint_dicts() const
  {
    return m_impl.get_uint_dicts();
  }

  inline const typename M::str_dicts_holder_t& get_str_dicts() const
  {
    return m_impl.get_str_dicts();
  }

  inline const std::vector<std::vector<std::string>>& get_classes() const
  {
    return m_impl.get_classes();
  }

  inline const std::string& get_embd_fn(size_t idx) const
  {
    return m_impl.get_embd_fn(idx);
  }

protected:

  M m_impl;
};

} // namespace impl
} // namespace tagging
} // namespace deeplima

#endif
