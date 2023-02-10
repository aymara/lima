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

#include "deep_biaffine_attention_decoder.h"

namespace deeplima
{
namespace nets
{
namespace torch_modules
{

torch::Tensor DeepBiaffineAttentionDecoderImpl::forward(torch::Tensor input)
{
  int64_t batch_size = input.size(1);

  //std::cerr << "input.sizes() == " << input.sizes() << std::endl;
  torch::Tensor input_t = input.transpose(0, 1); // to [ batch x len x input_dim ]
  //std::cerr << "input_t.sizes() == " << input_t.sizes() << std::endl;

  torch::Tensor arc_head;
  if (!m_input_includes_root)
  {
    torch::Tensor roots2 = torch::tile(root2, { batch_size, 1, 1 });
    //arc_head = torch::cat({ roots2, elu(mlp_head(input_t)) }, 1);
    arc_head = elu(torch::cat({ roots2, mlp_head(input_t) }, 1));
  }
  else
  {
    // h_j^(arc-head) = MLP^(arc-head)(r_j) in (5) from [Dozat&Manning,2017]
    // and thus
    // H^(arc-head) in (6) from [Dozat&Manning,2017]
    arc_head = elu(mlp_head(input_t));
  }
  // arc_head = [ batch x len x hidden_dim ]
  //std::cerr << "arc_head.sizes() == " << arc_head.sizes() << std::endl;

  torch::Tensor arc_dep;
  if (!m_input_includes_root)
  {
    torch::Tensor roots = torch::tile(root, { batch_size, 1, 1 });
    //arc_dep = torch::cat({ roots, elu(mlp_dep(input_t)) }, 1);
    arc_dep = elu(torch::cat({ roots, mlp_dep(input_t) }, 1));
  }
  else
  {
    // h_i^(arc-dep) = MLP^(arc-dep)(r_i) in (4) from [Dozat&Manning,2017]
    arc_dep = elu(mlp_dep(input_t));
  }

  //std::cerr << "arc_dep.sizes() == " << arc_dep.sizes() << std::endl;
  //std::cerr << "U1.sizes() == " << U1.sizes() << std::endl;
  torch::Tensor W = torch::matmul(arc_head, U1);
  //std::cerr << "W.sizes() == " << W.sizes() << std::endl;
  torch::Tensor Wx = torch::matmul(W, arc_dep.transpose(1, 2)); //
  //std::cerr << "Wx.sizes() == " << Wx.sizes() << std::endl;

  //std::cerr << "u2.sizes() == " << u2.sizes() << std::endl;
  torch::Tensor b = torch::matmul(arc_head, torch::tile(u2, { 1, arc_head.size(1) })); // H^(arc-head)u^(2) in (6) from [Dozat&Manning,2017]

  //std::cerr << "b.sizes() == " << b.sizes() << std::endl;
  torch::Tensor r = torch::add(Wx, b);
  //std::cerr << "r.sizes() == " << r.sizes() << std::endl;

  if (!m_input_includes_root)
  {
    r = r.index({ torch::indexing::Slice(),
                  torch::indexing::Slice(1, r.size(1)),
                  torch::indexing::Slice()});
  }
  //std::cerr << "r.sizes() == " << r.sizes() << std::endl;

  return r;
}

} // torch_modules
} // nets
} // deeplima
