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

#ifndef DEEPLIMA_SRC_INFERENCE_EIGEN_BILSTM_H
#define DEEPLIMA_SRC_INFERENCE_EIGEN_BILSTM_H

#include <iostream>
#include <vector>
#include <eigen3/Eigen/Dense>

#include "op_base.h"

namespace deeplima
{
namespace eigen_impl
{

template<class M=Eigen::MatrixXf, class V=Eigen::VectorXf>
struct params_lstm_t : public param_base_t
{
  M weight_ih;
  M weight_hh;
  V bias_ih;
  V bias_hh;

  void init_from_double(const params_lstm_t<Eigen::MatrixXd, Eigen::VectorXd>& arg)
  {
    weight_ih = arg.weight_ih.cast<float>();
    weight_hh = arg.weight_hh.cast<float>();
    bias_ih = arg.bias_ih.cast<float>();
    bias_hh = arg.bias_hh.cast<float>();
  }

  std::ostream& operator<< (std::ostream& out) const
  {
    out << weight_ih << std::endl;
    out << weight_hh << std::endl;
    out << bias_ih << std::endl;
    out << bias_hh << std::endl;
    return out;
  }
};

template<class M=Eigen::MatrixXf, class V=Eigen::VectorXf>
struct params_bilstm_t : public param_base_t
{
  typedef M matrix_t;

  params_lstm_t<M, V> fw;
  params_lstm_t<M, V> bw;

  void init_from_double(const params_bilstm_t<Eigen::MatrixXd, Eigen::VectorXd>& arg)
  {
    fw.init_from_double(arg.fw);
    bw.init_from_double(arg.bw);
  }

  std::ostream& operator<< (std::ostream& out) const
  {
    out << fw << std::endl;
    out << bw << std::endl;
    return out;
  }
};

template<class M, class V, class T>
class Op_BiLSTM : public Op_Base
{
protected:
  struct workbench_t : public Op_Base::workbench_t
  {
    workbench_t(uint32_t input_size,
                uint32_t hidden_size,
                bool precomputed_input=false)
      : temp(M::Zero(precomputed_input ? 0 : hidden_size * 8, input_size)),
        out(M::Zero(hidden_size * 2, input_size)),
        zero(V::Zero(hidden_size)),
        m_precomputed_input(precomputed_input)
    {
    }

    virtual ~workbench_t() {}

    M temp;
    M out;
    V fw_h, fw_c, bw_h, bw_c;
    const V zero;
    bool m_precomputed_input;
  };

public:
  typedef V Vector;
  typedef params_bilstm_t<M, V> params_t;

  virtual workbench_t* create_workbench(uint32_t input_size, const param_base_t* params, bool precomputed_input=false) const
  {
    assert(input_size > 0);
    assert(nullptr != params);
    const params_bilstm_t<M, V>& layer = *static_cast<const params_t*>(params);

    return new workbench_t(input_size, layer.fw.weight_ih.rows() / 4, precomputed_input);
  }

  virtual bool supports_precomputing() const
  {
    return true;
  }

  virtual void precompute_inputs(const param_base_t* params, const M& inputs, M& outputs, int64_t first_column)
  {
    assert(nullptr != params);
    const params_bilstm_t<M, V>& layer = *static_cast<const params_t*>(params);
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
                         size_t input_begin,
                         size_t input_end,
                         Vector& fw_h,
                         Vector& fw_c,
                         Vector& bw_h,
                         Vector& bw_c)
  {
    assert(nullptr != pwb);
    assert(nullptr != params);
    const params_bilstm_t<M, V>& layer = *static_cast<const params_t*>(params);

    workbench_t* wb = static_cast<workbench_t*>(pwb);
    M& temp = wb->temp;
    M& output = wb->out;
    const V& zero = wb->zero;

    size_t hidden_size = layer.fw.weight_ih.rows() / 4;
    V c = V::Zero(hidden_size);

    M input = input_matrix.block(0, input_begin, input_matrix.rows(), input_end);
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
      temp.topLeftCorner(hidden_size * 4, input_end) = (layer.fw.weight_ih * input).colwise() + layer.fw.bias_ih;
      // Bottom rows - backward pass
      temp.bottomLeftCorner(hidden_size * 4, input_end) = (layer.bw.weight_ih * input).colwise() + layer.bw.bias_ih;
    }

    // Forward pass
    s = temp.col(0).topRows(hidden_size * 4) + layer.fw.bias_hh + layer.fw.weight_hh * zero;
    step_fw(hidden_size, 0, s, g_u, g_o, g_if, c, output);

    for (size_t t = 1; t < input.cols(); t++)
    {
      s = temp.col(t).topRows(hidden_size * 4) + layer.fw.bias_hh + layer.fw.weight_hh * output.col(t-1).topRows(hidden_size);
      step_fw(hidden_size, t, s, g_u, g_o, g_if, c, output);
    }

    int t = input.cols() - 1;
    //V c_fw = c;
    wb->fw_c = c;
    wb->fw_h = output.col(t).topRows(hidden_size);
    //t--;

    // Backward pass
    c = V::Zero(hidden_size);
    s = temp.col(t).bottomRows(hidden_size * 4) + layer.bw.bias_hh + layer.bw.weight_hh * zero;
    step_bw(hidden_size, t, s, g_u, g_o, g_if, c, output);

    for (t = input.cols() - 2; t >= 0; t--)
    {
      s = temp.col(t).bottomRows(hidden_size * 4) + layer.bw.bias_hh + layer.bw.weight_hh * output.col(t+1).bottomRows(hidden_size);
      step_bw(hidden_size, t, s, g_u, g_o, g_if, c, output);
    }
    wb->bw_c = c;
    wb->bw_h = output.col(0).bottomRows(hidden_size);

    fw_h = wb->fw_h;
    fw_c = wb->fw_c;
    bw_h = wb->bw_h;
    bw_c = wb->bw_c;

    return 0;
  }

protected:
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
