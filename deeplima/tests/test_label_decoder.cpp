// Copyright 2026 CEA LIST
// SPDX-FileCopyrightText: 2026 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

// Unit tests for the biaffine label (deprel) scorer
// DeepBiaffineAttentionLabelDecoder: output shape, <ROOT> handling, finiteness
// and trainability (gradient flow).

#include <torch/torch.h>

#include <iostream>

#include "nn/torch_modules/deep_biaffine_attention_label_decoder.h"

using deeplima::nets::torch_modules::DeepBiaffineAttentionLabelDecoder;

static int g_failures = 0;

#define CHECK(cond, msg)                                        \
  do {                                                          \
    if (!(cond)) {                                              \
      std::cerr << "FAIL: " << (msg) << std::endl;              \
      ++g_failures;                                             \
    }                                                           \
  } while (0)

int main()
{
  torch::manual_seed(42);
  const int64_t input_dim = 10, hidden = 8, num_labels = 5, seq = 4, batch = 3;

  // input_includes_root == true: output is [batch, dep=seq, head=seq, num_labels]
  {
    DeepBiaffineAttentionLabelDecoder dec(input_dim, hidden, num_labels, true);
    torch::Tensor input = torch::randn({seq, batch, input_dim});
    torch::Tensor out = dec->forward(input);
    CHECK(out.dim() == 4, "output rank should be 4");
    CHECK(out.size(0) == batch, "batch dim");
    CHECK(out.size(1) == seq, "dependent dim");
    CHECK(out.size(2) == seq, "head dim (root already in input)");
    CHECK(out.size(3) == num_labels, "num_labels dim");
    CHECK(torch::isfinite(out).all().item<bool>(), "outputs are finite");
  }

  // input_includes_root == false: a root row is prepended on the head axis only
  {
    DeepBiaffineAttentionLabelDecoder dec(input_dim, hidden, num_labels, false);
    torch::Tensor input = torch::randn({seq, batch, input_dim});
    torch::Tensor out = dec->forward(input);
    CHECK(out.size(0) == batch, "batch dim (no root)");
    CHECK(out.size(1) == seq, "dependent dim (no root)");
    CHECK(out.size(2) == seq + 1, "head dim should be seq+1 (root prepended)");
    CHECK(out.size(3) == num_labels, "num_labels dim (no root)");
  }

  // The scorer is trainable: a backward pass produces non-zero gradients.
  {
    DeepBiaffineAttentionLabelDecoder dec(input_dim, hidden, num_labels, true);
    torch::Tensor input = torch::randn({seq, batch, input_dim});
    torch::Tensor out = dec->forward(input);
    out.sum().backward();
    bool any_grad = false;
    for (const auto& p : dec->parameters())
    {
      if (p.grad().defined() && p.grad().abs().sum().item<double>() > 0.0)
      {
        any_grad = true;
      }
    }
    CHECK(any_grad, "parameters receive gradients");
  }

  if (g_failures == 0)
  {
    std::cerr << "test_label_decoder: OK" << std::endl;
    return 0;
  }
  std::cerr << "test_label_decoder: " << g_failures << " failure(s)" << std::endl;
  return 1;
}
