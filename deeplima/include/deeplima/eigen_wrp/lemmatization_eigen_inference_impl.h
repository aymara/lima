// Copyright 2002-2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_LEMMATIZATION_EIGEN_INFERENCE_IMPL_H
#define DEEPLIMA_LEMMATIZATION_EIGEN_INFERENCE_IMPL_H

#include <eigen3/Eigen/Dense>

#include "bilstm.h"
#include "bilstm_and_dense.h"
#include "lstm_beam_decoder.h"

#include "embd_dict.h"

#include "birnn_inference_base.h"
#include "morph_model/morph_model.h"

//#define LEMM_INFERENCE_PROFILE

#ifdef LEMM_INFERENCE_PROFILE
#include <chrono>
#endif

namespace deeplima
{
namespace lemmatization
{
namespace eigen_impl
{

#ifdef WIN32
#ifdef LEMM_EXPORTING
  #define LEMM_EXPORT __declspec(dllexport)
#else
  #define LEMM_EXPORT  __declspec(dllimport)
#endif
#else
  #define LEMM_EXPORT
#endif

template <class M, class V, class T>
class LEMM_EXPORT BiRnnSeq2SeqEigenInferenceForLemmatization : public deeplima::eigen_impl::BiRnnInferenceBase<M, V, T>
{
public:
  typedef M Matrix;
  typedef V Vector;
  typedef T Scalar;
  typedef M tensor_t;
  typedef EmbdUInt64FloatHolder dicts_holder_t;
  typedef deeplima::eigen_impl::BiRnnInferenceBase<M, V, T> Parent;

  virtual ~BiRnnSeq2SeqEigenInferenceForLemmatization() = default;

  virtual void load(const std::string& fn) override
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

    m_workbenches.push_back(std::make_shared<workbench_t>());

    return new_worker_idx;
  }

  virtual size_t get_precomputed_dim() const
  {
    auto p_params = std::dynamic_pointer_cast<typename deeplima::eigen_impl::Op_BiLSTM<M, V, T>::params_t>(Parent::m_params[0]);

    assert(p_params->layers.size() > 0);
    const auto& layer = p_params->layers[0];
    size_t hidden_size = layer.fw.weight_ih.rows() + layer.bw.weight_ih.rows();
    return hidden_size;
  }

  virtual void precompute_inputs(
      const M& inputs,
      M& outputs,
      int64_t input_size
      ) override
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
      ) override
  {
    assert(false);
  }

  struct workbench_t
  {
    Vector encoded_feats_for_encoder;
    Vector encoded_feats_for_decoder;
    Vector fw_h, fw_c, bw_h, bw_c; // encoder's output
    Vector encoder_state; // too bad: this is a copy of previous things
    Vector decoder_initial_h, decoder_initial_c;
  };

