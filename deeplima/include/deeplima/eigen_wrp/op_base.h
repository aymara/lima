// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_SRC_INFERENCE_EIGEN_OP_BASE_H
#define DEEPLIMA_SRC_INFERENCE_EIGEN_OP_BASE_H

#include <eigen3/Eigen/Dense>

namespace deeplima
{
namespace eigen_impl
{

struct param_base_t
{
  virtual ~param_base_t() { }

};

class Op_Base
{
public:

  virtual ~Op_Base() { }

  struct workbench_t
  {
    virtual ~workbench_t() { }
  };

  virtual workbench_t* create_workbench(uint32_t input_size, const param_base_t* params, bool precomputed_input) const = 0;
};

} // namespace eigen_impl
} // namespace deeplima

#endif
