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

#include "deep_biaffine_attention_label_decoder.h"

namespace deeplima
{
namespace nets
{
namespace torch_modules
{

std::pair<torch::Tensor, torch::Tensor> DeepBiaffineAttentionLabelDecoderImpl::forward(torch::Tensor input)
{
  // The initial implementation by Dozat (python, tensorflow)
  // https://github.com/tdozat/Parser-v1
  //
  // A possible implementation (python, tensorflow):
  // https://github.com/yzhangcs/parser/tree/main/supar/models/dep/biaffine
  //
  // A possible implementation (python, pytorch)
  // https://github.com/XuezheMax/NeuroNLP2/blob/master/neuronlp2/models/parsing.py
  //
  // Another implementation (python, pytorch)
  // https://github.com/chantera/biaffineparser
  //
  // The Stanza implementation (python, pytorch)
  // https://github.com/stanfordnlp/stanza/blob/main/stanza/models/depparse/model.py
  //
  // Our initial implementation (python, tensorflow)
  // https://github.com/aymara/lima-ud-depparser/blob/master/model.py


  // # scorer (ARCS)
  int64_t batch_size = input.size(1);

  //std::cerr << "input.sizes() == " << input.sizes() << std::endl;
  torch::Tensor input_t = input.transpose(0, 1); // to [ batch x len x input_dim ]
  //std::cerr << "input_t.sizes() == " << input_t.sizes() << std::endl;

  // h_j^(arc-head) = MLP^(arc-head)(r_j) in (5) from [Dozat&Manning,2017]
  // and thus
  // H^(arc-head) in (6) from [Dozat&Manning,2017]
  torch::Tensor arc_head;
  if (!m_input_includes_root)
  {
    torch::Tensor roots2 = torch::tile(root2, { batch_size, 1, 1 });
    //arc_head = torch::cat({ roots2, elu(mlp_head(input_t)) }, 1);
    arc_head = elu(torch::cat({ roots2, mlp_head(input_t) }, 1));
  }
  else
  {
    arc_head = elu(mlp_head(input_t));
  }
  // arc_head = [ batch x len x hidden_dim ]
  //std::cerr << "arc_head.sizes() == " << arc_head.sizes() << std::endl;


  // h_i^(arc-dep) = MLP^(arc-dep)(r_i) in (4) from [Dozat&Manning,2017]
  torch::Tensor arc_dep;
  if (!m_input_includes_root)
  {
    torch::Tensor roots = torch::tile(root, { batch_size, 1, 1 });
    //arc_dep = torch::cat({ roots, elu(mlp_dep(input_t)) }, 1);
    arc_dep = elu(torch::cat({ roots, mlp_dep(input_t) }, 1));
  }
  else
  {
    arc_dep = elu(mlp_dep(input_t));
  }

  //std::cerr << "arc_dep.sizes() == " << arc_dep.sizes() << std::endl;
  //std::cerr << "U1.sizes() == " << U1.sizes() << std::endl;
  torch::Tensor W = torch::matmul(arc_head, U1);
  //std::cerr << "W.sizes() == " << W.sizes() << std::endl;
  //std::cerr << "u2.sizes() == " << u2.sizes() << std::endl;

  // H^(arc-head)u^(2) in (6) from [Dozat&Manning,2017]
  torch::Tensor b = torch::matmul(arc_head, torch::tile(u2, { 1, arc_head.size(1) }));


  //std::cerr << "b.sizes() == " << b.sizes() << std::endl;
  torch::Tensor Wx = torch::matmul(W, arc_dep.transpose(1, 2));
  //std::cerr << "Wx.sizes() == " << Wx.sizes() << std::endl;
  torch::Tensor r = torch::add(Wx, b);
  //std::cerr << "r.sizes() == " << r.sizes() << std::endl;

  if (!m_input_includes_root)
  {
    r = r.index({ torch::indexing::Slice(),
                  torch::indexing::Slice(1, r.size(1)),
                  torch::indexing::Slice()});
  }
  //std::cerr << "r.sizes() == " << r.sizes() << std::endl;



  // # scorer (LABELS)


  // label of word i given the predicted (or gold) head yi, from [Dozat&Manning,2017]
  // here U^(1) is bold, thus a higher-order tensor while U^(2) is in italics, thus a matrix
  // s_i(label) = r_yi^⊤ U^(1) r_i + (r_yi ⊕ r_i )^⊤ U^(2) + b
  // b = Ru^(2)

    // Dozat Parser implementation
    //
    // top_recur = RNN(embed_inputs) # n recur times
    //
    // # MLP = e.g. dropout + linear + relu splitted in 2
    // dep_mlp, head_mlp = self.MLP(top_recur,  self.class_mlp_size+self.attn_mlp_size, n_splits=2)
    //
    // # cut the returned dep and head tensors in arc and rel parts
    // dep_arc_mlp, dep_rel_mlp = dep_mlp[:,:,:self.attn_mlp_size], dep_mlp[:,:,self.attn_mlp_size:]
    // head_arc_mlp, head_rel_mlp = head_mlp[:,:,:self.attn_mlp_size], head_mlp[:,:,self.attn_mlp_size:]
    //
    // # Arcs
    // arc_logits = squeeze(linalg.bilinear(dropout(dep_arc_mlp), dropout(head_arc_mlp), …))
    // arc_output = self.output(arc_logits, targets[:,:,1]) # softmax + loss + …
    //
    // # retrieve arc predictions to use in labels
    // predictions = arc_output['predictions']
    //
    // rel_logits = linalg.bilinear(dropout(dep_rel_mlp), dropout(head_rel_mlp), n_classes, …)
    // rel_logits_cond = tf.batch_matmul(rel_logits, tf.expand_dims(predictions, 3))
    // rel_output = self.output(rel_logits, targets[:,:,2])
    // rel_output['probabilities'] = self.conditional_probabilities(rel_logits_cond)

  torch::Tensor r_label = torch::add(Wx, b);



  return {r, r_label};
}

} // torch_modules
} // nets
} // deeplima
