// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_SRC_INFERENCE_EIGEN_BILSTM_AND_DENSE_H
#define DEEPLIMA_SRC_INFERENCE_EIGEN_BILSTM_AND_DENSE_H

#include <vector>

#include "bilstm.h"
#include "linear.h"

namespace deeplima
{
namespace eigen_impl
{

template<class M, class V>
struct params_bilstm_dense_argmax_t : public param_base_t
{
  params_bilstm_t<M, V> bilstm;
  std::vector<params_linear_t<M, V>> linear;
};

template<class M, class V, class T>
class Op_BiLSTM_Dense_ArgMax : public Op_Base
{
protected:
  struct workbench_t : public Op_Base::workbench_t
  {
    workbench_t(uint32_t input_size,
                uint32_t hidden_size,
                const std::vector<uint32_t>& output_sizes,
                bool precomputed_input=false)
      : temp(M::Zero(precomputed_input ? 0 : hidden_size * 8, input_size)),
        out(M::Zero(hidden_size * 2, input_size)),
        zero(V::Zero(hidden_size)),
        m_precomputed_input(precomputed_input)
    {
      lin_out.resize(output_sizes.size());
      for (size_t i = 0; i < output_sizes.size(); i++)
      {
        lin_out[i] = M::Zero(output_sizes[i], input_size);
      }
    }

    virtual ~workbench_t() {}

    M temp;
    M out;
    std::vector<M> lin_out;
    const V zero;
    bool m_precomputed_input;
  };

public:

  typedef params_bilstm_dense_argmax_t<M, V> params_t;

  virtual std::shared_ptr<Op_Base::workbench_t> create_workbench(uint32_t input_size, const std::shared_ptr<param_base_t> params, bool precomputed_input=false) const override
  {
    assert(input_size > 0);
    assert(nullptr != params);
    const auto& layer = std::dynamic_pointer_cast<const params_t>(params)->bilstm;
    const auto& linear = std::dynamic_pointer_cast<const params_t>(params)->linear;

    std::vector<uint32_t> output_sizes;
    output_sizes.reserve(linear.size());
    for ( const auto& p : linear )
    {
      output_sizes.push_back(p.weight.rows());
    }
    return std::make_shared<workbench_t>(input_size, layer.fw.weight_ih.rows() / 4, output_sizes, precomputed_input);
  }

  virtual bool supports_precomputing() const
  {
    return true;
  }

  virtual void precompute_inputs(const std::shared_ptr<param_base_t> params, const M& inputs, M& outputs, int64_t first_column)
  {
    assert(nullptr != params);
    const auto& layer = std::dynamic_pointer_cast<const params_t>(params)->bilstm;
    size_t hidden_size = layer.fw.weight_ih.rows() / 4;

    auto output_block = outputs.block(0, first_column, outputs.rows(), inputs.cols());
    // Top rows - forward pass
    output_block.topRows(hidden_size * 4).noalias() = (layer.fw.weight_ih * inputs).colwise() + layer.fw.bias_ih + layer.fw.bias_hh;
    // Bottom rows - backward pass
    output_block.bottomRows(hidden_size * 4).noalias() = (layer.bw.weight_ih * inputs).colwise() + layer.bw.bias_ih + layer.bw.bias_hh;
  }

