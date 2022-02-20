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

#ifndef DEEPLIMA_LEMMATIZATION_IMPL_LEMMATIZATION_WRAPPER_H
#define DEEPLIMA_LEMMATIZATION_IMPL_LEMMATIZATION_WRAPPER_H

#include <string>

namespace deeplima
{
namespace lemmatization
{
namespace impl
{

// This class does nothing.
// It only defines the interface of the inference module

template <class M>
class LemmatizationInferenceWrapper
{
public:
  LemmatizationInferenceWrapper()
  {
  }

  inline void load(const std::string& fn)
  {
    m_impl.load(fn);
  }

  inline size_t init_new_worker(size_t input_len, bool precomputed_input=false)
  {
    return m_impl.init_new_worker(input_len, precomputed_input);
  }

  inline void predict(
      size_t worker_id,
      const typename M::tensor_t& inputs,
      const typename M::tensor_t& input_feats,
      int64_t input_len,
      int64_t output_max_len,
      int64_t beam_size,
      std::vector<uint32_t>& output,
      const std::vector<std::string>& output_names
      )
  {
    assert(0 == worker_id);
    m_impl.predict(worker_id, inputs, input_feats, input_len, output_max_len, beam_size, output, output_names);
  }

  inline const typename M::uint_dicts_holder_t& get_uint_dicts() const
  {
    return m_impl.get_uint_dicts();
  }

  inline const typename M::str_dicts_holder_t& get_str_dicts() const
  {
    return m_impl.get_str_dicts();
  }

  inline const morph_model::morph_model_t& get_morph_model() const
  {
    return m_impl.get_morph_model();
  }

  inline const std::vector<size_t>& get_fixed_upos() const
  {
    return m_impl.get_fixed_upos();
  }

protected:

  M m_impl;
};

} // namespace impl
} // namespace lemmatization
} // namespace deeplima

#endif // DEEPLIMA_LEMMATIZATION_IMPL_LEMMATIZATION_WRAPPER_H

