// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_SRC_INCLUDE_SEGMENTATION_SEGMENTATION_EIGEN_INFERENCE_IMPL_H
#define DEEPLIMA_SRC_INCLUDE_SEGMENTATION_SEGMENTATION_EIGEN_INFERENCE_IMPL_H

#include <eigen3/Eigen/Dense>

#include "bilstm.h"
#include "bilstm_and_dense.h"

#include "embd_dict.h"

#include "deeplima/segmentation/impl/char_ngram.h"
#include "birnn_inference_base.h"

namespace deeplima
{
namespace segmentation
{
namespace eigen_impl
{

template <class M, class V, class T>
class BiRnnEigenInferenceForSegmentation : public deeplima::eigen_impl::BiRnnInferenceBase<M, V, T>
{
public:
  typedef M Matrix;
  typedef V Vector;
  typedef T Scalar;
  typedef M tensor_t;
  typedef EmbdUInt64FloatHolder dicts_holder_t;
  typedef deeplima::eigen_impl::BiRnnInferenceBase<M, V, T> Parent;

  virtual void load(const std::string& fn)
  {
    convert_from_torch(fn);
  }

  const std::vector<impl::ngram_descr_t>& get_ngram_descr() const
  {
    return m_ngram_gescr;
  }

  virtual void precompute_inputs(
      const M& inputs,
      M& outputs,
      int64_t input_size
      )
  {
    throw std::runtime_error("Precomputing isn't supported for segmentation");
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
      )
  {
    deeplima::eigen_impl::Op_BiLSTM_Dense_ArgMax<M, V, T> *p_op
        = static_cast<deeplima::eigen_impl::Op_BiLSTM_Dense_ArgMax<M, V, T>*>(Parent::m_ops[0]);
    assert(Parent::m_wb.size() > 0);
    assert(worker_id < Parent::m_wb[0].size());
    p_op->execute(Parent::m_wb[0][worker_id],
        inputs, Parent::m_params[0], output,
        input_begin, input_end,
        output_begin, output_end);
  }

protected:
  std::vector<impl::ngram_descr_t> m_ngram_gescr;

  virtual void convert_from_torch(const std::string& fn);
};

typedef BiRnnEigenInferenceForSegmentation<Eigen::MatrixXf, Eigen::VectorXf, float> BiRnnEigenInferenceForSegmentationF;

} // namespace eigen_impl
} // namespace segmentation
} // namespace deeplima

#endif
