// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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

template<class M=Eigen::MatrixXf, class V=Eigen::VectorXf>
struct params_multilayer_bilstm_t : public param_base_t
{
  typedef M matrix_t;
  typedef params_bilstm_t<M, V> layer_params_t;

  std::vector<layer_params_t> layers;

  params_multilayer_bilstm_t() {}
  params_multilayer_bilstm_t(const std::vector<layer_params_t>& arg)
    : layers(arg)
  {}

  void init_from_double(const params_bilstm_t<Eigen::MatrixXd, Eigen::VectorXd>& arg)
  {
    layers.resize(1);
    layers[0].fw.init_from_double(arg.fw);
    layers[0].bw.init_from_double(arg.bw);
  }

  std::ostream& operator<< (std::ostream& out) const
  {
    for (size_t i = 0; i < layers.size(); ++i)
    {
      out << layers[i].fw << std::endl;
      out << layers[i].bw << std::endl;
    }
    return out;
  }
};

template<class M, class V, class T>
class Op_BiLSTM : public Op_Base
{
public:
  typedef params_multilayer_bilstm_t<M, V> params_t;

  struct workbench_t : public Op_Base::workbench_t
  {
    workbench_t(const params_t& p, uint32_t input_size, bool precomputed_input)
      : temps(p.layers.size()),
        outputs(p.layers.size()),
        zeros(p.layers.size()),
        fw_h(p.layers.size()),
        fw_c(p.layers.size()),
        bw_h(p.layers.size()),
        bw_c(p.layers.size()),
        m_precomputed_input(precomputed_input)
    {
      for (size_t i = 0; i < p.layers.size(); ++i)
      {
        uint32_t hidden_size = p.layers[i].fw.weight_ih.rows() / 4;
        temps[i] = M::Zero((0 == i && precomputed_input) ? 0 : hidden_size * 8, input_size);
        outputs[i] = M::Zero(hidden_size * 2, input_size);
        zeros[i] = V::Zero(hidden_size);
        fw_h[i] = V::Zero(hidden_size);
        fw_c[i] = V::Zero(hidden_size);
        bw_h[i] = V::Zero(hidden_size);
        bw_c[i] = V::Zero(hidden_size);
      }
    }

    const M& get_last_output()
    {
      return outputs[outputs.size() - 1];
    }

    virtual ~workbench_t() {}

    std::vector<M> temps;
    std::vector<M> outputs;
    std::vector<V> fw_h, fw_c, bw_h, bw_c;
    std::vector<V> zeros;
    bool m_precomputed_input;
  };

public:
  typedef V Vector;

  virtual workbench_t* create_workbench(uint32_t input_size, const param_base_t* params, bool precomputed_input=false) const
  {
    assert(input_size > 0);
    assert(nullptr != params);
    const params_multilayer_bilstm_t<M, V>& p = *static_cast<const params_t*>(params);

    //return new workbench_t(input_size, p.layers[0].fw.weight_ih.rows() / 4, precomputed_input);
    return new workbench_t(p, input_size, precomputed_input);
  }

  virtual bool supports_precomputing() const
  {
    return true;
  }

  virtual void precompute_inputs(const param_base_t* params, const M& inputs, M& outputs, int64_t first_column)
  {
    assert(nullptr != params);
    const params_t& p = *static_cast<const params_t*>(params);
    const typename params_t::layer_params_t& layer = p.layers[0];

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
    const params_t& p = *static_cast<const params_t*>(params);
    const typename params_t::layer_params_t& layer = p.layers[0];

    workbench_t* wb = static_cast<workbench_t*>(pwb);
    //M& temp = wb->temps[0];
    M& output = wb->outputs[0];
    const V& zero = wb->zeros[0];

    size_t hidden_size = layer.fw.weight_ih.rows() / 4;
    V c = fw_c; //V::Zero(hidden_size);

    M input = input_matrix.block(0, input_begin, input_matrix.rows(), input_end);
    // TODO: rewrite this!!!

    V s, g_u, g_o, g_if;
    M temp = M::Zero(hidden_size * 8, input_end - input_begin);

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
    s = temp.col(0).topRows(hidden_size * 4) + layer.fw.bias_hh + layer.fw.weight_hh * fw_h;
    step_fw(hidden_size, 0, s, g_u, g_o, g_if, c, output);

    for (size_t t = 1; t < input.cols(); t++)
    {
      s = temp.col(t).topRows(hidden_size * 4) + layer.fw.bias_hh + layer.fw.weight_hh * output.col(t-1).topRows(hidden_size);
      step_fw(hidden_size, t, s, g_u, g_o, g_if, c, output);
    }

    int t = input.cols() - 1;
    //V c_fw = c;
    wb->fw_c[0] = c;
    wb->fw_h[0] = output.col(t).topRows(hidden_size);
    //t--;

    // Backward pass
    c = bw_c; //V::Zero(hidden_size);
    s = temp.col(t).bottomRows(hidden_size * 4) + layer.bw.bias_hh + layer.bw.weight_hh * bw_h; //zero;
    step_bw(hidden_size, t, s, g_u, g_o, g_if, c, output);

    for (t = input.cols() - 2; t >= 0; t--)
    {
      s = temp.col(t).bottomRows(hidden_size * 4) + layer.bw.bias_hh + layer.bw.weight_hh * output.col(t+1).bottomRows(hidden_size);
      step_bw(hidden_size, t, s, g_u, g_o, g_if, c, output);
    }
    wb->bw_c[0] = c;
    wb->bw_h[0] = output.col(0).bottomRows(hidden_size);

    fw_h = wb->fw_h[0];
    fw_c = wb->fw_c[0];
    bw_h = wb->bw_h[0];
    bw_c = wb->bw_c[0];

    return 0;
  }