  virtual void predict(
      size_t worker_id,
      const M& inputs,
      const M& input_feats,
      int64_t input_len,
      int64_t output_max_len,
      int64_t /*beam_size*/,
      std::vector< uint32_t >& output,
      const std::vector<std::string>& /*output_names*/
      )
  {
    assert(Parent::m_wb.size() > 0);
    assert(worker_id < Parent::m_wb[0].size());
    assert(m_workbenches.size() > worker_id);
    workbench_t& wb = *(m_workbenches[worker_id]);

#ifdef LEMM_INFERENCE_PROFILE
    using clock = std::chrono::system_clock;
    using ms = std::chrono::duration<double, std::milli>;
#endif

    // Features encoders
    // for encoder
    auto p_linear_feats_enc = std::dynamic_pointer_cast<deeplima::eigen_impl::Op_Linear<M, V, T>>(Parent::m_ops[5]);

#ifdef LEMM_INFERENCE_PROFILE
    const auto before = clock::now();
#endif

    p_linear_feats_enc->execute(Parent::m_wb[5][worker_id],
                                input_feats,
                                Parent::m_params[5],
                                wb.encoded_feats_for_encoder);

#ifdef LEMM_INFERENCE_PROFILE
   const auto exec1 = clock::now();
#endif

    // for decoder
    auto p_linear_feats_dec = std::dynamic_pointer_cast<deeplima::eigen_impl::Op_Linear<M, V, T>>(Parent::m_ops[3]);

    p_linear_feats_dec->execute(Parent::m_wb[3][worker_id],
                                input_feats,
                                Parent::m_params[3],
                                wb.encoded_feats_for_decoder);
#ifdef LEMM_INFERENCE_PROFILE
   const auto exec2 = clock::now();
#endif
    auto p_encoder = std::dynamic_pointer_cast<deeplima::eigen_impl::Op_BiLSTM<M, V, T>>(Parent::m_ops[0]);

    auto p_decoder = std::dynamic_pointer_cast<deeplima::eigen_impl::Op_LSTM_Beam_Decoder<M, V, T>>(Parent::m_ops[4]);

    auto enc_mutlilayer_bilstm
        = std::dynamic_pointer_cast<const deeplima::eigen_impl::params_multilayer_bilstm_t<M, V>>(Parent::m_params[0]);
    size_t hidden_size = enc_mutlilayer_bilstm->layers[0].fw.weight_ih.rows() / 4;

    if (true)
    {
      // layout of encoded_feats_for_encoder:
      //
      // encoder_init_state_ = forward module=fc_cat2encoder input=categories_enc_embd
      // encoder_init_state = reshape input=encoder_init_state_ dims=2,-1,(encoder_input_size / 2)
      wb.fw_h = wb.encoded_feats_for_encoder.head(hidden_size);
      wb.fw_c = wb.fw_h;
      wb.bw_h = wb.encoded_feats_for_encoder.tail(hidden_size);
      wb.bw_c = wb.bw_h;
    }

#ifdef LEMM_INFERENCE_PROFILE
    std::cerr << "inputs.cols()=" << inputs.cols() << std::endl;
    std::cerr << "inputs.rows()=" << inputs.rows() << std::endl;

    const auto exec3 = clock::now();
#endif

    p_encoder->execute(Parent::m_wb[0][worker_id],
        inputs, Parent::m_params[0],
        0, input_len, wb.fw_h, wb.fw_c, wb.bw_h, wb.bw_c);

#ifdef LEMM_INFERENCE_PROFILE
    const auto exec4 = clock::now();
#endif

    if (wb.encoder_state.rows() == 0)
    {
      wb.encoder_state = Vector(hidden_size * 4 + wb.encoded_feats_for_decoder.rows());
    }
    wb.encoder_state << wb.fw_h, wb.fw_c, wb.bw_h, wb.bw_c, wb.encoded_feats_for_decoder;

    auto p_linear_h = std::dynamic_pointer_cast<deeplima::eigen_impl::Op_Linear<M, V, T>>(Parent::m_ops[1]);
    auto p_linear_c = std::dynamic_pointer_cast<deeplima::eigen_impl::Op_Linear<M, V, T>>(Parent::m_ops[2]);

#ifdef LEMM_INFERENCE_PROFILE
    const auto exec5 = clock::now();
#endif

    p_linear_h->execute(Parent::m_wb[1][worker_id], wb.encoder_state, Parent::m_params[1], wb.decoder_initial_h);

#ifdef LEMM_INFERENCE_PROFILE
    const auto exec6 = clock::now();
#endif

    p_linear_c->execute(Parent::m_wb[2][worker_id], wb.encoder_state, Parent::m_params[2], wb.decoder_initial_c);

#ifdef LEMM_INFERENCE_PROFILE
    const auto exec7 = clock::now();
#endif

    const EmbdUInt64Float& decoder_embd = Parent::m_input_uint_dicts[1];
    p_decoder->execute(Parent::m_wb[4][worker_id], decoder_embd,
        wb.decoder_initial_h, wb.decoder_initial_c,
        Parent::m_params[4], 0x10FFFE, 0x10FFFF, 9, output, output_max_len);

#ifdef LEMM_INFERENCE_PROFILE
    const auto exec8 = clock::now();

    std::cerr << "feats for enc : " << (exec1 - before).count() << std::endl;
    std::cerr << "feats for dec : " << (exec2 - exec1).count() << std::endl;

    std::cerr << "copy          : " << (exec3 - exec2).count() << std::endl;
    std::cerr << "encoder       : " << (exec4 - exec3).count() << std::endl;
    std::cerr << "copy          : " << (exec5 - exec4).count() << std::endl;

    std::cerr << "prep h        : " << (exec6 - exec5).count() << std::endl;
    std::cerr << "prep c        : " << (exec7 - exec6).count() << std::endl;
    std::cerr << "decoder       : " << (exec8 - exec7).count() << std::endl;
    throw;
#endif
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

  std::vector<std::shared_ptr<workbench_t>> m_workbenches;

  virtual void convert_from_torch(const std::string& fn) override;
};

typedef BiRnnSeq2SeqEigenInferenceForLemmatization<Eigen::MatrixXf, Eigen::VectorXf, float> BiRnnSeq2SeqEigenInferenceForLemmatizationF;

} // namespace eigen_impl
} // namespace lemmatization
} // namespace deeplima

#endif // DEEPLIMA_LEMMATIZATION_EIGEN_INFERENCE_IMPL_H

