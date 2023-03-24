// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_SRC_INFERENCE_EIGEN_BILSTM_AND_DENSE_H
#define DEEPLIMA_SRC_INFERENCE_EIGEN_BILSTM_AND_DENSE_H

#include <vector>
#include <type_traits>

#include "bilstm.h"
#include "linear.h"

//#define BILSTM_AND_DENSE_PROFILE
//#define MATMUL_WITH_FACTORIZATION

#ifdef BILSTM_AND_DENSE_PROFILE
#include <chrono>
#endif

namespace deeplima
{
namespace eigen_impl
{
template<typename M,                // Basic matrix type
         typename V,                // Basic vector type
         typename T,                // Basic scalar type,
         typename AuxScalar=float,  // Input scalar type:
                                    // - float   - for normal arithmetics
                                    // - int16_t - for fixed point operations where possible
         uint8_t IFracBits=4,       // Fraction bits in input data (used if AuxScalar is integer)
         uint8_t WFracBits=4        // Fraction bits in weights_hh (used if AuxScalar is integer)
         >
class Op_BiLSTM_Dense_ArgMax : public Op_Base
{
protected:

  // For fixed point calculations (used only if AusScalar is integer)
  static_assert(std::is_signed_v<AuxScalar>);
  typedef AuxScalar fixed_point_t;
  using MatrixWeight = typename std::conditional<std::is_integral_v<AuxScalar> && std::is_signed_v<AuxScalar>,
                                                 Eigen::Matrix<fixed_point_t, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>,
                                                 M>::type;
  using MatrixInput = typename std::conditional<std::is_integral_v<AuxScalar> && std::is_signed_v<AuxScalar>,
                                                Eigen::Matrix<fixed_point_t, Eigen::Dynamic, Eigen::Dynamic, Eigen::ColMajor>,
                                                M>::type;

  static constexpr fixed_point_t WEIGHT_FRACTION_MULT = fixed_point_t(1) << WFracBits;
  static constexpr fixed_point_t DATA_FRACTION_MULT = fixed_point_t(1) << IFracBits;
  static constexpr fixed_point_t WEIGHT_DATA_FRACTION_MULT = fixed_point_t(1) << (WFracBits + IFracBits);
  // End of fixed point releated definitions

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
  struct params_bilstm_dense_argmax_t : public param_base_t
  {
    params_bilstm_t<M, V> bilstm;
    std::vector<params_linear_t<M, V>> linear;

#ifdef MATMUL_WITH_FACTORIZATION
    struct multiplier_t
    {
      Eigen::PartialPivLU<M> matmul_input;
      Eigen::PartialPivLU<M> matmul_forget;
      Eigen::PartialPivLU<M> matmul_update;
      Eigen::PartialPivLU<M> matmul_output;
    };

    multiplier_t mul_fw;
    multiplier_t mul_bw;

    bool precompute()
    {
      std::cerr << "fw weights size: " << bilstm.fw.weight_hh.rows() << " x " << bilstm.fw.weight_hh.cols() << std::endl;

      size_t hidden_size = bilstm.fw.weight_hh.cols();
      std::cerr << "precompute(fw.input):" << std::endl;
      // /*
      mul_fw.matmul_input = bilstm.fw.weight_hh.block(0, 0, hidden_size, hidden_size).inverse().partialPivLu();
      mul_fw.matmul_forget = bilstm.fw.weight_hh.block(hidden_size, 0, hidden_size, hidden_size).inverse().partialPivLu();
      mul_fw.matmul_update = bilstm.fw.weight_hh.block(hidden_size*2, 0, hidden_size, hidden_size).inverse().partialPivLu();
      mul_fw.matmul_output = bilstm.fw.weight_hh.block(hidden_size*3, 0, hidden_size, hidden_size).inverse().partialPivLu();
      // */

      hidden_size = bilstm.bw.weight_hh.cols();
      std::cerr << "precompute(bw.input):" << std::endl;
      // /*
      mul_bw.matmul_input = bilstm.bw.weight_hh.block(0, 0, hidden_size, hidden_size).inverse().partialPivLu();
      mul_bw.matmul_forget = bilstm.bw.weight_hh.block(hidden_size, 0, hidden_size, hidden_size).inverse().partialPivLu();
      mul_bw.matmul_update = bilstm.bw.weight_hh.block(hidden_size*2, 0, hidden_size, hidden_size).inverse().partialPivLu();
      mul_bw.matmul_output = bilstm.bw.weight_hh.block(hidden_size*3, 0, hidden_size, hidden_size).inverse().partialPivLu();
      // */

      std::cerr << "end of precomputing" << std::endl;
      return true;
    }
#else
    // These two matrices of weights are used (and filled) only if we are going
    // to use fixed point multiplication of the recurrent state.
    MatrixWeight weight_fw_hh_fixed_point;
    MatrixWeight weight_bw_hh_fixed_point;

