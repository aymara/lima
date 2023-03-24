// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_TAGGING_EIGEN_INFERENCE_IMPL_H
#define DEEPLIMA_TAGGING_EIGEN_INFERENCE_IMPL_H

#include <eigen3/Eigen/Dense>

#include "bilstm.h"
#include "bilstm_and_dense.h"

#include "embd_dict.h"

#include "deeplima/segmentation/impl/char_ngram.h"
#include "birnn_inference_base.h"

namespace deeplima
{
namespace tagging
{
namespace eigen_impl
{
#ifdef WIN32
#ifdef TAG_EXPORTING
  #define TAG_EXPORT __declspec(dllexport)
#else
  #define TAG_EXPORT __declspec(dllimport)
#endif
#else
  #define TAG_EXPORT
#endif

template <typename M, typename V, typename T, typename AuxScalar=float>
class TAG_EXPORT BiRnnEigenInferenceForTagging : public deeplima::eigen_impl::BiRnnInferenceBase<M, V, T>
{
public:
  typedef M Matrix;
  typedef V Vector;
  typedef T Scalar;
  typedef M tensor_t;
  typedef EmbdUInt64FloatHolder dicts_holder_t;
  typedef deeplima::eigen_impl::BiRnnInferenceBase<M, V, T> Parent;
  typedef deeplima::eigen_impl::Op_BiLSTM_Dense_ArgMax<M, V, T, AuxScalar> op_bilstm_dense_argmax_t;

  virtual void load(const std::string& fn)
  {
    convert_from_torch(fn);
  }

  virtual void convert_classes_from_fn(const std::string& fn,
                                       std::vector<std::string>& classes_names,
                                       std::vector<std::vector<std::string>>& classes);

  virtual size_t get_precomputed_dim() const
  {
    auto p_params =
      std::dynamic_pointer_cast<typename op_bilstm_dense_argmax_t::params_t>(Parent::m_params[0]);

    const auto& layer = p_params->bilstm;
    size_t hidden_size = layer.fw.weight_ih.rows() + layer.bw.weight_ih.rows();
    return hidden_size;
  }

  virtual void precompute_inputs(
      const M& inputs,
      M& outputs,
      int64_t input_size
      )
  {
    auto p_op = std::dynamic_pointer_cast<op_bilstm_dense_argmax_t>(Parent::m_ops[0]);

    p_op->precompute_inputs(Parent::m_params[0], inputs, outputs, input_size);
  }

  virtual void predict(
      size_t worker_id,
      const M& inputs,
      int64_t input_begin,
      int64_t input_end,
      int64_t output_begin,
      int64_t output_end,
      std::shared_ptr< StdMatrix<uint8_t> >& output,
      const std::vector<std::string>& /*outputs_names*/
      )
  {
    auto p_op = std::dynamic_pointer_cast<op_bilstm_dense_argmax_t>(Parent::m_ops[0]);
    assert(Parent::m_wb.size() > 0);
    assert(worker_id < Parent::m_wb[0].size());
    p_op->execute(Parent::m_wb[0][worker_id],
        inputs, Parent::m_params[0], output->m_tensor,
        input_begin, input_end,
        output_begin, output_end);
  }

  inline const std::string& get_embd_fn(size_t idx) const
  {
    return m_embd_fn[idx];
  }

protected:
  std::vector<std::string> m_embd_fn;

  virtual void convert_from_torch(const std::string& fn);
};

} // namespace eigen_impl
} // namespace tagging
} // namespace deeplima

#endif
