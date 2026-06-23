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
#include "deep_biaffine_attn_label_decoder.h"
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

class DEP_PARSING_EXPORT BiRnnAndDeepBiaffineAttentionEigenInference :
    public deeplima::eigen_impl::BiRnnInferenceBase
{
public:
  typedef Eigen::MatrixXf Matrix;
  typedef Eigen::VectorXf Vector;
  typedef float Scalar;
  typedef Eigen::MatrixXf tensor_t;
  typedef EmbdUInt64FloatHolder dicts_holder_t;
  typedef deeplima::eigen_impl::BiRnnInferenceBase Parent;

  virtual ~BiRnnAndDeepBiaffineAttentionEigenInference() = default;

  virtual void load(const std::string& fn)
  {
    convert_from_torch(fn);
  }

  virtual size_t get_precomputed_dim() const
  {
    auto p_params = std::dynamic_pointer_cast<typename deeplima::eigen_impl::Op_BiLSTM<Eigen::MatrixXf, Eigen::VectorXf, float>::params_t>(Parent::m_params[0]);
    const auto& layer = p_params->layers[0];
    auto hidden_size = layer.fw.weight_ih.rows() + layer.bw.weight_ih.rows();
    return hidden_size;
  }

  virtual void precompute_inputs(
      const Eigen::MatrixXf& inputs,
      Eigen::MatrixXf& outputs,
      int64_t input_size
      )
  {
    auto p_op = std::dynamic_pointer_cast<deeplima::eigen_impl::Op_BiLSTM<Eigen::MatrixXf, Eigen::VectorXf, float>>(Parent::m_ops[0]);

    p_op->precompute_inputs(Parent::m_params[0], inputs, outputs, input_size);
  }

  virtual void predict(
      size_t /*worker_id*/,
      const Eigen::MatrixXf& /*inputs*/,
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
      const Eigen::MatrixXf& inputs,
      int64_t input_begin,
      int64_t /*input_end*/,
      int64_t /*output_begin*/,
      int64_t /*output_end*/,
      std::shared_ptr< StdMatrix<uint32_t> >& output,
      const std::vector<size_t>& lengths,
      const std::vector<std::string>& /*outputs_names*/
      )
  {
    // std::cerr << "BiRnnAndDeepBiaffineAttentionEigenInference<Eigen::MatrixXf, Eigen::VectorXf, float>::predict "
    //           << "input_begin=" << input_begin
    //           << ", output dim=" << output->dim() << ", lengths=" << lengths << std::endl;
    auto p_encoder = std::dynamic_pointer_cast<deeplima::eigen_impl::Op_BiLSTM<Eigen::MatrixXf, Eigen::VectorXf, float>>(Parent::m_ops[0]);

    // const typename deeplima::eigen_impl::Op_BiLSTM<Eigen::MatrixXf, Eigen::VectorXf, float>::params_t *plstm
    //     = static_cast<const typename deeplima::eigen_impl::Op_BiLSTM<Eigen::MatrixXf, Eigen::VectorXf, float>::params_t*>(Parent::m_params[0]);

    assert(Parent::m_wb.size() > 0);
    assert(worker_id < Parent::m_wb[0].size());

    auto wb = std::dynamic_pointer_cast<typename deeplima::eigen_impl::Op_BiLSTM<Eigen::MatrixXf, Eigen::VectorXf, float>::workbench_t>(Parent::m_wb[0][worker_id]);

    auto p_decoder = std::dynamic_pointer_cast<typename deeplima::eigen_impl::Op_DeepBiaffineAttnDecoder<Eigen::MatrixXf, Eigen::VectorXf, float>>(
      Parent::m_ops[1]);

    const bool predict_labels =
        !m_deep_biaffine_attn_label_decoder.empty() && output->size() >= 2;
    deeplima::eigen_impl::Op_DeepBiaffineAttnLabelDecoder<Eigen::MatrixXf, Eigen::VectorXf, float> label_op;

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

      if (predict_labels)
      {
        // Score deprels at the heads just predicted by the arc decoder.
        const Eigen::MatrixXf& enc = wb->get_last_output();
        const Eigen::MatrixXf sent_input = enc.block(0, 0, enc.rows(), lengths[i]);
        label_op.predict_labels(*m_deep_biaffine_attn_label_decoder[0],
                                sent_input, (*output)[0], start, (*output)[1]);
      }

      start += lengths[i];
    }
    // std::cerr << "BiRnnAndDeepBiaffineAttentionEigenInference<Eigen::MatrixXf, Eigen::VectorXf, float>::predict executes done " << start << std::endl;
    // arborescence<uint32_t, typename Eigen::MatrixXf::Scalar>((*output)[0], start);
    // std::cerr << "BiRnnAndDeepBiaffineAttentionEigenInference<Eigen::MatrixXf, Eigen::VectorXf, float>::predict after correcting arborescence: " << (*output)[0] << std::endl;
  }

  inline const std::string& get_embd_fn(size_t idx) const
  {
    return m_embd_fn[idx];
  }

  // deprel id -> string; empty if the model has no label decoder.
  const std::vector<std::string>& get_rel_class_names() const
  {
    return m_rel_class_names;
  }

  bool has_label_decoder() const
  {
    return !m_deep_biaffine_attn_label_decoder.empty();
  }

protected:
  std::vector<std::string> m_embd_fn;

  std::vector<std::shared_ptr<deeplima::eigen_impl::params_deep_biaffine_attn_decoder_t<Eigen::MatrixXf, Eigen::VectorXf>>> m_deep_biaffine_attn_decoder;
  std::map<std::string, size_t> m_deep_biaffine_attn_decoder_idx;

  std::vector<std::shared_ptr<deeplima::eigen_impl::params_deep_biaffine_attn_label_decoder_t<Eigen::MatrixXf, Eigen::VectorXf>>> m_deep_biaffine_attn_label_decoder;
  std::vector<std::string> m_rel_class_names;

  virtual void convert_from_torch(const std::string& fn);
};


} // namespace eigen_impl
} // namespace graph_dp
} // namespace deeplima

#endif