    void convert_matrix(const M& src, MatrixWeight& dst)
    {
      dst = MatrixWeight::Zero(src.rows(), src.cols());
      for (Eigen::Index row = 0; row < src.rows(); ++row)
        for (Eigen::Index col = 0; col < src.cols(); ++col)
        {
          auto val = src(row, col);
          dst(row, col) = static_cast<fixed_point_t>((val >= 0.0) ? (val * WEIGHT_FRACTION_MULT + T{0.5}) : (val * WEIGHT_FRACTION_MULT - T{0.5}));
        }
    }

    bool precompute()
    {
      if constexpr (std::is_integral_v<AuxScalar> && std::is_signed_v<AuxScalar>)
      {
        std::cerr << "Converting hh to fixed_point" << std::endl;
        std::cerr << "min(fw_weight_hh) = " << bilstm.fw.weight_hh.minCoeff() << " "
                  << "max(fw_weight_hh) = " << bilstm.fw.weight_hh.maxCoeff() << std::endl;
        convert_matrix(bilstm.fw.weight_hh, weight_fw_hh_fixed_point);
        std::cerr << "min(fw_weight_hh) = " << static_cast<T>(weight_fw_hh_fixed_point.minCoeff()) / WEIGHT_FRACTION_MULT << " "
                  << "max(fw_weight_hh) = " << static_cast<T>(weight_fw_hh_fixed_point.maxCoeff()) / WEIGHT_FRACTION_MULT << std::endl;

        std::cerr << "min(bw_weight_hh) = " << bilstm.bw.weight_hh.minCoeff() << " "
                  << "max(bw_weight_hh) = " << bilstm.bw.weight_hh.maxCoeff() << std::endl;
        convert_matrix(bilstm.bw.weight_hh, weight_bw_hh_fixed_point);
        std::cerr << "min(bw_weight_hh) = " << static_cast<T>(weight_bw_hh_fixed_point.minCoeff()) / WEIGHT_FRACTION_MULT << " "
                  << "max(bw_weight_hh) = " << static_cast<T>(weight_bw_hh_fixed_point.maxCoeff()) / WEIGHT_FRACTION_MULT << std::endl;
      }

      return true;
    }
#endif
};

  typedef params_bilstm_dense_argmax_t params_t;

  virtual std::shared_ptr<Op_Base::workbench_t> create_workbench(uint32_t input_size, const std::shared_ptr<param_base_t> params, bool precomputed_input=false) const override
  {
    assert(input_size > 0);
    assert(nullptr != params);
    const auto& layer = std::dynamic_pointer_cast<const params_t>(params)->bilstm;
    const auto& linear = std::dynamic_pointer_cast<const params_t>(params)->linear;

#ifdef MATMUL_WITH_FACTORIZATION
    const auto& mul_fw = std::dynamic_pointer_cast<const params_t>(params)->mul_fw;
    const auto& mul_bw = std::dynamic_pointer_cast<const params_t>(params)->mul_bw;
#endif

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
    const V fw_bias = layer.fw.bias_ih + layer.fw.bias_hh;
    output_block.topRows(hidden_size * 4).noalias() = (layer.fw.weight_ih * inputs).colwise() + fw_bias;

    /*
    std::cerr << "min(precomputed inputs) = " << output_block.topRows(hidden_size * 4).minCoeff() << " "
              << "max(precomputed inputs) = " << output_block.topRows(hidden_size * 4).maxCoeff() << std::endl;
    */

    // Bottom rows - backward pass
    const V bw_bias = layer.bw.bias_ih + layer.bw.bias_hh;
    output_block.bottomRows(hidden_size * 4).noalias() = (layer.bw.weight_ih * inputs).colwise() + bw_bias;

    /*
    std::cerr << "min(precomputed inputs) = " << output_block.bottomRows(hidden_size * 4).minCoeff() << " "
              << "max(precomputed inputs) = " << output_block.bottomRows(hidden_size * 4).maxCoeff() << std::endl;
    */
  }

