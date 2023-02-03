// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_GRAPH_DP_EIGEN_INFERENCE_IMPL_H
#define DEEPLIMA_GRAPH_DP_EIGEN_INFERENCE_IMPL_H

#include <eigen3/Eigen/Dense>

#include "birnn_inference_base.h"
#include "bilstm.h"
#include "deep_biaffine_attn_decoder.h"
// #include "deeplima/graph_dp/impl/arborescence.h"

namespace deeplima
{
namespace graph_dp
{
namespace eigen_impl
{

#ifdef WIN32
#ifdef DEP_PARSING_EXPORTING
  #define DEP_PARSING_EXPORT __declspec(dllexport)
#else
  #define DEP_PARSING_EXPORT  __declspec(dllimport)
#endif
#else
  #define DEP_PARSING_EXPORT
#endif

template <class M, class V, class T>
class DEP_PARSING_EXPORT BiRnnAndDeepBiaffineAttentionEigenInference : public deeplima::eigen_impl::BiRnnInferenceBase<M, V, T>
{
public:
  typedef M Matrix;
  typedef V Vector;
  typedef T Scalar;
  typedef M tensor_t;
  typedef EmbdUInt64FloatHolder dicts_holder_t;
  typedef deeplima::eigen_impl::BiRnnInferenceBase<M, V, T> Parent;

  virtual ~BiRnnAndDeepBiaffineAttentionEigenInference<M, V, T>() = default;

  virtual void load(const std::string& fn)
  {
    convert_from_torch(fn);
  }

  virtual size_t get_precomputed_dim() const
  {
    auto p_params = std::dynamic_pointer_cast<typename deeplima::eigen_impl::Op_BiLSTM<M, V, T>::params_t>(Parent::m_params[0]);
    const auto& layer = p_params->layers[0];
    auto hidden_size = layer.fw.weight_ih.rows() + layer.bw.weight_ih.rows();
    return hidden_size;
  }

  virtual void precompute_inputs(
      const M& inputs,
      M& outputs,
      int64_t input_size
      )
  {
    auto p_op = std::dynamic_pointer_cast<deeplima::eigen_impl::Op_BiLSTM<M, V, T>>(Parent::m_ops[0]);

    p_op->precompute_inputs(Parent::m_params[0], inputs, outputs, input_size);
  }

  virtual void predict(
      size_t /*worker_id*/,
      const M& /*inputs*/,
      int64_t /*input_begin*/,
      int64_t /*input_end*/,
      int64_t /*output_begin*/,
      int64_t /*output_end*/,
      std::shared_ptr< StdMatrix<uint8_t> >& /*output*/,
      const std::vector<std::string>& /*outputs_names*/
      )
  {
//     TODO to be implemented?
    assert(false);
  }

  virtual void predict(
      size_t worker_id,
      const M& inputs,
      int64_t input_begin,
      int64_t /*input_end*/,
      int64_t /*output_begin*/,
      int64_t /*output_end*/,
      std::shared_ptr< StdMatrix<uint32_t> >& output,
      const std::vector<size_t>& lengths,
      const std::vector<std::string>& /*outputs_names*/
      )
  {
    // std::cerr << "BiRnnAndDeepBiaffineAttentionEigenInference<M, V, T>::predict "
    //           << "input_begin=" << input_begin
    //           << ", output dim=" << output->dim() << ", lengths=" << lengths << std::endl;
    auto p_encoder = std::dynamic_pointer_cast<deeplima::eigen_impl::Op_BiLSTM<M, V, T>>(Parent::m_ops[0]);

    // const typename deeplima::eigen_impl::Op_BiLSTM<M, V, T>::params_t *plstm
    //     = static_cast<const typename deeplima::eigen_impl::Op_BiLSTM<M, V, T>::params_t*>(Parent::m_params[0]);

    assert(Parent::m_wb.size() > 0);
    assert(worker_id < Parent::m_wb[0].size());

    auto wb = std::dynamic_pointer_cast<typename deeplima::eigen_impl::Op_BiLSTM<M, V, T>::workbench_t>(Parent::m_wb[0][worker_id]);

    auto p_decoder = std::dynamic_pointer_cast<typename deeplima::eigen_impl::Op_DeepBiaffineAttnDecoder<M, V, T>>(
      Parent::m_ops[1]);

    size_t start = input_begin;
    for (size_t i = 0; i < lengths.size(); ++i)
    {
      p_encoder->execute(Parent::m_wb[0][worker_id],
          inputs, Parent::m_params[0],
          start, start + lengths[i]);

      p_decoder->execute(
        Parent::m_wb[1][worker_id],
        wb->get_last_output(),
        Parent::m_params[1],
        start,
        start + lengths[i],
        (*output)[0]);

      start += lengths[i];
    }
    // std::cerr << "BiRnnAndDeepBiaffineAttentionEigenInference<M, V, T>::predict executes done " << start << std::endl;
    // arborescence<uint32_t, typename M::Scalar>((*output)[0], start);
    // std::cerr << "BiRnnAndDeepBiaffineAttentionEigenInference<M, V, T>::predict after correcting arborescence: " << (*output)[0] << std::endl;
  }

  inline const std::string& get_embd_fn(size_t idx) const
  {
    return m_embd_fn[idx];
  }

protected:
  std::vector<std::string> m_embd_fn;

  std::vector<std::shared_ptr<deeplima::eigen_impl::params_deep_biaffine_attn_decoder_t<M, V>>> m_deep_biaffine_attn_decoder;
  std::map<std::string, size_t> m_deep_biaffine_attn_decoder_idx;

  virtual void convert_from_torch(const std::string& fn);
};

typedef BiRnnAndDeepBiaffineAttentionEigenInference<Eigen::MatrixXf, Eigen::VectorXf, float> BiRnnAndDeepBiaffineAttentionEigenInferenceF;

} // namespace eigen_impl
} // namespace graph_dp
} // namespace deeplima

#endif
