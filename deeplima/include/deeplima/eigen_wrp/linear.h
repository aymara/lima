// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_SRC_INFERENCE_EIGEN_LINEAR_H
#define DEEPLIMA_SRC_INFERENCE_EIGEN_LINEAR_H

#include <eigen3/Eigen/Dense>
#include "op_base.h"

namespace deeplima
{
namespace eigen_impl
{

template<class M=Eigen::MatrixXf, class V=Eigen::VectorXf>
struct params_linear_t : public param_base_t
{
  M weight;
  V bias;
};

template<class M, class V, class T>
class Op_Linear : public Op_Base
{
protected:
  struct workbench_t : public Op_Base::workbench_t
  {
    workbench_t()
    {
    }
    virtual ~workbench_t() {}
  };

public:
  typedef V Vector;
  typedef params_linear_t<M, V> params_t;

  virtual std::shared_ptr<Op_Base::workbench_t> create_workbench([[maybe_unused]] uint32_t input_size,
                                        [[maybe_unused]] const std::shared_ptr<param_base_t> params,
                                        bool /*precomputed_input=false*/) const override
  {
    assert(input_size > 0);
    assert(nullptr != params);
    // TODO should it be used?
    // const params_linear_t<M, V>& layer = *static_cast<const params_t*>(params);

    return std::make_shared<workbench_t>();
  }

  virtual size_t execute([[maybe_unused]] std::shared_ptr<Op_Base::workbench_t> pwb,
                         const V& input,
                         const std::shared_ptr<param_base_t> params,
                         Vector& output)
  {
    assert(nullptr != pwb);
    assert(nullptr != params);
    const auto& layer = std::dynamic_pointer_cast<const params_t>(params);

    // TODO should it be used?
    // auto wb = std::dynamic_pointer_cast<workbench_t>(pwb);

    output = (layer->weight * input).colwise() + layer->bias;

    return 0;
  }
};

} // namespace eigen_impl
} // namespace deeplima

#endif
