// Copyright 2002-2022 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_SRC_INFERENCE_EIGEN_LSTM_BEAM_DECODER_H
#define DEEPLIMA_SRC_INFERENCE_EIGEN_LSTM_BEAM_DECODER_H

#include <vector>

#include "bilstm.h"
#include "linear.h"

#include "embd_dict.h"

namespace deeplima
{
namespace eigen_impl
{

template<class M, class V>
struct params_lstm_beam_decoder_t : public param_base_t
{
  params_bilstm_t<M, V> bilstm;
  std::vector<params_linear_t<M, V>> linear;
};

template<class M, class V, class T>
class Op_LSTM_Beam_Decoder : public Op_Base
{
protected:
  struct workbench_t : public Op_Base::workbench_t
  {
    workbench_t(uint32_t hidden_size,
                const std::vector<uint32_t>& output_sizes,
                bool precomputed_input=false)
      : temp(M::Zero(precomputed_input ? 0 : hidden_size * 4, 1)),
        out(M::Zero(hidden_size, 1)),
        zero(V::Zero(hidden_size)),
        m_precomputed_input(precomputed_input)
    {
      lin_out.resize(output_sizes.size());
      for (size_t i = 0; i < output_sizes.size(); i++)
      {
        lin_out[i] = M::Zero(output_sizes[i], 1);
      }
    }

    virtual ~workbench_t() {}

    M temp;
    M out;
    std::vector<M> lin_out;
    const V zero;
    bool m_precomputed_input;
  };

  struct decoding_timepoint_t
  {
    size_t prev_pos;
    uint32_t cls;

    decoding_timepoint_t() : prev_pos(0), cls(0) { }
  };

public:

  typedef params_lstm_beam_decoder_t<M, V> params_t;

  virtual workbench_t* create_workbench(uint32_t /*input_size*/, const param_base_t* params,
                                        bool precomputed_input=false) const
  {
    assert(nullptr != params);
    const params_bilstm_t<M, V>& layer = static_cast<const params_t*>(params)->bilstm;
    const std::vector<params_linear_t<M, V>>& linear = static_cast<const params_t*>(params)->linear;

    std::vector<uint32_t> output_sizes;
    output_sizes.reserve(linear.size());
    for ( const auto& p : linear )
    {
      output_sizes.push_back(p.weight.rows());
    }
    return new workbench_t(layer.fw.weight_ih.rows() / 4, output_sizes, precomputed_input);
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
                         const EmbdUInt64Float& embd,
                         const V& initial_state_h,
                         const V& initial_state_c,
                         const param_base_t* params,
                         uint32_t start_code,
                         size_t beam_size,
                         std::vector<uint32_t>& output_seq,
                         size_t max_output_len)
  {
    assert(nullptr != pwb);
    assert(nullptr != params);
    const params_bilstm_t<M, V>& layer = static_cast<const params_t*>(params)->bilstm;
    const params_linear_t<M, V>& linear = static_cast<const params_t*>(params)->linear[0];

    workbench_t* wb = static_cast<workbench_t*>(pwb);
    M& temp = wb->temp;
    M& output = wb->out;
//     TODO should it be used?
    // const V& zero = wb->zero;

    std::vector<std::vector<decoding_timepoint_t>> decoding_log(max_output_len, std::vector<decoding_timepoint_t>(beam_size));

    size_t hidden_size = layer.fw.weight_ih.rows() / 4;
    V s, g_u, g_o, g_if;
    V c = initial_state_c;
    V h = initial_state_h;

    M input(embd.dim(), 1); //= input_matrix.block(0, input_begin, input_matrix.rows(), temp.cols());
    embd.get(start_code, input, 0, 0);

    std::vector<uint32_t> initial_top_classes(beam_size);
    std::vector<float> initial_logprob(beam_size);
    step_with_decoding(layer, linear, input, output, s, g_u, g_o, g_if, temp, hidden_size, beam_size, 0,
                       initial_top_classes, initial_logprob, c, h);

    size_t decoding_step = 0;
    for (size_t i = 0; i < beam_size; ++i)
    {
      decoding_log[decoding_step][i].cls = initial_top_classes[i];
    }
    decoding_step++;

    M states_c(hidden_size, beam_size);
    for (size_t i = 0; i < beam_size; ++i) states_c.col(i) = c;
    M states_h(hidden_size, beam_size);
    for (size_t i = 0; i < beam_size; ++i) states_h.col(i) = h;

    while (decoding_step < max_output_len)
    {
      std::vector<uint32_t> top_classes(beam_size * beam_size);
      std::vector<float> logprob(beam_size * beam_size);
      for (size_t i = 0; i < beam_size; ++i)
      {
        V tc = states_c.col(i);
        V th = states_h.col(i);
        embd.get_direct(decoding_log[decoding_step - 1][i].cls, input, 0, 0);
        step_with_decoding(layer, linear, input, output, s, g_u, g_o, g_if, temp,
                           hidden_size, beam_size,
                           i * beam_size,
                           top_classes, logprob, tc, th); //, states_h.col(i));
        states_c.col(i) = tc;
        states_h.col(i) = th;
        for (size_t j = 0; j < beam_size; ++j)
        {
          logprob[i * beam_size + j] += initial_logprob[i];
        }
      }

      std::vector<uint32_t> indices(beam_size * beam_size);
      for (uint32_t i = 0; i < beam_size * beam_size; ++i) indices[i] = i;
      std::sort(indices.begin(), indices.end(), [&logprob](size_t a, size_t b){
        return logprob[a] > logprob[b];
      });

      /*
      for (size_t i = 0; i < beam_size * beam_size; ++i)
      {
        size_t idx = indices[i];
        std::cerr << i << " " << idx << " "
                     << initial_top_classes[idx % beam_size] << " " << top_classes[idx]
                     << " "
                     << initial_logprob[idx % beam_size] << " " << logprob[idx] << std::endl;
      }
      */

      for (size_t i = 0; i < beam_size; ++i)
      {
        size_t idx = indices[i];
        size_t ref = idx % beam_size;
        states_c.col(i) = states_c.col(ref);
        states_h.col(i) = states_h.col(ref);
      }

      for (size_t i = 0; i < beam_size; ++i)
      {
        size_t idx = indices[i];
        decoding_log[decoding_step][i].cls = top_classes[idx];
        decoding_log[decoding_step][i].prev_pos = idx % beam_size;
        initial_logprob[i] = logprob[idx];
      }

      decoding_step++;

      //std::cerr << std::endl;
    }

    /*
    for (size_t p = 0; p < beam_size; ++p)
    {
      for (size_t step = 0; step < decoding_log.size(); ++step)
      {
        char32_t ch = embd.decode(decoding_log[step][p].cls);
        std::cerr << decoding_log[step][p].prev_pos
                  << ":"
                  << decoding_log[step][p].cls
                  << ":"
                  << (char)ch
                  << " ";

      }
      std::cerr << std::endl;
    }

    std::cerr << std::endl;
    */

    // Backtracking
    size_t step = 0;
    size_t pos = 0;
    while (step < decoding_log.size() && decoding_log[step][pos].cls != 1) step++;

    if (step > 0)
    {
      step--;
    }

    while (true)
    {
      char32_t ch = embd.decode(decoding_log[step][pos].cls);
      output_seq.push_back(ch);
      pos = decoding_log[step][pos].prev_pos;
      if (step == 0) // break out here because step is unsigned int
        break;
      step--;
    }
    std::reverse(output_seq.begin(), output_seq.end());

    return 0;
  }

protected:

