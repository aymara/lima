// Copyright 2022 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "deep_biaffine_attention_label_decoder.h"

namespace deeplima
{
namespace nets
{
namespace torch_modules
{

torch::Tensor DeepBiaffineAttentionLabelDecoderImpl::forward(torch::Tensor input)
{
  using torch::indexing::Slice;

  const int64_t batch_size = input.size(1);

  // [seq, batch, input_dim] -> [batch, seq, input_dim]
  torch::Tensor input_t = input.transpose(0, 1);

  // Per-token dependent and head representations (eq. (4)-(5) of Dozat&Manning).
  torch::Tensor h_dep = elu(mlp_dep(input_t));   // [batch, seq, hidden]
  torch::Tensor h_head = elu(mlp_head(input_t)); // [batch, seq, hidden]

  if (!m_input_includes_root)
  {
    // Prepend a learned <ROOT> row on the head axis so head index 0 is the root.
    torch::Tensor roots = torch::tile(root, {batch_size, 1, 1}); // [batch, 1, hidden]
    h_head = torch::cat({roots, h_head}, 1);                     // [batch, seq+1, hidden]
  }

  // Affine augmentation: append a constant 1 feature to fold the linear and bias
  // terms into a single per-label matrix multiply.
  auto append_ones = [](const torch::Tensor& t) {
    torch::Tensor ones = torch::ones({t.size(0), t.size(1), 1}, t.options());
    return torch::cat({t, ones}, 2);
  };
  torch::Tensor aug_dep = append_ones(h_dep);   // [batch, dep, hidden+1]
  torch::Tensor aug_head = append_ones(h_head); // [batch, head, hidden+1]

  // s[b, i, j, l] = aug_dep[b,i,:]^T U[l] aug_head[b,j,:]
  // -> [batch, dep, head, num_labels]
  torch::Tensor logits = torch::einsum("bxi,lij,byj->bxyl", {aug_dep, U, aug_head});

  return logits;
}

} // torch_modules
} // nets
} // deeplima