  virtual size_t execute(std::shared_ptr<Op_Base::workbench_t> pwb,
                         const M& input_matrix,
                         const std::shared_ptr<param_base_t> pparams,
                         std::vector<std::vector<uint8_t>>& final_output,
                         size_t input_begin,
                         size_t /*input_end*/,
                         size_t output_begin,
                         size_t output_end)
  {
    assert(nullptr != pwb);
    assert(nullptr != params);
    const params_t& params = *std::dynamic_pointer_cast<const params_t>(pparams);
    const auto& layer = params.bilstm;
    const auto& linear = params.linear;

#ifdef MATMUL_WITH_FACTORIZATION
    const auto& mul_fw = params.mul_fw;
    const auto& mul_bw = params.mul_bw;
#endif

    auto wb = std::dynamic_pointer_cast<workbench_t>(pwb);
    M& temp = wb->temp;
    M& output = wb->out;
    const V& zero = wb->zero;

    size_t hidden_size = layer.fw.weight_ih.rows() / 4;
    V c = zero;
    V s = V::Zero(hidden_size * 4);

    Eigen::Ref<const M> input = input_matrix.block(0, input_begin, input_matrix.rows(), temp.cols());

    if (wb->m_precomputed_input)
    {
#ifdef MATMUL_WITH_FACTORIZATION
      forward_pass(hidden_size, input, layer.fw.weight_hh, mul_fw, s, c, output);
#else
      if constexpr (std::is_integral_v<AuxScalar> && std::is_signed_v<AuxScalar>)
      {
        forward_pass(hidden_size, input, params.weight_fw_hh_fixed_point, s, c, output);
      }
      else if constexpr (std::is_floating_point_v<AuxScalar>)
      {
        forward_pass(hidden_size, input, layer.fw.weight_hh, s, c, output);
      }
      else
      {
        static_assert((std::is_integral_v<AuxScalar> && std::is_signed_v<AuxScalar>)
          || std::is_floating_point_v<AuxScalar>, "AuxScalar must be a signed integer or floating-point type");
      }
#endif

      V c_fw = c;
      c = V::Zero(hidden_size);
#ifdef MATMUL_WITH_FACTORIZATION
      backward_pass(hidden_size, input, layer.bw.weight_hh, mul_bw, s, c, output);
#else
      if constexpr (std::is_integral_v<AuxScalar> && std::is_signed_v<AuxScalar>)
      {
        backward_pass(hidden_size, input, params.weight_bw_hh_fixed_point, s, c, output);
      }
      else if constexpr (std::is_floating_point_v<AuxScalar>)
      {
        backward_pass(hidden_size, input, layer.bw.weight_hh, s, c, output);
      }
      else
      {
        static_assert((std::is_integral_v<AuxScalar> && std::is_signed_v<AuxScalar>)
          || std::is_floating_point_v<AuxScalar>, "AuxScalar must be a signed integer or floating-point type");
      }
#endif
    }
    else
    {
      // Top rows - forward pass
      temp.topRows(hidden_size * 4) = (layer.fw.weight_ih * input).colwise() + layer.fw.bias_ih;
      // Bottom rows - backward pass
      temp.bottomRows(hidden_size * 4) = (layer.bw.weight_ih * input).colwise() + layer.bw.bias_ih;

#ifdef MATMUL_WITH_FACTORIZATION
      forward_pass(hidden_size, temp, layer.fw.weight_hh, mul_fw, s, c, output);
#else
      if constexpr (std::is_integral_v<AuxScalar> && std::is_signed_v<AuxScalar>)
      {
        forward_pass(hidden_size, temp, params.weight_fw_hh_fixed_point, s, c, output);
      }
      else if constexpr (std::is_floating_point_v<AuxScalar>)
      {
        forward_pass(hidden_size, temp, layer.fw.weight_hh, s, c, output);
      }
      else
      {
        static_assert((std::is_integral_v<AuxScalar> && std::is_signed_v<AuxScalar>)
          || std::is_floating_point_v<AuxScalar>, "AuxScalar must be a signed integer or floating-point type");
      }
#endif

      V c_fw = c;
      c = V::Zero(hidden_size);

#ifdef MATMUL_WITH_FACTORIZATION
      backward_pass(hidden_size, temp, layer.bw.weight_hh, mul_bw, s, c, output);
#else
      if constexpr (std::is_integral_v<AuxScalar> && std::is_signed_v<AuxScalar>)
      {
        backward_pass(hidden_size, temp, params.weight_bw_hh_fixed_point, s, c, output);
      }
      else if constexpr (std::is_floating_point_v<AuxScalar>)
      {
        backward_pass(hidden_size, temp, layer.bw.weight_hh, s, c, output);
      }
      else
      {
        static_assert((std::is_integral_v<AuxScalar> && std::is_signed_v<AuxScalar>)
          || std::is_floating_point_v<AuxScalar>, "AuxScalar must be a signed integer or floating-point type");
      }
#endif
    }

#ifdef BILSTM_AND_DENSE_PROFILE
      const auto before_linear = std::chrono::high_resolution_clock::now();
#endif

/*
    M lo = linear[0].weight * output.col(0);
    for (auto i = output_begin - input_begin; i < output_end - input_begin; i++)
    {
      for (size_t j = 0; j < final_output.size(); ++j)
      {
        //lo = (linear[j].weight * output.col(i)).colwise() + linear[j].bias;
        Eigen::Index idx = 0;
        // TODO scalar v value is not used but maxCoeff has side effect. It must be kept. Should we remove the
        // return value or use it somewhat?
        // typename M::Scalar v =
        ((linear[j].weight * output.col(i)) + linear[j].bias).col(0).maxCoeff(&idx);

        //lo.col(0).maxCoeff(&idx);
        assert(idx >= 0);
        assert(idx < std::numeric_limits<uint8_t>::max());
        final_output[j][input_begin+i] = (uint8_t) idx;
      }
    }
*/

    // Linear layer on top of RNN outputs
    M& linear_output = wb->lin_out[0];
    for (size_t j = 0; j < final_output.size(); j++)
    {
      linear_output.noalias() = (linear[j].weight * output).colwise() + linear[j].bias;
      //linear_output.colwise() += linear[j].bias;

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

#ifdef BILSTM_AND_DENSE_PROFILE
    const auto after_linear = std::chrono::high_resolution_clock::now();
    std::cerr << "output.cols()==" << output.cols() << " "
              << "linear time: " << std::chrono::duration_cast<std::chrono::microseconds>(after_linear - before_linear).count() << " "
              << std::endl;
#endif


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
        const MatrixWeight& weight_hh,  // [in]     the weights matrix
#ifdef MATMUL_WITH_FACTORIZATION
        const typename params_t::multiplier_t& mul,
#endif
        V& s,                           // [temp]   preallocated space for results before gating
        V& c,                           // [in/out] the cell state
        M& output                       // [out]    the output states
      )
  {
    step_fw(hidden_size, 0, input.col(0).topRows(hidden_size * 4), c, output);

#ifdef MATMUL_WITH_FACTORIZATION
    V prev_state;
#endif

#ifdef BILSTM_AND_DENSE_PROFILE
    std::chrono::duration<double> sum1(0), sum2(0);
#endif
    for (Eigen::Index t = 1; t < input.cols(); t++)
    {
#ifdef BILSTM_AND_DENSE_PROFILE
      const auto before1 = std::chrono::high_resolution_clock::now();
#endif

#ifdef MATMUL_WITH_FACTORIZATION
      /*
      s.segment(0, hidden_size).noalias() = mul.matmul_input.permutationP() * output.block(0, t-1, hidden_size, 1);
      mul.matmul_input.matrixLU().template triangularView<Eigen::UnitLower>().solveInPlace(s.segment(0, hidden_size));
      mul.matmul_input.matrixLU().template triangularView<Eigen::Upper>().solveInPlace(s.segment(0, hidden_size));

      s.segment(hidden_size, hidden_size) = mul.matmul_forget.permutationP() * output.block(0, t-1, hidden_size, 1);
      mul.matmul_forget.matrixLU().template triangularView<Eigen::UnitLower>().solveInPlace(s.segment(hidden_size, hidden_size));
      mul.matmul_forget.matrixLU().template triangularView<Eigen::Upper>().solveInPlace(s.segment(hidden_size, hidden_size));

      s.segment(hidden_size*2, hidden_size) = mul.matmul_update.permutationP() * output.block(0, t-1, hidden_size, 1);
      mul.matmul_update.matrixLU().template triangularView<Eigen::UnitLower>().solveInPlace(s.segment(hidden_size*2, hidden_size));
      mul.matmul_update.matrixLU().template triangularView<Eigen::Upper>().solveInPlace(s.segment(hidden_size*2, hidden_size));

      s.segment(hidden_size*3, hidden_size) = mul.matmul_output.permutationP() * output.block(0, t-1, hidden_size, 1);
      mul.matmul_output.matrixLU().template triangularView<Eigen::UnitLower>().solveInPlace(s.segment(hidden_size*3, hidden_size));
      mul.matmul_output.matrixLU().template triangularView<Eigen::Upper>().solveInPlace(s.segment(hidden_size*3, hidden_size));
      */

      prev_state = output.block(0, t-1, hidden_size, 1);
      s.segment(0, hidden_size) = mul.matmul_input.solve(prev_state);
      s.segment(hidden_size, hidden_size) = mul.matmul_forget.solve(prev_state);
      s.segment(hidden_size*2, hidden_size) = mul.matmul_update.solve(prev_state);
      s.segment(hidden_size*3, hidden_size) = mul.matmul_output.solve(prev_state);

      s += input.col(t).topRows(hidden_size * 4);
#else
      if constexpr (std::is_integral_v<AuxScalar> && std::is_signed_v<AuxScalar>)
      {
        s = input.col(t).topRows(hidden_size * 4);
        MatrixInput arg2 = (output.block(0, t-1, hidden_size, 1) * DATA_FRACTION_MULT).template cast<fixed_point_t>() ;
        s.noalias() += ((weight_hh * arg2).template cast<T>() / WEIGHT_DATA_FRACTION_MULT);
      }
      else if constexpr (std::is_floating_point_v<AuxScalar>)
      {
        s = input.col(t).topRows(hidden_size * 4);
        s.noalias() += weight_hh * output.block(0, t-1, hidden_size, 1);

        //s = weight_hh * output.block(0, t-1, hidden_size, 1);
        //s += input.col(t).topRows(hidden_size * 4);
      }
      else
      {
        static_assert((std::is_integral_v<AuxScalar> && std::is_signed_v<AuxScalar>)
          || std::is_floating_point_v<AuxScalar>, "AuxScalar must be a signed integer or floating-point type");
      }
#endif

#ifdef BILSTM_AND_DENSE_PROFILE
      const auto after1 = std::chrono::high_resolution_clock::now();
      sum1 += after1 - before1;

      const auto before2 = std::chrono::high_resolution_clock::now();
#endif
      step_fw(hidden_size, t, s, c, output);

#ifdef BILSTM_AND_DENSE_PROFILE
      const auto after2 = std::chrono::high_resolution_clock::now();
      sum2 += after2 - before2;
#endif
    }

#ifdef BILSTM_AND_DENSE_PROFILE
    std::cerr << "input.cols()==" << input.cols() << " "
              << "step_fw avg time: " << std::chrono::duration_cast<std::chrono::microseconds>(sum2).count() << " "
              << "in+recc avg time: " << std::chrono::duration_cast<std::chrono::microseconds>(sum1).count()
              << std::endl;
#endif
  }

  inline void backward_pass(
        const size_t hidden_size,       // [in]     the size of the hidden state
        Eigen::Ref<const M> input,      // [in]     the pre-computed inputs
        const MatrixWeight& weight_hh,  // [in]     the weights matrix
#ifdef MATMUL_WITH_FACTORIZATION
        const typename params_t::multiplier_t& mul,
#endif
        V& s,                           // [temp]   preallocated space for results before gating
        V& c,                           // [in/out] the cell state
        M& output                       // [out]    the output states
      )
  {
    int t = input.cols() - 1;
    step_bw(hidden_size, t, input.col(t).bottomRows(hidden_size * 4), c, output);

    for (t = input.cols() - 2; t >= 0; t--)
    {
      if constexpr (std::is_integral_v<AuxScalar> && std::is_signed_v<AuxScalar>)
      {
        s = input.col(t).bottomRows(hidden_size * 4);
        MatrixInput arg2 = (output.block(hidden_size, t+1, hidden_size, 1) * DATA_FRACTION_MULT).template cast<fixed_point_t>() ;
        s.noalias() += ((weight_hh * arg2).template cast<float>() / WEIGHT_DATA_FRACTION_MULT);
      }
      else if constexpr (std::is_floating_point_v<AuxScalar>)
      {
        s = input.col(t).bottomRows(hidden_size * 4);
        s.noalias() += weight_hh * output.block(hidden_size, t+1, hidden_size, 1);
      }
      else
      {
        static_assert((std::is_integral_v<AuxScalar> && std::is_signed_v<AuxScalar>)
          || std::is_floating_point_v<AuxScalar>, "AuxScalar must be a signed integer or floating-point type");
      }
      step_bw(hidden_size, t, s, c, output);
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