  virtual size_t execute(std::shared_ptr<Op_Base::workbench_t> pwb,
                         const M& input_matrix,
                         const std::shared_ptr<param_base_t> params,
                         std::vector<std::vector<uint8_t>>& final_output,
                         size_t input_begin,
                         size_t /*input_end*/,
                         size_t output_begin,
                         size_t output_end)
  {
    assert(nullptr != pwb);
    assert(nullptr != params);
    const auto& layer = std::dynamic_pointer_cast<const params_t>(params)->bilstm;
    const auto& linear = std::dynamic_pointer_cast<const params_t>(params)->linear;

    auto wb = std::dynamic_pointer_cast<workbench_t>(pwb);
    M& temp = wb->temp;
    M& output = wb->out;
    const V& zero = wb->zero;

    size_t hidden_size = layer.fw.weight_ih.rows() / 4;
    V c = V::Zero(hidden_size);
    V s;

    Eigen::Ref<const M> input = input_matrix.block(0, input_begin, input_matrix.rows(), temp.cols());

    if (wb->m_precomputed_input)
    {
      forward_pass(hidden_size, input, layer.fw, s, c, output);

      V c_fw = c;
      c = V::Zero(hidden_size);
      backward_pass(hidden_size, input, layer.bw, s, c, output);
    }
    else
    {
      // Top rows - forward pass
      temp.topRows(hidden_size * 4) = (layer.fw.weight_ih * input).colwise() + layer.fw.bias_ih;
      // Bottom rows - backward pass
      temp.bottomRows(hidden_size * 4) = (layer.bw.weight_ih * input).colwise() + layer.bw.bias_ih;

      forward_pass(hidden_size, temp, layer.fw, s, c, output);

      V c_fw = c;
      c = V::Zero(hidden_size);
      backward_pass(hidden_size, temp, layer.bw, s, c, output);
    }

    // Linear layer on top of RNN outputs
    for (size_t j = 0; j < final_output.size(); j++)
    {
      M& linear_output = wb->lin_out[j];
      linear_output.noalias() = (linear[j].weight * output).colwise() + linear[j].bias;

      for (auto i = output_begin - input_begin; i < output_end - input_begin; i++)
      {
        Eigen::Index idx = 0;
        // TODO scalar v value is not used but maxCoeff has side effect. It must be kept. Should we remove the
        // return value or use it somewhat?
        // typename M::Scalar v =
        linear_output.col(i).maxCoeff(&idx);
        assert(idx >= 0);
        assert(idx < std::numeric_limits<uint8_t>::max());
        final_output[j][input_begin+i] = (uint8_t) idx;
      }
    }

    // Linear layer on top of RNN outputs - alternative way to calculate (not faster)
    /*for (size_t j = 0; j < final_output.size(); j++)
    {
      M& linear_output = wb->lin_out[j];
      linear_output = (linear[j].weight * output).colwise() + linear[j].bias;
    }

    for (size_t j = 0; j < final_output.size(); j++)
    {
      M& linear_output = wb->lin_out[j];

      for (Eigen::Index i = output_begin - input_begin;
           i < output_end - input_begin; i++)
      {
        Eigen::Index idx = 0;
        typename M::Scalar v = linear_output.col(i).maxCoeff(&idx);
        assert(idx >= 0);
        assert(idx < std::numeric_limits<uint8_t>::max());
        final_output[j][input_begin+i] = (uint8_t) idx;
      }
    }*/

    return 0;
  }

protected:
  inline void forward_pass(
        const size_t hidden_size,       // [in]     the size of the hidden state
        Eigen::Ref<const M> input,      // [in]     the pre-computed inputs
        const params_lstm_t<M, V>& fw,  // [in]     weights for the forward pass
        V& s,                           // [temp]   preallocated space for results before gating
        V& c,                           // [in/out] the cell state
        M& output                       // [out]    the output states
      )
  {
    step_fw(hidden_size, 0, input.col(0).topRows(hidden_size * 4), c, output);

    for (Eigen::Index t = 1; t < input.cols(); t++)
    {
      s = input.col(t).topRows(hidden_size * 4);
      s.noalias() += fw.weight_hh * output.block(0, t-1, hidden_size, 1);

      step_fw(hidden_size, t, s, c, output);
    }
  }

  inline void backward_pass(
        const size_t hidden_size,       // [in]     the size of the hidden state
        Eigen::Ref<const M> input,      // [in]     the pre-computed inputs
        const params_lstm_t<M, V>& bw,  // [in]     weights for the backward pass
        V& s,                           // [temp]   preallocated space for results before gating
        V& c,                           // [in/out] the cell state
        M& output                       // [out]    the output states
      )
  {
    int t = input.cols() - 1;
    step_bw(hidden_size, t, input.col(t).bottomRows(hidden_size * 4), c, output);

    for (t = input.cols() - 2; t >= 0; t--)
    {
      s = input.col(t).bottomRows(hidden_size * 4);
      s.noalias() += bw.weight_hh * output.block(hidden_size, t+1, hidden_size, 1);
      step_bw(hidden_size, t, s, c, output);
    }
  }

  #define TANH(x)    ( T(2) / ( T(1) + Eigen::exp( T(-2) * x.array() ) ) - T(1) )
  #define SIGMOID(x) ( T(1) / ( T(1) + Eigen::exp( - x.array() ) ) )

  inline void update_c(
        const size_t hidden_size,  // [in]     the size of the hidden state
        const V& s,                // [in]     the results before gating
        V& c                       // [in/out] the cell state
      )
  {
    // c = c * forget_gate = c * 1 / sigmoid(...)
    c = c.array() / (1 + Eigen::exp( - s.segment(hidden_size, hidden_size).array() ) );

    // c = c + input_gate * update_gate
    c += ( SIGMOID(s.segment(0, hidden_size)).array() * TANH(s.segment(hidden_size * 2, hidden_size)).array() ).matrix();
  }

  inline void step_fw(
        const size_t hidden_size,  // [in]     the size of the hidden state
        const size_t t,            // [in]     step (the position in output)
        const V& s,                // [in]     the results before gating
        V& c,                      // [in/out] the cell state
        M& output                  // [out]    the matrix of output states
      )
  {
    update_c(hidden_size, s, c);

    // output = output_gate * tanh(c)
    output.col(t).topRows(hidden_size).noalias() = (SIGMOID(s.segment(hidden_size * 3, hidden_size)).array() * TANH(c).array()).matrix();
  }

  inline void step_bw(
        const size_t hidden_size,  // [in]     the size of the hidden state
        const size_t t,            // [in]     step (the position in output)
        const V& s,                // [in]     the results before gating
        V& c,                      // [in/out] the cell state
        M& output                  // [out]    matrix of output states
      )
  {
    update_c(hidden_size, s, c);

    // output = output_gate * tanh(c)
    output.col(t).bottomRows(hidden_size).noalias() = (SIGMOID(s.segment(hidden_size * 3, hidden_size)).array() * TANH(c).array()).matrix();
  }
};

} // namespace eigen_impl
} // namespace deeplima

#endif
