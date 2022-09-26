// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_EIGEN_WRP_BIRNN_INFERENCE_BASE_H
#define DEEPLIMA_EIGEN_WRP_BIRNN_INFERENCE_BASE_H

#include <eigen3/Eigen/Dense>

#include "bilstm.h"
#include "bilstm_and_dense.h"

#include "embd_dict.h"

#include "nn/birnn_seq_cls/birnn_seq_classifier.h"

namespace deeplima
{
namespace eigen_impl
{

template <class M, class V, class T>
class BiRnnInferenceBase
{
public:
  typedef M Matrix;
  typedef V Vector;
  typedef T Scalar;
  typedef M tensor_t;
  typedef EmbdUInt64FloatHolder uint_dicts_holder_t;
  typedef EmbdStrFloatHolder str_dicts_holder_t;

  virtual void load(const std::string& fn) = 0;

  inline const uint_dicts_holder_t& get_input_uint_dicts() const
  {
    return m_input_uint_dicts;
  }

  inline const std::vector<std::string>& get_input_uint_dicts_names() const
  {
    return m_input_uint_dicts_names;
  }

  inline const str_dicts_holder_t& get_input_str_dicts() const
  {
    return m_input_str_dicts;
  }

  inline const std::vector<std::string>& get_input_str_dicts_names() const
  {
    return m_input_str_dicts_names;
  }

  inline const std::vector<std::vector<std::string>>& get_output_str_dicts() const
  {
    return m_output_str_dicts;
  }

  inline const std::vector<std::string>& get_output_str_dicts_names() const
  {
    return m_output_str_dicts_names;
  }

  virtual size_t init_new_worker(size_t input_len, bool precomputed_input=false)
  {
    assert(m_wb.size() > 0);
    assert(m_ops.size() > 0);
    assert(m_ops.size() == m_params.size());

    size_t new_worker_idx = m_wb[0].size();
    for (size_t i = 0; i < m_ops.size(); i++)
    {
      assert(m_wb[i].size() == new_worker_idx);
      assert(nullptr != m_params[i]);
      m_wb[i].push_back(m_ops[i]->create_workbench(input_len, m_params[i], precomputed_input));
    }

    return new_worker_idx;
  }

  virtual ~BiRnnInferenceBase()
  {
    for (std::vector<Op_Base::workbench_t*>& v : m_wb)
    {
      for (Op_Base::workbench_t* p : v)
      {
        if (nullptr != p)
        {
          delete p;
        }
      }
    }

    for (Op_Base* p : m_ops)
    {
      if (nullptr != p)
      {
        delete p;
      }
    }

    for (param_base_t* p : m_params)
    {
      if (nullptr != p)
      {
        // TODO: Why this fails?
        // delete p;
      }
    }
  }

  virtual void precompute_inputs(
      const M& inputs,
      M& outputs,
      int64_t input_size
      ) = 0;

  virtual void predict(
      size_t worker_id,
      const M& inputs,
      int64_t input_begin,
      int64_t input_end,
      int64_t output_begin,
      int64_t output_end,
      std::vector<std::vector<uint8_t>>& output,
      const std::vector<std::string>& outputs_names
      ) = 0;

protected:
  std::vector<Op_Base*> m_ops;
  std::vector<param_base_t*> m_params; // TODO: replace this

  std::vector<std::vector<Op_Base::workbench_t*>> m_wb; // outer - calculation step, inner - worker id

  uint_dicts_holder_t m_input_uint_dicts;
  std::vector<std::string> m_input_uint_dicts_names;
  str_dicts_holder_t m_input_str_dicts;
  std::vector<std::string> m_input_str_dicts_names;

  std::vector<std::vector<std::string>> m_output_str_dicts;
  std::vector<std::string> m_output_str_dicts_names;

  typedef params_bilstm_t<M, V> params_bilstm_spec_t;
  std::vector<params_bilstm_spec_t> m_lstm;
  std::map<std::string, size_t> m_lstm_idx;

  typedef params_multilayer_bilstm_t<M, V> params_multilayer_bilstm_spec_t;
  std::vector<params_multilayer_bilstm_spec_t> m_multi_bilstm;
  std::map<std::string, size_t> m_multi_bilstm_idx;

  std::vector<params_linear_t<M, V>> m_linear;
  std::map<std::string, size_t> m_linear_idx;

  virtual void convert_from_torch(const std::string& fn) = 0;
  virtual void convert_dicts_and_embeddings(const nets::BiRnnClassifierImpl& src);
};

} // namespace eigen_impl
} // namespace deeplima

#endif
