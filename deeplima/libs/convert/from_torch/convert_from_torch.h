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

#ifndef DEEPLIMA_LIBS_CONVERT_FROM_TORCH_H
#define DEEPLIMA_LIBS_CONVERT_FROM_TORCH_H

#include <torch/torch.h>

#include "static_graph/dict.h"
#include "deeplima/eigen_wrp/embd_dict.h"

namespace deeplima
{
namespace convert_from_torch
{

template<class V, class S>
void copy_vector(const torch::Tensor& src, V& dst)
{
  assert(src.size(0) >= 0);
  dst.resize(src.size(0));
  for (long i = 0; i < src.size(0); i++)
  {
    dst(i) = src[i].item<S>();
  }
}

template<class M, class S>
void copy_matrix(const torch::Tensor& src, M& dst)
{
  assert(src.size(0) >= 0);
  assert(src.size(1) >= 0);
  dst.resize(src.size(0), src.size(1));
  auto accs = src.accessor<S,2>();
  for (long i = 0; i < src.size(0); i++)
  {
    for (long j = 0; j < src.size(1); j++)
    {
      dst(i, j) = accs[i][j];
    }
  }
}

template <class M, class V, class S=float>
void convert_module_from_torch(const torch::nn::LSTM& src, eigen_impl::params_bilstm_t<M, V>& dst)
{
  const std::vector<torch::Tensor> v = src->all_weights();

  copy_matrix<M, S>(v[0], dst.fw.weight_ih);
  copy_matrix<M, S>(v[1], dst.fw.weight_hh);
  copy_vector<V, S>(v[2], dst.fw.bias_ih);
  copy_vector<V, S>(v[3], dst.fw.bias_hh);

  copy_matrix<M, S>(v[4], dst.bw.weight_ih);
  copy_matrix<M, S>(v[5], dst.bw.weight_hh);
  copy_vector<V, S>(v[6], dst.bw.bias_ih);
  copy_vector<V, S>(v[7], dst.bw.bias_hh);
}

template <class M, class V, class S=float>
void convert_module_from_torch(const torch::nn::Linear& src, eigen_impl::params_linear_t<M, V>& dst)
{
  copy_matrix<M, S>(src->weight, dst.weight);
  copy_vector<V, S>(src->bias, dst.bias);
}

template <class M, class S=float>
void convert_module_from_torch(const torch::nn::Embedding& src, std::shared_ptr<DictBase> sp_dict, EmbdDict<uint64_t, M>& dst)
{
  std::shared_ptr<UInt64Dict> sp_uint64_dict = std::dynamic_pointer_cast<UInt64Dict, DictBase>(sp_dict);
  assert(nullptr != sp_uint64_dict);
  M tmp;
  copy_matrix<M, S>(src->weight, tmp);
  dst.init(sp_uint64_dict, tmp);
}

template <class M, class S=float>
void convert_module_from_torch(const torch::nn::Embedding& src, std::shared_ptr<DictBase> sp_dict, EmbdDict<std::string, M>& dst)
{
  std::shared_ptr<StringDict> sp_str_dict = std::dynamic_pointer_cast<StringDict, DictBase>(sp_dict);
  assert(nullptr != sp_str_dict);
  M tmp;
  copy_matrix<M, S>(src->weight, tmp);
  dst.init(sp_str_dict, tmp);
}

} // namespace convert_from_torch
} // namespace deeplima

#endif
