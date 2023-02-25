// Copyright 2002-2022 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_SRC_INFERENCE_EIGEN_LSTM_BEAM_DECODER_H
#define DEEPLIMA_SRC_INFERENCE_EIGEN_LSTM_BEAM_DECODER_H

#include <vector>
#include <algorithm>

#include "bilstm.h"
#include "linear.h"

#include "embd_dict.h"

//#define BEAM_DECODER_PROFILE
#ifdef BEAM_DECODER_PROFILE
#include <chrono>
#endif

namespace deeplima
{
namespace eigen_impl
{

template<class M, class V>
struct params_lstm_beam_decoder_t : public param_base_t
{
  params_lstm_t<M, V> lstm;
  params_linear_t<M, V> linear;
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
        lin_out(V::Zero(output_sizes[0])),
        zero(V::Zero(hidden_size)),
        temp_indicies(output_sizes[0]),
        m_precomputed_input(precomputed_input)
    {
    }

    virtual ~workbench_t() {}

    M temp;
    M out;
    M states_c, states_h;
    V s;
    V lin_out;
    const V zero;
    std::vector<uint32_t> initial_top_classes;
    std::vector<float> initial_logprob;
    std::vector<uint32_t> top_classes;
    std::vector<float> logprob;
    std::vector<uint32_t> indices;
    std::vector<uint32_t> temp_indicies;
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

  virtual std::shared_ptr<Op_Base::workbench_t> create_workbench(uint32_t /*input_size*/,
                                                                 const std::shared_ptr<param_base_t> params,
                                                                 bool precomputed_input=false) const override
  {
    assert(nullptr != params);
    const auto& layer = std::dynamic_pointer_cast<const params_t>(params)->lstm;
    const auto& linear = std::dynamic_pointer_cast<const params_t>(params)->linear;

    std::vector<uint32_t> output_sizes = { linear.weight.rows() };
    return std::make_shared<workbench_t>(layer.weight_ih.rows() / 4, output_sizes, precomputed_input);
  }

  virtual bool supports_precomputing() const
  {
    return true;
  }

  virtual void precompute_inputs(const std::shared_ptr<param_base_t> params, const M& inputs, M& outputs, int64_t first_column)
  {
    assert(nullptr != params);
    const auto& layer = std::dynamic_pointer_cast<const params_t>(params)->lstm;

    const V biases = layer.bias_ih + layer.bias_hh;
    outputs.block(0, first_column, outputs.rows(), inputs.cols()) = (layer.weight_ih * inputs).colwise() + biases;
  }

