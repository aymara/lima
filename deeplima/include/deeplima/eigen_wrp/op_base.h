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