  virtual size_t execute(Op_Base::workbench_t* pwb,
                         const M& input_matrix,
                         const param_base_t* params,
                         size_t input_begin,
                         size_t input_end)
  {
    assert(nullptr != pwb);
    assert(nullptr != params);
    const params_t& p = *static_cast<const params_t*>(params);
    workbench_t* wb = static_cast<workbench_t*>(pwb);

    for (size_t i = 0; i < p.layers.size(); ++i)
    {
      V s, g_u, g_o, g_if;
      M& temp = wb->temps[i]; //.leftCols(input_end - input_begin);
      M& output = wb->outputs[i]; //.leftCols(input_end - input_begin);
      const typename params_t::layer_params_t& layer = p.layers[i];

      size_t hidden_size = layer.fw.weight_ih.rows() / 4;
      V c = V::Zero(hidden_size);
      const V& zero = wb->zeros[i];

      if (0 == i && wb->m_precomputed_input)
      {
        // precomputed inputs
        temp = input_matrix.block(0, input_begin, input_matrix.rows(), input_end);
      }
      else
      {
        if (0 == i)
        {
          //const M input = input_matrix.block(0, input_begin, input_matrix.rows(), input_end);
          // Top rows - forward pass
          temp.topLeftCorner(hidden_size * 4, input_end - input_begin)
              = (layer.fw.weight_ih * input_matrix.block(0, input_begin, input_matrix.rows(), input_end)).colwise()
                + layer.fw.bias_ih;

          // Bottom rows - backward pass
          temp.bottomLeftCorner(hidden_size * 4, input_end - input_begin)
              = (layer.bw.weight_ih * input_matrix.block(0, input_begin, input_matrix.rows(), input_end)).colwise()
                + layer.bw.bias_ih;
        }
        else
        {
          //const M input = wb->outputs[i-1].leftCols(input_end - input_begin);
          // Top rows - forward pass
          temp.topLeftCorner(hidden_size * 4, input_end - input_begin)
              = (layer.fw.weight_ih * wb->outputs[i-1].leftCols(input_end - input_begin)).colwise()
                + layer.fw.bias_ih;
          // Bottom rows - backward pass
          temp.bottomLeftCorner(hidden_size * 4, input_end - input_begin)
              = (layer.bw.weight_ih * wb->outputs[i-1].leftCols(input_end - input_begin)).colwise()
                + layer.bw.bias_ih;
        }
      }

      // Forward pass
      forward_pass(hidden_size, layer.fw, temp, s, g_u, g_o, g_if, c, output, zero, input_begin, input_end);
      wb->fw_c[i] = c;
      wb->fw_h[i] = output.col(input_end - input_begin - 1).topRows(hidden_size);

      // Backward pass
      c = V::Zero(hidden_size);
      backward_pass(hidden_size, layer.bw, temp, s, g_u, g_o, g_if, c, output, zero, input_begin, input_end);
      wb->bw_c[i] = c;
      wb->bw_h[i] = output.col(input_begin).bottomRows(hidden_size);
    }

    return 0;
  }


protected:
  inline void forward_pass(
        size_t hidden_size,             // [in]     LSTM parameter
        const params_lstm_t<M, V>& fw,  // [in]     forward LSTM params
        M& input,                       // [in]     precomputed input
        V& s,                           // [in]     result before gating
        V& g_u,                         // [temp]   update gate
        V& g_o,                         // [temp]   output gate
        V& g_if,                        // [temp]   input and forget gates
        V& c,                           // [in/out] cell state
        M& output,                      // [out]    matrix of output states
        const V& initial_h,             // [in]     initial cell hidden state
        int begin,                      // [in]     first column
        int end                         // [in]     the column after the last one
      )
  {
    s = input.col(begin).topRows(hidden_size * 4) + fw.bias_hh + fw.weight_hh * initial_h;
    step_fw(hidden_size, 0, s, g_u, g_o, g_if, c, output);

    for (size_t t = begin + 1; t < end; t++)
    {
      s = input.col(t).topRows(hidden_size * 4) + fw.bias_hh + fw.weight_hh * output.col(t-1).topRows(hidden_size);
      step_fw(hidden_size, t, s, g_u, g_o, g_if, c, output);
    }
  }

  inline void backward_pass(
        size_t hidden_size,             // [in]     LSTM parameter
        const params_lstm_t<M, V>& bw,  // [in]     backward LSTM params
        M& input,                       // [in]     precomputed input
        V& s,                           // [in]     result before gating
        V& g_u,                         // [temp]   update gate
        V& g_o,                         // [temp]   output gate
        V& g_if,                        // [temp]   input and forget gates
        V& c,                           // [in/out] cell state
        M& output,                      // [out]    matrix of output states
        const V& initial_h,             // [in]     initial cell hidden state
        int begin,                      // [in]     first column
        int end                         // [in]     the column after the last one
      )
  {
    int t = end - 1;
    s = input.col(t).bottomRows(hidden_size * 4) + bw.bias_hh + bw.weight_hh * initial_h;
    step_bw(hidden_size, t, s, g_u, g_o, g_if, c, output);

    for (t = end - 2; t >= begin; t--)
    {
      s = input.col(t).bottomRows(hidden_size * 4) + bw.bias_hh + bw.weight_hh * output.col(t+1).bottomRows(hidden_size);
      step_bw(hidden_size, t, s, g_u, g_o, g_if, c, output);
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
