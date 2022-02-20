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

#ifndef DEEPLIMA_LEMMATIZATION_EIGEN_INFERENCE_IMPL_H
#define DEEPLIMA_LEMMATIZATION_EIGEN_INFERENCE_IMPL_H

#include <eigen3/Eigen/Dense>

#include "bilstm.h"
#include "bilstm_and_dense.h"
#include "lstm_beam_decoder.h"

#include "embd_dict.h"

#include "birnn_inference_base.h"
#include "morph_model/morph_model.h"

namespace deeplima
{
namespace lemmatization
{
namespace eigen_impl
{

template <class M, class V, class T>
class BiRnnSeq2SeqEigenInferenceForLemmatization : public deeplima::eigen_impl::BiRnnInferenceBase<M, V, T>
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

  virtual size_t init_new_worker(size_t input_len, bool precomputed_input=false) override
  {
    assert(Parent::m_wb.size() > 0);
    assert(Parent::m_ops.size() > 0);
    assert(Parent::m_ops.size() == Parent::m_params.size());

    size_t new_worker_idx = Parent::m_wb[0].size();
    for (size_t i = 0; i < Parent::m_ops.size(); i++)
    {
      assert(Parent::m_wb[i].size() == new_worker_idx);
      assert(nullptr != Parent::m_params[i]);
      Parent::m_wb[i].push_back(Parent::m_ops[i]->create_workbench(input_len, Parent::m_params[i], precomputed_input));
    }

    return new_worker_idx;
  }

  virtual size_t get_precomputed_dim() const
  {
    typename deeplima::eigen_impl::Op_BiLSTM_Dense_ArgMax<M, V, T>::params_t *p_params =
      static_cast<typename deeplima::eigen_impl::Op_BiLSTM_Dense_ArgMax<M, V, T>::params_t*>(Parent::m_params[0]);

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
    deeplima::eigen_impl::Op_BiLSTM_Dense_ArgMax<M, V, T> *p_op
        = static_cast<deeplima::eigen_impl::Op_BiLSTM_Dense_ArgMax<M, V, T>*>(Parent::m_ops[0]);

    p_op->precompute_inputs(Parent::m_params[0], inputs, outputs, input_size);
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
    assert(false);
  }

  virtual void predict(
      size_t worker_id,
      const M& inputs,
      const M& input_feats,
      int64_t input_len,
      int64_t output_max_len,
      int64_t beam_size,
      std::vector<uint32_t>& output,
      const std::vector<std::string>& output_names
      )
  {
    assert(Parent::m_wb.size() > 0);
    assert(worker_id < Parent::m_wb[0].size());

    // Features encoders
    // for encoder
    deeplima::eigen_impl::Op_Linear<M, V, T> *p_linear_feats_enc
        = static_cast<deeplima::eigen_impl::Op_Linear<M, V, T>*>(Parent::m_ops[5]);
    Vector encoded_feats_for_encoder;
    p_linear_feats_enc->execute(Parent::m_wb[5][worker_id], input_feats, Parent::m_params[5], encoded_feats_for_encoder);

    // for decoder
    deeplima::eigen_impl::Op_Linear<M, V, T> *p_linear_feats_dec
        = static_cast<deeplima::eigen_impl::Op_Linear<M, V, T>*>(Parent::m_ops[3]);

    Vector encoded_feats;
    p_linear_feats_dec->execute(Parent::m_wb[3][worker_id], input_feats, Parent::m_params[3], encoded_feats);

    deeplima::eigen_impl::Op_BiLSTM<M, V, T> *p_encoder
        = static_cast<deeplima::eigen_impl::Op_BiLSTM<M, V, T>*>(Parent::m_ops[0]);

    deeplima::eigen_impl::Op_LSTM_Beam_Decoder<M, V, T> *p_decoder
        = static_cast<deeplima::eigen_impl::Op_LSTM_Beam_Decoder<M, V, T>*>(Parent::m_ops[4]);

    const deeplima::eigen_impl::params_bilstm_t<M, V>& layer
        = *static_cast<const deeplima::eigen_impl::params_bilstm_t<M, V>*>(Parent::m_params[0]);
    size_t hidden_size = layer.fw.weight_ih.rows() / 4;

    Vector fw_h, fw_c, bw_h, bw_c;
    if (true)
    {
      // layout of encoded_feats_for_encoder:
      //
      // encoder_init_state_ = forward module=fc_cat2encoder input=categories_enc_embd
      // encoder_init_state = reshape input=encoder_init_state_ dims=2,-1,(encoder_input_size / 2)
      fw_h = encoded_feats_for_encoder.head(hidden_size);
      fw_c = fw_h;
      bw_h = encoded_feats_for_encoder.tail(hidden_size);
      bw_c = bw_h;
    }
    p_encoder->execute(Parent::m_wb[0][worker_id],
        inputs, Parent::m_params[0],
        0, input_len, fw_h, fw_c, bw_h, bw_c);


    Vector encoder_state(hidden_size * 4 + encoded_feats.rows());
    encoder_state << fw_h, fw_c, bw_h, bw_c, encoded_feats;

    deeplima::eigen_impl::Op_Linear<M, V, T> *p_linear_h
        = static_cast<deeplima::eigen_impl::Op_Linear<M, V, T>*>(Parent::m_ops[1]);
    deeplima::eigen_impl::Op_Linear<M, V, T> *p_linear_c
        = static_cast<deeplima::eigen_impl::Op_Linear<M, V, T>*>(Parent::m_ops[2]);

    Vector decoder_initial_h, decoder_initial_c;
    p_linear_h->execute(Parent::m_wb[1][worker_id], encoder_state, Parent::m_params[1], decoder_initial_h);
    p_linear_c->execute(Parent::m_wb[2][worker_id], encoder_state, Parent::m_params[2], decoder_initial_c);

    const EmbdUInt64Float& decoder_embd = Parent::m_uint_dicts[1];
    p_decoder->execute(Parent::m_wb[4][worker_id], decoder_embd,
        decoder_initial_h, decoder_initial_c,
        Parent::m_params[4], 0x10FFFE, 9, output, output_max_len);
  }

  const std::vector<std::vector<std::string>>& get_classes() const
  {
    return m_classes;
  }

  const std::vector<std::string>& get_class_names() const
  {
    return m_class_names;
  }

  const morph_model::morph_model_t& get_morph_model() const
  {
    return m_morph_model;
  }

  inline const std::string& get_embd_fn(size_t idx) const
  {
    return m_embd_fn[idx];
  }

  const std::vector<size_t>& get_fixed_upos() const
  {
    return m_fixed_upos;
  }

protected:
  std::vector<std::string> m_class_names;
  std::vector<std::vector<std::string>> m_classes;
  std::vector<std::string> m_embd_fn;
  morph_model::morph_model_t m_morph_model;
  std::vector<size_t> m_fixed_upos;

  virtual void convert_from_torch(const std::string& fn);
};

typedef BiRnnSeq2SeqEigenInferenceForLemmatization<Eigen::MatrixXf, Eigen::VectorXf, float> BiRnnSeq2SeqEigenInferenceForLemmatizationF;

} // namespace eigen_impl
} // namespace lemmatization
} // namespace deeplima

#endif // DEEPLIMA_LEMMATIZATION_EIGEN_INFERENCE_IMPL_H

