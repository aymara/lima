// Copyright 2022 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_LIBS_NN_DEEP_BIAFFINE_ATTENTION_LABEL_DECODER_H
#define DEEPLIMA_LIBS_NN_DEEP_BIAFFINE_ATTENTION_LABEL_DECODER_H

#include <torch/torch.h>

namespace deeplima
{
namespace nets
{
namespace torch_modules
{

// Biaffine label (deprel) scorer from Dozat & Manning, 2017
// (https://nlp.stanford.edu/pubs/dozat2017deep.pdf), section 2.2 "Deep biaffine
// scoring", equations (4)-(6) applied to the label scorer.
//
// For every ordered pair (dependent i, head j) it produces a score per relation
// label. The caller selects, for each dependent, the scores at its head (the gold
// head while training, the predicted head at inference).
//
// forward(input):
//   input:  [seq_len, batch, input_dim]   (the encoder states, time-major)
//   result: [batch, dep, head, num_labels] label logits.
//   When input_includes_root is false a learned <ROOT> row is prepended on the
//   head axis (so head index 0 is the artificial root and `head` == seq_len + 1),
//   and the root is removed from the dependent axis (`dep` == seq_len), matching
//   the convention of DeepBiaffineAttentionDecoder (the arc scorer).
class DeepBiaffineAttentionLabelDecoderImpl : public torch::nn::Module
{
public:
  DeepBiaffineAttentionLabelDecoderImpl(int64_t input_dim,
                                        int64_t hidden_dim,
                                        int64_t num_labels,
                                        bool input_includes_root = false)
    : m_input_includes_root(input_includes_root),
      m_hidden_dim(hidden_dim),
      m_num_labels(num_labels),
      mlp_head(register_module("mlp_head", torch::nn::Linear(input_dim, hidden_dim))),
      mlp_dep(register_module("mlp_dep", torch::nn::Linear(input_dim, hidden_dim))),
      // Affine biaffine: the dependent and head representations are each augmented
      // with a constant 1, so a single (hidden+1) x (hidden+1) matrix per label
      // captures the bilinear term, both linear terms and the bias.
      U(register_parameter("U", torch::randn({num_labels, hidden_dim + 1, hidden_dim + 1})
                                    * (1.0 / std::sqrt(double(hidden_dim + 1))))),
      root(register_parameter("root", torch::randn({1, 1, hidden_dim})))
  {
  }

  int64_t num_labels() const { return m_num_labels; }

  torch::Tensor forward(torch::Tensor input);

private:
  bool m_input_includes_root;
  int64_t m_hidden_dim;
  int64_t m_num_labels;
  torch::nn::Linear mlp_head;
  torch::nn::Linear mlp_dep;
  torch::nn::ELU elu;
  torch::Tensor U;
  torch::Tensor root; // learned head-side <ROOT> row, used if input_includes_root == false
};

TORCH_MODULE(DeepBiaffineAttentionLabelDecoder);

} // torch_modules
} // nets
} // deeplima

#endif // DEEPLIMA_LIBS_NN_DEEP_BIAFFINE_ATTENTION_LABEL_DECODER_H
