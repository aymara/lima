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

} // namespace eigen_impl
} // namespace deeplima

#endif
