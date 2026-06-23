// Copyright 2022 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_LIBS_NN_DEEP_BIAFFINE_ATTENTION_DECODER_H
#define DEEPLIMA_LIBS_NN_DEEP_BIAFFINE_ATTENTION_DECODER_H

#include <torch/torch.h>

namespace deeplima
{
namespace nets
{
namespace torch_modules
{

class DeepBiaffineAttentionDecoderImpl : public torch::nn::Module
{
public:
  DeepBiaffineAttentionDecoderImpl(int64_t input_dim, int64_t hidden_arc_dim, bool input_includes_root=false)
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

  torch::Tensor forward(torch::Tensor input);

  // see https://nlp.stanford.edu/pubs/dozat2017deep.pdf , page 3
  bool m_input_includes_root;
  int64_t m_hidden_arc_dim;
  torch::nn::Linear mlp_head;
  torch::nn::Linear mlp_dep;
  torch::nn::ELU elu; // this with the linear above makes the MLP from [Dozat&Manning,2017]
  torch::Tensor U1, u2;
  torch::Tensor root, root2; // used if input_includes_root == false
};

TORCH_MODULE(DeepBiaffineAttentionDecoder);

} // torch_modules
} // nets
} // deeplima

#endif // DEEPLIMA_LIBS_NN_DEEP_BIAFFINE_ATTENTION_DECODER_H
