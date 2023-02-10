/*
    Copyright 2022 CEA LIST

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

#ifndef DEEPLIMA_LIBS_NN_DEEP_BIAFFINE_ATTENTION_DECODER_H
#define DEEPLIMA_LIBS_NN_DEEP_BIAFFINE_ATTENTION_DECODER_H

#include <torch/torch.h>

namespace deeplima
{
namespace nets
{
namespace torch_modules
{

class DeepBiaffineAttentionLabelDecoderImpl : public torch::nn::Module
{
public:
  DeepBiaffineAttentionLabelDecoderImpl(int64_t input_dim, int64_t hidden_arc_dim, bool input_includes_root=false)
    : m_input_includes_root(input_includes_root),
      m_hidden_arc_dim(hidden_arc_dim),
      mlp_head(register_module("mlp_head", torch::nn::Linear(input_dim, hidden_arc_dim))),
      mlp_dep(register_module("mlp_dep", torch::nn::Linear(input_dim, hidden_arc_dim))),
      U1(register_parameter("U1", torch::randn({hidden_arc_dim, hidden_arc_dim}))),
      u2(register_parameter("u2", torch::randn({hidden_arc_dim, 1}))),
      root(register_parameter("root", torch::randn({1, 1, hidden_arc_dim}))),
      root2(register_parameter("root2", torch::randn({1, 1, hidden_arc_dim})))
  {
    // std::cerr << U1.dtype() << std::endl;
  }

  std::pair<torch::Tensor, torch::Tensor> forward(torch::Tensor input);

private:

  // see https://nlp.stanford.edu/pubs/dozat2017deep.pdf , page 3
  bool m_input_includes_root;
  int64_t m_hidden_arc_dim;
  torch::nn::Linear mlp_head;
  torch::nn::Linear mlp_dep;
  torch::nn::ELU elu;
  torch::Tensor U1, U2, u2;
  torch::Tensor root, root2; // used if input_includes_root == false
};

TORCH_MODULE(DeepBiaffineAttentionLabelDecoder);

} // torch_modules
} // nets
} // deeplima

#endif // DEEPLIMA_LIBS_NN_DEEP_BIAFFINE_ATTENTION_DECODER_H
