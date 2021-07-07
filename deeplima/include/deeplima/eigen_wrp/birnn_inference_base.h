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

  const uint_dicts_holder_t& get_uint_dicts() const
  {
    return m_uint_dicts;
  }

  const str_dicts_holder_t& get_str_dicts() const
  {
    return m_str_dicts;
  }

  virtual size_t init_new_worker(size_t input_len)
  {
    assert(m_wb.size() > 0);
    assert(m_ops.size() > 0);
    assert(m_ops.size() == m_params.size());

    size_t new_worker_idx = m_wb[0].size();
    for (size_t i = 0; i < m_ops.size(); i++)
    {
      assert(m_wb[i].size() == new_worker_idx);
      assert(nullptr != m_params[i]);
      m_wb[i].push_back(m_ops[i]->create_workbench(input_len, m_params[i]));
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
        delete p;
      }
    }
  }

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

  uint_dicts_holder_t m_uint_dicts;
  str_dicts_holder_t m_str_dicts;

  std::vector<params_bilstm_t<M, V>> m_lstm;
  std::map<std::string, size_t> m_lstm_idx;

  std::vector<params_linear_t<M, V>> m_linear;
  std::map<std::string, size_t> m_linear_idx;

  virtual void convert_from_torch(const std::string& fn) = 0;
  virtual void convert_dicts_and_embeddings(const nets::BiRnnClassifierImpl& src);
};

} // namespace eigen_impl
} // namespace deeplima

#endif