  virtual size_t execute(std::shared_ptr<Op_Base::workbench_t> pwb,
                         const EmbdUInt64Float& embd,
                         const V& initial_state_h,
                         const V& initial_state_c,
                         const std::shared_ptr<param_base_t> params,
                         const uint32_t start_code,
                         const uint32_t stop_code,
                         const size_t beam_size,
                         std::vector<uint32_t>& output_seq,
                         const size_t max_output_len
      )
  {
#ifdef BEAM_DECODER_PROFILE
    using clock = std::chrono::system_clock;
    using ms = std::chrono::duration<double, std::milli>;

    auto before = clock::now();
#endif

    assert(nullptr != pwb);
    assert(nullptr != params);
    const auto& layer = std::dynamic_pointer_cast<const params_t>(params)->lstm;
    const auto& linear = std::dynamic_pointer_cast<const params_t>(params)->linear;
    EmbdUInt64Float::index_t stop_idx = embd.lookup(stop_code);
    assert(stop_idx != 0);

    auto wb = std::dynamic_pointer_cast<workbench_t>(pwb);
    M& output = wb->out;
    V& lin_out = wb->lin_out;
    std::vector<uint32_t>& temp_indicies = wb->temp_indicies;

    std::vector<std::vector<decoding_timepoint_t>> decoding_log(max_output_len, std::vector<decoding_timepoint_t>(beam_size));

    size_t hidden_size = layer.weight_ih.rows() / 4;
    V& s = wb->s;
    V c = initial_state_c;
    V h = initial_state_h;

    Eigen::Ref<const M> input = embd.get_ref_by_key(start_code);

    std::vector<uint32_t>& initial_top_classes = wb->initial_top_classes;
    initial_top_classes.resize(beam_size);
    std::vector<float>&  initial_logprob = wb->initial_logprob;
    initial_logprob.resize(beam_size);

#ifdef BEAM_DECODER_PROFILE
    auto start_decoding = clock::now();
#endif

    step_with_decoding(layer, linear, input, output, s, lin_out, hidden_size, beam_size, 0,
                       initial_top_classes, temp_indicies, initial_logprob, c, h);

#ifdef BEAM_DECODER_PROFILE
    auto after_first_step = clock::now();
#endif

    size_t decoding_step = 0;
    for (size_t i = 0; i < beam_size; ++i)
    {
      decoding_log[decoding_step][i].cls = initial_top_classes[i];
    }
    decoding_step++;

    M& states_c = wb->states_c;
    if (states_c.cols() != beam_size)
      states_c = M::Zero(hidden_size, beam_size);
    for (size_t i = 0; i < beam_size; ++i) states_c.col(i) = c;
    M& states_h = wb->states_h;
    if (states_h.cols() != beam_size)
      states_h = M::Zero(hidden_size, beam_size);
    for (size_t i = 0; i < beam_size; ++i) states_h.col(i) = h;

    std::vector<uint32_t>& top_classes = wb->top_classes;
    top_classes.resize(beam_size * beam_size);
    std::vector<float>& logprob = wb->logprob;
    logprob.resize(beam_size * beam_size);
    std::vector<uint32_t>& indices = wb->indices;
    indices.resize(beam_size * beam_size);

#ifdef BEAM_DECODER_PROFILE
    auto start_next_step = clock::now();
#endif

    while (decoding_step < max_output_len)
    {
      size_t pos = 0;
      for (size_t i = 0; i < beam_size; ++i)
      {
        if (stop_idx == decoding_log[decoding_step - 1][i].cls)
        {
          continue;
        }
        //embd.get_direct(decoding_log[decoding_step - 1][i].cls, input, 0, 0);
        Eigen::Ref<const M> input = embd.get_ref_by_idx(decoding_log[decoding_step - 1][i].cls);
        step_with_decoding(layer, linear, input, output, s, lin_out,
                           hidden_size, beam_size,
                           pos, /*i * beam_size,*/
                           top_classes, temp_indicies, logprob, states_c, states_h, i);
        for (size_t j = 0; j < beam_size; ++j)
        {
          logprob[pos /*i * beam_size*/ + j] += initial_logprob[i];
        }
        pos += beam_size;
      }

      for (uint32_t i = 0; i < pos; ++i) indices[i] = i;
      std::sort(indices.begin(), indices.begin() + pos, [&logprob](size_t a, size_t b){
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

      size_t non_eos = 0;
      for (size_t i = 0; i < std::min(beam_size, pos); ++i)
      {
        size_t idx = indices[i];
        decoding_log[decoding_step][i].cls = top_classes[idx];
        if (decoding_log[decoding_step][i].cls != stop_idx)
        {
          non_eos++;
        }
        decoding_log[decoding_step][i].prev_pos = idx % beam_size;
        initial_logprob[i] = logprob[idx];
      }

      decoding_step++;

      if (0 == non_eos)
      {
        break; // all beam_size hypotheses gave EOS
      }

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

#ifdef BEAM_DECODER_PROFILE
    auto start_backtracking = clock::now();
#endif

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

#ifdef BEAM_DECODER_PROFILE
    auto end_backtracking = clock::now();

    std::cerr << "  prepare      : " << (start_decoding - before).count() << std::endl;
    std::cerr << "  first step   : " << (after_first_step - start_decoding).count() << std::endl;
    std::cerr << "  prepare      : " << (start_next_step - after_first_step).count() << std::endl;
    std::cerr << "  all steps    : " << (start_backtracking - start_next_step).count() << std::endl;
    std::cerr << "  backtracking : " << (end_backtracking - start_backtracking).count() << std::endl;
#endif

    return 0;
  }

protected:

  inline void step_with_decoding(
        const params_lstm_t<M, V>& layer,     // [in]
        const params_linear_t<M, V>& linear,  // [in]
        Eigen::Ref<const M>& input,           // [in]
        M& output,                            // [temp]
        V& s,                                 // [temp]
        V& temp,                              // [temp]
        const size_t hidden_size,             // [in]
        const size_t beam_size,               // [in]
        const size_t start_pos,               // [in]
        std::vector<uint32_t>& top_classes,   // [out]
        std::vector<uint32_t> indices,        // [temp]
        std::vector<float>& logprob,          // [out]
        V& c,                                 // [in/out]
        V& h                                  // [in/out]
      )
  {
    // Forward pass
    s = input.col(0);
    s.noalias() += layer.weight_hh * h;

    step_fw(hidden_size, 0, s, c, output);
    h = output.col(0).topRows(hidden_size);

    linear_and_decoding(linear, output, temp, beam_size, start_pos, top_classes, indices, logprob);
  }

  inline void step_with_decoding(
        const params_lstm_t<M, V>& layer,     // [in]
        const params_linear_t<M, V>& linear,  // [in]
        Eigen::Ref<const M>& input,           // [in]
        M& output,                            // [temp]
        V& s,                                 // [in]
        V& temp,                              // [temp]
        const size_t hidden_size,             // [in]
        const size_t beam_size,               // [in]
        const size_t start_pos,               // [in]
        std::vector<uint32_t>& top_classes,   // [out]
        std::vector<uint32_t> indices,        // [temp]
        std::vector<float>& logprob,          // [out]
        M& states_c,                          // [in/out]
        M& states_h,                          // [in/out]
        const size_t beam_idx                 // [in]
      )
  {
    // Forward pass
    s = input.col(0);
    s.noalias() += layer.weight_hh * states_h.col(beam_idx);

    step_fw(hidden_size, 0, s, states_c, beam_idx, output);
    states_h.col(beam_idx) = output.col(0).topRows(hidden_size);

    linear_and_decoding(linear, output, temp, beam_size, start_pos, top_classes, indices, logprob);
  }

  inline void linear_and_decoding(
        const params_linear_t<M, V>& linear,  // [in]
        const M& output,                      // [in]
        V& temp,                              // [temp]
        const size_t beam_size,               // [in]
        const size_t start_pos,               // [in]
        std::vector<uint32_t>& top_classes,   // [out]
        std::vector<uint32_t> indices,        // [temp]
        std::vector<float>& logprob           // [out]
      )
  {
    // Linear
    temp = linear.bias;
    temp.noalias() += linear.weight * output;
    Eigen::Index idx = 0;
    typename V::Scalar max_value = temp.maxCoeff(&idx);
    //Eigen::ArrayXf all_logprob = lin_out.array() - (max_value + Eigen::exp(lin_out.array() - max_value).sum());
    temp.array() -= (max_value + Eigen::exp(temp.array() - max_value).sum());
    for (uint32_t i = 0; i < indices.size(); ++i) indices[i] = i;

    std::partial_sort_copy(indices.begin(), indices.end(), top_classes.begin() + start_pos, top_classes.begin() + start_pos + beam_size,
                           [&temp](uint32_t a, uint32_t b) {
      return temp[a] > temp[b];
    });

    for (size_t i = 0; i < beam_size; ++i)
    {
      logprob[start_pos + i] = temp[top_classes[start_pos + i]];
      /*
      std::cerr << i << " " << top_classes[start_pos + i]
                     << " " << all_logprob[top_classes[start_pos + i]]
                     << " " << lin_out[top_classes[start_pos + i]] << std::endl;
      */
    }
  }

  #define TANH(x)    ( T(2) / ( T(1) + Eigen::exp( T(-2) * (x).array() ) ) - T(1) )
  #define SIGMOID(x) ( T(1) / ( T(1) + Eigen::exp( - (x).array() ) ) )

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

  inline void update_c(
        const size_t hidden_size,  // [in]     the size of the hidden state
        const V& s,                // [in]     the results before gating
        M& c,                      // [in/out] the cell state
        const size_t beam_idx
      )
  {
    // c = c * forget_gate = c * 1 / sigmoid(...)
    c.col(beam_idx) = c.col(beam_idx).array() / (1 + Eigen::exp( - s.segment(hidden_size, hidden_size).array() ) );

    // c = c + input_gate * update_gate
    c.col(beam_idx) += ( SIGMOID(s.segment(0, hidden_size)).array() * TANH(s.segment(hidden_size * 2, hidden_size)).array() ).matrix();
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

  inline void step_fw(
        const size_t hidden_size,  // [in]     the size of the hidden state
        const size_t t,            // [in]     step (the position in output)
        const V& s,                // [in]     the results before gating
        M& c,                      // [in/out] the cell state
        const size_t beam_idx,
        M& output                  // [out]    the matrix of output states
      )
  {
    update_c(hidden_size, s, c, beam_idx);

    // output = output_gate * tanh(c)
    output.col(t).topRows(hidden_size).noalias() = (SIGMOID(s.segment(hidden_size * 3, hidden_size)).array() * TANH(c.col(beam_idx)).array()).matrix();
  }
};

} // namespace eigen_impl
} // namespace deeplima

#endif
