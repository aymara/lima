// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_GRAPH_DP_IMPL_TAGGING_WRAPPER_H
#define DEEPLIMA_GRAPH_DP_IMPL_TAGGING_WRAPPER_H

#include <string>

namespace deeplima
{
namespace graph_dp
{
namespace impl
{

// This class does nothing.
// It only defines the interface of the inference module

template <class M>
class GraphDpInferenceWrapper
{
public:
  GraphDpInferenceWrapper()
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

  inline size_t get_precomputed_dim() const
  {
    return m_impl.get_precomputed_dim();
  }

  inline void precompute_inputs(
      const typename M::tensor_t& inputs,
      typename M::tensor_t& outputs,
      int64_t input_size
      )
  {
    m_impl.precompute_inputs(inputs, outputs, input_size);
  }

  inline void predict(
      size_t worker_id,
      const typename M::tensor_t& inputs,
      int64_t input_begin,
      int64_t input_end,
      int64_t output_begin,
      int64_t output_end,
      std::vector<std::vector<uint32_t>>& output,
      const std::vector<size_t>& lengths,
      const std::vector<std::string>& output_names
      )
  {
    m_impl.predict(worker_id, inputs,
                   input_begin, input_end,
                   output_begin, output_end,
                   output,
                   lengths,
                   output_names);
  }

  inline const typename M::uint_dicts_holder_t& get_uint_dicts() const
  {
    return m_impl.get_uint_dicts();
  }

  inline const typename M::str_dicts_holder_t& get_input_str_dicts() const
  {
    return m_impl.get_input_str_dicts();
  }

  inline const typename M::str_dicts_holder_t& get_output_str_dicts() const
  {
    return m_impl.get_output_str_dicts();
  }

  inline const std::vector<std::string>& get_output_str_dicts_names() const
  {
    return m_impl.get_output_str_dicts_names();
  }

  inline const std::vector<std::string>& get_input_str_dicts_names() const
  {
    return m_impl.get_input_str_dicts_names();
  }

  inline const std::string& get_embd_fn(size_t idx) const
  {
    return m_impl.get_embd_fn(idx);
  }

protected:

  M m_impl;
};

} // namespace impl
} // namespace graph_dp
} // namespace deeplima

#endif
