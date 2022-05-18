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

} // namespace eigen_impl
} // namespace deeplima

#endif
