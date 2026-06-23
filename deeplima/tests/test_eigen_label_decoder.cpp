// Copyright 2026 CEA LIST
// SPDX-FileCopyrightText: 2026 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

// Numeric-equivalence test: the Eigen (CPU-inference) label decoder must produce
// the same label logits as the torch DeepBiaffineAttentionLabelDecoder for the
// same parameters and input, for both <ROOT> conventions.

#include <torch/torch.h>
#include <eigen3/Eigen/Dense>

#include <cmath>
#include <iostream>

#include "nn/torch_modules/deep_biaffine_attention_label_decoder.h"
#include "deeplima/eigen_wrp/deep_biaffine_attn_label_decoder.h"

using deeplima::nets::torch_modules::DeepBiaffineAttentionLabelDecoder;
using deeplima::eigen_impl::Op_DeepBiaffineAttnLabelDecoder;
using deeplima::eigen_impl::params_deep_biaffine_attn_label_decoder_t;

static int g_failures = 0;

#define CHECK(cond, msg)                            \
  do {                                              \
    if (!(cond)) {                                  \
      std::cerr << "FAIL: " << (msg) << std::endl;  \
      ++g_failures;                                 \
    }                                               \
  } while (0)

static Eigen::MatrixXf to_eigen_mat(const torch::Tensor& src)
{
  torch::Tensor t = src.contiguous();
  Eigen::MatrixXf m(t.size(0), t.size(1));
  auto acc = t.accessor<float, 2>();
  for (int64_t i = 0; i < t.size(0); ++i)
    for (int64_t j = 0; j < t.size(1); ++j)
      m(i, j) = acc[i][j];
  return m;
}

static Eigen::VectorXf to_eigen_vec(const torch::Tensor& src)
{
  torch::Tensor t = src.contiguous().reshape({-1});
  Eigen::VectorXf v(t.size(0));
  auto acc = t.accessor<float, 1>();
  for (int64_t i = 0; i < t.size(0); ++i) v(i) = acc[i];
  return v;
}

int main()
{
  torch::manual_seed(7);
  const int64_t input_dim = 6, hidden = 5, num_labels = 4, seq = 3;

  for (bool root_incl : { true, false })
  {
    DeepBiaffineAttentionLabelDecoder dec(input_dim, hidden, num_labels, root_incl);
    dec->eval();
    torch::NoGradGuard no_grad;

    torch::Tensor input = torch::randn({ seq, 1, input_dim });
    torch::Tensor tl = dec->forward(input); // [1, dep, head, num_labels]

    params_deep_biaffine_attn_label_decoder_t<Eigen::MatrixXf, Eigen::VectorXf> p;
    p.m_weight_head = to_eigen_mat(dec->mlp_head->weight);
    p.m_bias_head = to_eigen_vec(dec->mlp_head->bias);
    p.m_weight_dep = to_eigen_mat(dec->mlp_dep->weight);
    p.m_bias_dep = to_eigen_vec(dec->mlp_dep->bias);
    p.m_input_includes_root = root_incl;
    p.m_U.resize(num_labels);
    for (int64_t l = 0; l < num_labels; ++l) p.m_U[l] = to_eigen_mat(dec->U[l]);
    p.m_root = to_eigen_vec(dec->root.reshape({-1}));

    Eigen::MatrixXf ein(input_dim, seq);
    for (int64_t d = 0; d < input_dim; ++d)
      for (int64_t t = 0; t < seq; ++t)
        ein(d, t) = input[t][0][d].item<float>();

    Op_DeepBiaffineAttnLabelDecoder<Eigen::MatrixXf, Eigen::VectorXf, float> op;
    std::vector<Eigen::MatrixXf> el = op.compute_logits(p, ein);

    const int64_t dep = tl.size(1), head = tl.size(2);
    CHECK((int64_t) el.size() == num_labels, "number of label matrices");
    CHECK(el.size() > 0 && el[0].rows() == dep, "dep dimension");
    CHECK(el.size() > 0 && el[0].cols() == head, "head dimension");

    double max_diff = 0.0;
    for (int64_t l = 0; l < num_labels; ++l)
      for (int64_t x = 0; x < dep; ++x)
        for (int64_t y = 0; y < head; ++y)
          max_diff = std::max(max_diff,
                              std::abs((double) tl[0][x][y][l].item<float>() - (double) el[l](x, y)));
    std::cerr << "root_includes=" << root_incl << " max|torch-eigen|=" << max_diff << std::endl;
    CHECK(max_diff < 1e-3, "eigen label logits match torch");
  }

  if (g_failures == 0) { std::cerr << "test_eigen_label_decoder: OK" << std::endl; return 0; }
  std::cerr << "test_eigen_label_decoder: " << g_failures << " failure(s)" << std::endl;
  return 1;
}
