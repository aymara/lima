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

class DeepBiaffineAttentionDecoderImpl : public torch::nn::Module
{
public:
  DeepBiaffineAttentionDecoderImpl(int64_t input_dim, int64_t hidden_arc_dim)
    : m_hidden_arc_dim(hidden_arc_dim),
      mlp_head(register_module("mlp_head", torch::nn::Linear(input_dim, hidden_arc_dim))),
      mlp_dep(register_module("mlp_dep", torch::nn::Linear(input_dim, hidden_arc_dim))),
      U1(register_parameter("U1", torch::randn({hidden_arc_dim, hidden_arc_dim}))),
      u2(register_parameter("u2", torch::randn({hidden_arc_dim, 1}))),
      root(register_parameter("root", torch::randn({1, 1, hidden_arc_dim}))),
      root2(register_parameter("root2", torch::randn({1, 1, hidden_arc_dim})))
  {
    std::cerr << U1.dtype() << std::endl;
  }

  torch::Tensor forward(torch::Tensor input)
  {
    int64_t batch_size = input.size(1);

    //std::cerr << "input.sizes() == " << input.sizes() << std::endl;
    torch::Tensor input_t = input.transpose(0, 1); // to [ batch x len x input_dim ]
    //std::cerr << "input_t.sizes() == " << input_t.sizes() << std::endl;

#ifdef WITH_ROOTS
    torch::Tensor roots2 = torch::tile(root2, { batch_size, 1, 1 });
    torch::Tensor arc_head = torch::cat({ roots2, elu(mlp_head(input_t)) }, 1);
#else
    torch::Tensor arc_head = elu(mlp_head(input_t));
#endif
    // arc_head = [ batch x len x hidden_dim ]
    //std::cerr << "arc_head.sizes() == " << arc_head.sizes() << std::endl;

#ifdef WITH_ROOTS
    torch::Tensor roots = torch::tile(root, { batch_size, 1, 1 });
    torch::Tensor arc_dep = torch::cat({ roots, elu(mlp_dep(input_t)) }, 1);
#else
    torch::Tensor arc_dep = elu(mlp_dep(input_t));
#endif

    //std::cerr << "arc_dep.sizes() == " << arc_dep.sizes() << std::endl;
    //std::cerr << "U1.sizes() == " << U1.sizes() << std::endl;
    torch::Tensor W = torch::matmul(arc_head, U1);
    //std::cerr << "W.sizes() == " << W.sizes() << std::endl;
    //std::cerr << "u2.sizes() == " << u2.sizes() << std::endl;
    torch::Tensor b = torch::matmul(arc_head, torch::tile(u2, { 1, arc_head.size(1) }));

    //std::cerr << "b.sizes() == " << b.sizes() << std::endl;
    torch::Tensor Wx = torch::matmul(W, arc_dep.transpose(1, 2));
    //std::cerr << "Wx.sizes() == " << Wx.sizes() << std::endl;
    torch::Tensor r = torch::add(Wx, b);
    //std::cerr << "r.sizes() == " << r.sizes() << std::endl;

#ifdef WITH_ROOTS
    r = r.index({ torch::indexing::Slice(),
                  torch::indexing::Slice(1, r.size(1)),
                  torch::indexing::Slice()});
    std::cerr << "r.sizes() == " << r.sizes() << std::endl;
#endif

    return r;
  }

  // see https://nlp.stanford.edu/pubs/dozat2017deep.pdf , page 3
  int64_t m_hidden_arc_dim;
  torch::nn::Linear mlp_head;
  torch::nn::Linear mlp_dep;
  torch::nn::ELU elu;
  torch::Tensor U1, u2, root, root2;
};

TORCH_MODULE(DeepBiaffineAttentionDecoder);

} // torch_modules
} // nets
} // deeplima

#endif // DEEPLIMA_LIBS_NN_DEEP_BIAFFINE_ATTENTION_DECODER_H
