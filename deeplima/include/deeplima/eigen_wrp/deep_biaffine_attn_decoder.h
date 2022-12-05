// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_SRC_INFERENCE_EIGEN_DEEP_BIAFFINE_ATTN_DECODER_H
#define DEEPLIMA_SRC_INFERENCE_EIGEN_DEEP_BIAFFINE_ATTN_DECODER_H

#include <eigen3/Eigen/Dense>
#include "op_base.h"
#include "deeplima/graph_dp/impl/arborescence.h"

namespace deeplima
{
namespace eigen_impl
{

template<class M=Eigen::MatrixXf, class V=Eigen::VectorXf>
struct params_deep_biaffine_attn_decoder_t : public param_base_t
{
  M m_weight_head;
  V m_bias_head;
  M m_weight_dep;
  V m_bias_dep;

  M m_U1;
  V m_u2;
};

template<class M, class V, class T>
class Op_DeepBiaffineAttnDecoder : public Op_Base
{
protected:
  struct workbench_t : public Op_Base::workbench_t
  {
    workbench_t()
    {
    }
    virtual ~workbench_t() {}

    M output;
  };

public:
  typedef V Vector;
  typedef params_deep_biaffine_attn_decoder_t<M, V> params_t;

  virtual workbench_t* create_workbench(uint32_t input_size, const param_base_t* params,
                                        bool precomputed_input=false) const
  {
    assert(input_size > 0);
    assert(nullptr != params);
//     TODO should it be used
    // const params_deep_biaffine_attn_decoder_t<M, V>& layer = *static_cast<const params_t*>(params);

    return new workbench_t();
  }

  virtual size_t execute(Op_Base::workbench_t* pwb,
                         const M& input_matrix,
                         const param_base_t* params,
                         const size_t input_begin,
                         const size_t input_end,
                         std::vector<uint32_t>& output)
  {
    assert(nullptr != pwb);
    assert(nullptr != params);
    const params_deep_biaffine_attn_decoder_t<M, V>& layer = *static_cast<const params_t*>(params);

//     TODO should it be used?
    // workbench_t* wb = static_cast<workbench_t*>(pwb);

    const M input = input_matrix.block(0, 0, input_matrix.rows(), input_end - input_begin);

    M arc_head = ((layer.m_weight_head * input).colwise() + layer.m_bias_head).transpose();
    elu_inplace(arc_head);
    M arc_dep = (layer.m_weight_dep * input).colwise() + layer.m_bias_dep;
    elu_inplace(arc_dep);

    M b(arc_head.rows(), arc_head.rows());
    for (Eigen::Index i = 0; i < arc_head.rows(); ++i)
    {
      b.col(i) = arc_head * layer.m_u2;
    }

    M Wx = (arc_head * layer.m_U1) * arc_dep;

    M logits = Wx + b;

    for (Eigen::Index i = 0; i < logits.rows(); ++i)
    {
      Eigen::Index idx = 0;
//       TODO should it be used?
      typename M::Scalar v = logits.row(i).maxCoeff(&idx);
      assert(idx >= 0);
      assert(idx < std::numeric_limits<Eigen::Index>::max());
      output[input_begin + i] = (uint32_t) idx;
      // std::cerr << i << "\t" << idx << std::endl;
    }
    // std::cerr << std::endl;

    //arborescence<M, uint32_t, typename M::Scalar>(logits, output, input_begin);

    return 0;
  }

protected:
  inline void elu_inplace(M& m)
  {
    for (Eigen::Index r = 0; r < m.rows(); ++r)
    {
      for (Eigen::Index c = 0; c < m.cols(); ++c)
      {
        if (m(r,c) < 0)
        {
          m(r,c) = expf(m(r,c)) - 1;
        }
      }
    }
  }
};

} // namespace eigen_impl
} // namespace deeplima

#endif