  inline void step_with_decoding(
        const params_bilstm_t<M, V>& layer,
        const params_linear_t<M, V>& linear,
        const M& input,                      // [in]
        M& output,                           // [temp]
        V& s,                                // [in]     result before gating
        V& g_u,                              // [temp]   update gate
        V& g_o,                              // [temp]   output gate
        V& g_if,                             // [temp]   input and forget gates
        M& temp,                             // [temp]
        size_t hidden_size,                  // [in]
        size_t beam_size,                    // [in]
        size_t start_pos,                    // [in]
        std::vector<uint32_t>& top_classes,  // [out]
        std::vector<float>& logprob,         // [out]
        V& c,                                // [in/out]
        V& h                                 // [in/out]
      )
  {
    // Top rows - forward pass
    temp.topRows(hidden_size * 4) = (layer.fw.weight_ih * input).colwise() + layer.fw.bias_ih;

    // Forward pass
    s = temp.col(0).topRows(hidden_size * 4) + layer.fw.bias_hh + layer.fw.weight_hh * h;
    step_fw(hidden_size, 0, s, g_u, g_o, g_if, c, output);
    h = output.col(0).topRows(hidden_size);

    // Linear
    V lin_out = (linear.weight * output).colwise() + linear.bias;
    Eigen::Index idx = 0;
    typename V::Scalar max_value = lin_out.maxCoeff(&idx);
    auto d1 = lin_out.array() - max_value;
    auto d2 = Eigen::exp(d1);
    typename V::Scalar d3 = d2.sum();
    typename V::Scalar d4 = max_value + d3;
    Eigen::ArrayXf all_logprob = lin_out.array() - d4;
    std::vector<uint32_t> indices(lin_out.rows());
    for (uint32_t i = 0; i < indices.size(); ++i) indices[i] = i;

    std::partial_sort_copy(indices.begin(), indices.end(), top_classes.begin() + start_pos, top_classes.begin() + start_pos + beam_size,
                           [&all_logprob](uint32_t a, uint32_t b) {
      return all_logprob[a] > all_logprob[b];
    });

    for (size_t i = 0; i < beam_size; ++i)
    {
      logprob[start_pos + i] = all_logprob[top_classes[start_pos + i]];
      /*
      std::cerr << i << " " << top_classes[start_pos + i]
                     << " " << all_logprob[top_classes[start_pos + i]]
                     << " " << lin_out[top_classes[start_pos + i]] << std::endl;
      */
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
