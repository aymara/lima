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

  virtual workbench_t* create_workbench(uint32_t input_size, const param_base_t* params, bool precomputed_input=false) const
  {
    assert(input_size > 0);
    assert(nullptr != params);
    const params_bilstm_t<M, V>& layer = static_cast<const params_t*>(params)->bilstm;
    const std::vector<params_linear_t<M, V>>& linear = static_cast<const params_t*>(params)->linear;

    std::vector<uint32_t> output_sizes;
    output_sizes.reserve(linear.size());
    for ( const auto& p : linear )
    {
      output_sizes.push_back(p.weight.rows());
    }
    return new workbench_t(input_size, layer.fw.weight_ih.rows() / 4, output_sizes, precomputed_input);
  }

  virtual bool supports_precomputing() const
  {
    return true;
  }

  virtual void precompute_inputs(const param_base_t* params, const M& inputs, M& outputs, int64_t first_column)
  {
    assert(nullptr != params);
    const params_bilstm_t<M, V>& layer = static_cast<const params_t*>(params)->bilstm;
    size_t hidden_size = layer.fw.weight_ih.rows() / 4;

    auto output_block = outputs.block(0, first_column, outputs.rows(), inputs.cols());
    // Top rows - forward pass
    output_block.topRows(hidden_size * 4) = (layer.fw.weight_ih * inputs).colwise() + layer.fw.bias_ih;
    // Bottom rows - backward pass
    output_block.bottomRows(hidden_size * 4) = (layer.bw.weight_ih * inputs).colwise() + layer.bw.bias_ih;
  }

  virtual size_t execute(Op_Base::workbench_t* pwb,
                         const M& input_matrix,
                         const param_base_t* params,
                         std::vector<std::vector<uint8_t>>& final_output,
                         size_t input_begin,
                         size_t input_end,
                         size_t output_begin,
                         size_t output_end)
  {
    assert(nullptr != pwb);
    assert(nullptr != params);
    const params_bilstm_t<M, V>& layer = static_cast<const params_t*>(params)->bilstm;
    const std::vector<params_linear_t<M, V>>& linear = static_cast<const params_t*>(params)->linear;

    workbench_t* wb = static_cast<workbench_t*>(pwb);
    M& temp = wb->temp;
    M& output = wb->out;
    const V& zero = wb->zero;

    size_t hidden_size = layer.fw.weight_ih.rows() / 4;
    V c = V::Zero(hidden_size);

    M input = input_matrix.block(0, input_begin, input_matrix.rows(), temp.cols());
    // TODO: rewrite this!!!

    V s, g_u, g_o, g_if;

    if (wb->m_precomputed_input)
    {
      // precomputed inputs
      temp = input;
    }
    else
    {
      // Top rows - forward pass
      temp.topRows(hidden_size * 4) = (layer.fw.weight_ih * input).colwise() + layer.fw.bias_ih;
      // Bottom rows - backward pass
      temp.bottomRows(hidden_size * 4) = (layer.bw.weight_ih * input).colwise() + layer.bw.bias_ih;
    }

    // Forward pass
    s = temp.col(0).topRows(hidden_size * 4) + layer.fw.bias_hh + layer.fw.weight_hh * zero;
    step_fw(hidden_size, 0, s, g_u, g_o, g_if, c, output);

    for (size_t t = 1; t < input.cols(); t++)
    {
      s = temp.col(t).topRows(hidden_size * 4) + layer.fw.bias_hh + layer.fw.weight_hh * output.col(t-1).topRows(hidden_size);
      step_fw(hidden_size, t, s, g_u, g_o, g_if, c, output);
    }

    // Backward pass
    V c_fw = c;
    c = V::Zero(hidden_size);
    int t = input.cols() - 1;
    s = temp.col(t).bottomRows(hidden_size * 4) + layer.bw.bias_hh + layer.bw.weight_hh * zero;
    step_bw(hidden_size, t, s, g_u, g_o, g_if, c, output);

    for (t = input.cols() - 2; t >= 0; t--)
    {
      s = temp.col(t).bottomRows(hidden_size * 4) + layer.bw.bias_hh + layer.bw.weight_hh * output.col(t+1).bottomRows(hidden_size);
      step_bw(hidden_size, t, s, g_u, g_o, g_if, c, output);
    }

    // Linear layer on top of RNN outputs
    for (size_t j = 0; j < final_output.size(); j++)
    {
      M& linear_output = wb->lin_out[j];
      linear_output = (linear[j].weight * output).colwise() + linear[j].bias;

      for (Eigen::Index i = output_begin - input_begin;
           i < output_end - input_begin; i++)
      {
        Eigen::Index idx = 0;
        typename M::Scalar v = linear_output.col(i).maxCoeff(&idx);
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
  inline void step(
        size_t hidden_size, // [in]     LSTM parameter
        size_t t,           // [in]     step (position in output)
        const V& s,         // [in]     result before gating
        V& g_u,             // [temp]   update gate
        V& g_o,             // [temp]   output gate
        V& g_if,            // [temp]   input and forget gates
        V& c,               // [in/out] cell state
        M& output,          // [out]    matrix of output states
        bool forward        // [in]     true - forward, false - backward
      )
  {
    g_if = 1 / (1 + Eigen::exp( 0 - s.segment(0, hidden_size * 2).array() ) );
    g_u = 2 / (1 + Eigen::exp( 0 - 2 * s.segment(hidden_size * 2, hidden_size).array() ) ) - 1; // tanh
    g_o = 1 / (1 + Eigen::exp( 0 - s.segment(hidden_size * 3, hidden_size).array() ) );

    c = g_if.segment(0, hidden_size).cwiseProduct(g_u) + g_if.segment(hidden_size, hidden_size).cwiseProduct(c);

    // TODO: remove if
    if (forward)
    {
      output.col(t).topRows(hidden_size) = g_o.cwiseProduct(c.unaryExpr( [](float x) { return my_tanh(x); } ));
    }
    else
    {
      output.col(t).bottomRows(hidden_size) = g_o.cwiseProduct(c.unaryExpr( [](float x) { return my_tanh(x); } ));
    }
  }

  inline void step_fw(
        size_t hidden_size, // [in]     LSTM parameter
        size_t t,           // [in]     step (position in output)
        const V& s,         // [in]     result before gating
        V& g_u,             // [temp]   update gate
        V& g_o,             // [temp]   output gate
        V& g_if,            // [temp]   input and forget gates
        V& c,               // [in/out] cell state
        M& output           // [out]    matrix of output states
      )
  {
    g_if = 1 / (1 + Eigen::exp( 0 - s.segment(0, hidden_size * 2).array() ) );
    g_u = 2 / (1 + Eigen::exp( 0 - 2 * s.segment(hidden_size * 2, hidden_size).array() ) ) - 1; // tanh
    g_o = 1 / (1 + Eigen::exp( 0 - s.segment(hidden_size * 3, hidden_size).array() ) );

    c = g_if.segment(0, hidden_size).cwiseProduct(g_u) + g_if.segment(hidden_size, hidden_size).cwiseProduct(c);

    output.col(t).topRows(hidden_size) = g_o.cwiseProduct(c.unaryExpr( [](float x) { return my_tanh(x); } ));
  }

  inline void step_bw(
        size_t hidden_size, // [in]     LSTM parameter
        size_t t,           // [in]     step (position in output)
        const V& s,         // [in]     result before gating
        V& g_u,             // [temp]   update gate
        V& g_o,             // [temp]   output gate
        V& g_if,            // [temp]   input and forget gates
        V& c,               // [in/out] cell state
        M& output           // [out]    matrix of output states
      )
  {
    g_if = 1 / (1 + Eigen::exp( 0 - s.segment(0, hidden_size * 2).array() ) );
    g_u = 2 / (1 + Eigen::exp( 0 - 2 * s.segment(hidden_size * 2, hidden_size).array() ) ) - 1; // tanh
    g_o = 1 / (1 + Eigen::exp( 0 - s.segment(hidden_size * 3, hidden_size).array() ) );

    c = g_if.segment(0, hidden_size).cwiseProduct(g_u) + g_if.segment(hidden_size, hidden_size).cwiseProduct(c);

    output.col(t).bottomRows(hidden_size) = g_o.cwiseProduct(c.unaryExpr( [](float x) { return my_tanh(x); } ));
  }

  inline static float my_tanh(float x)
  {
    return ::tanhf(x);
  }
};

} // namespace eigen_impl
} // namespace deeplima

#endif
