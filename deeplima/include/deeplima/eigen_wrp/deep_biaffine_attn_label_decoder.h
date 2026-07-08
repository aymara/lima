// Copyright 2026 CEA LIST
// SPDX-FileCopyrightText: 2026 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_SRC_INFERENCE_EIGEN_DEEP_BIAFFINE_ATTN_LABEL_DECODER_H
#define DEEPLIMA_SRC_INFERENCE_EIGEN_DEEP_BIAFFINE_ATTN_LABEL_DECODER_H

#include <cmath>
#include <vector>

#include <eigen3/Eigen/Dense>
#include "op_base.h"

namespace deeplima
{
namespace eigen_impl
{

// Eigen (CPU-inference) counterpart of
// nets::torch_modules::DeepBiaffineAttentionLabelDecoder. One sentence at a time.
template<class M=Eigen::MatrixXf, class V=Eigen::VectorXf>
struct params_deep_biaffine_attn_label_decoder_t : public param_base_t
{
  M m_weight_head;
  V m_bias_head;
  M m_weight_dep;
  V m_bias_dep;
  std::vector<M> m_U; // one (hidden+1) x (hidden+1) matrix per label
  // The m_U matrices concatenated column-wise into a single [(hidden+1), L*(hidden+1)]
  // matrix, so the per-label products can be computed with one GEMM instead of L
  // tiny ones. Built once at load time (build_stacked_U); if empty, predict_labels
  // falls back to the per-label loop.
  M m_U_stacked;
  V m_root;           // head-side <ROOT> row, used when !m_input_includes_root
  bool m_input_includes_root = false;

  // Concatenate m_U[0..L-1] horizontally into m_U_stacked. Call once after m_U is
  // populated (e.g. at model conversion time).
  void build_stacked_U()
  {
    if (m_U.empty())
    {
      m_U_stacked = M();
      return;
    }
    const Eigen::Index d = m_U.front().rows();   // hidden+1
    const Eigen::Index L = (Eigen::Index) m_U.size();
    m_U_stacked.resize(d, L * d);
    for (Eigen::Index l = 0; l < L; ++l)
    {
      m_U_stacked.block(0, l * d, d, d) = m_U[l];
    }
  }
};

template<class M, class V, class T>
class Op_DeepBiaffineAttnLabelDecoder
{
public:
  typedef params_deep_biaffine_attn_label_decoder_t<M, V> params_t;

  // Per-label label logits for one sentence.
  // input: [input_dim, n_tokens] (token features as columns).
  // returns: a vector of m_U.size() matrices, each [n_dep, n_head], where
  //   n_dep  = n_tokens
  //   n_head = n_tokens (+1 when a root row is prepended, i.e. !input_includes_root)
  std::vector<M> compute_logits(const params_t& p, const M& input) const
  {
    // h = elu(W x + b), as [n_tokens, hidden]
    M h_dep = ((p.m_weight_dep * input).colwise() + p.m_bias_dep).transpose();
    elu_inplace(h_dep);
    M h_head = ((p.m_weight_head * input).colwise() + p.m_bias_head).transpose();
    elu_inplace(h_head);

    if (!p.m_input_includes_root)
    {
      M h_head_r(h_head.rows() + 1, h_head.cols());
      h_head_r.row(0) = p.m_root.transpose();
      h_head_r.block(1, 0, h_head.rows(), h_head.cols()) = h_head;
      h_head = h_head_r;
    }

    // Append a constant 1 column (affine augmentation).
    M aug_dep(h_dep.rows(), h_dep.cols() + 1);
    aug_dep << h_dep, M::Ones(h_dep.rows(), 1);
    M aug_head(h_head.rows(), h_head.cols() + 1);
    aug_head << h_head, M::Ones(h_head.rows(), 1);

    std::vector<M> logits;
    logits.reserve(p.m_U.size());
    for (const M& u : p.m_U)
    {
      // [n_dep, h+1] (h+1, h+1) (h+1, n_head) -> [n_dep, n_head]
      logits.push_back(aug_dep * u * aug_head.transpose());
    }
    return logits;
  }

  // For each dependent token, score the labels at its given head and take the
  // argmax. heads[i] is the head index of token input_begin + i (in the head
  // space, i.e. already accounting for the root row when applicable).
  void predict_labels(const params_t& p,
                      const M& input,
                      const std::vector<uint32_t>& heads,
                      size_t input_begin,
                      std::vector<uint32_t>& output) const
  {
    const size_t n_labels = p.m_U.size();
    if (n_labels == 0)
    {
      return;
    }

    // Reproduce the augmented dep/head representations exactly as compute_logits,
    // but score only the head each token actually got (already decoded by the arc
    // decoder). We therefore never materialise the full [n_dep x n_head] logit
    // matrices; we gather each token's head row and take a row-wise dot product.
    M h_dep = ((p.m_weight_dep * input).colwise() + p.m_bias_dep).transpose();
    elu_inplace(h_dep);
    M h_head = ((p.m_weight_head * input).colwise() + p.m_bias_head).transpose();
    elu_inplace(h_head);

    if (!p.m_input_includes_root)
    {
      M h_head_r(h_head.rows() + 1, h_head.cols());
      h_head_r.row(0) = p.m_root.transpose();
      h_head_r.block(1, 0, h_head.rows(), h_head.cols()) = h_head;
      h_head = h_head_r;
    }

    M aug_dep(h_dep.rows(), h_dep.cols() + 1);
    aug_dep << h_dep, M::Ones(h_dep.rows(), 1);
    M aug_head(h_head.rows(), h_head.cols() + 1);
    aug_head << h_head, M::Ones(h_head.rows(), 1);

    const Eigen::Index n_dep = aug_dep.rows();
    const Eigen::Index d = aug_dep.cols(); // hidden+1

    // gathered.row(i) = aug_head.row(head_of_token_i). heads[] is already in head
    // space (root row accounted for), matching aug_head's rows.
    M gathered(n_dep, d);
    for (Eigen::Index i = 0; i < n_dep; ++i)
    {
      gathered.row(i) = aug_head.row((Eigen::Index) heads[input_begin + i]);
    }

    // For each label l, score_l(i) = aug_dep.row(i) * U_l * gathered.row(i)^T.
    // Compute aug_dep * U_l for all labels at once via the pre-stacked U
    // ([d, L*d]) -> one GEMM producing [n_dep, L*d]; then a row-wise dot with
    // gathered per label slice. Falls back to per-label GEMMs if U isn't stacked.
    std::vector<Eigen::Index> best(n_dep, 0);
    std::vector<T> best_score(n_dep, -std::numeric_limits<T>::infinity());

    if (p.m_U_stacked.cols() == (Eigen::Index) n_labels * d
        && p.m_U_stacked.rows() == d)
    {
      const M projected = aug_dep * p.m_U_stacked; // [n_dep, L*d]
      for (size_t l = 0; l < n_labels; ++l)
      {
        const auto slice = projected.block(0, (Eigen::Index) l * d, n_dep, d);
        const V score = (slice.array() * gathered.array()).rowwise().sum();
        for (Eigen::Index i = 0; i < n_dep; ++i)
        {
          if (score(i) > best_score[i])
          {
            best_score[i] = score(i);
            best[i] = (Eigen::Index) l;
          }
        }
      }
    }
    else
    {
      for (size_t l = 0; l < n_labels; ++l)
      {
        const M tmp = aug_dep * p.m_U[l]; // [n_dep, d]
        const V score = (tmp.array() * gathered.array()).rowwise().sum();
        for (Eigen::Index i = 0; i < n_dep; ++i)
        {
          if (score(i) > best_score[i])
          {
            best_score[i] = score(i);
            best[i] = (Eigen::Index) l;
          }
        }
      }
    }

    for (Eigen::Index i = 0; i < n_dep; ++i)
    {
      output[input_begin + i] = (uint32_t) best[i];
    }
  }

protected:
  inline void elu_inplace(M& m) const
  {
    for (Eigen::Index r = 0; r < m.rows(); ++r)
    {
      for (Eigen::Index c = 0; c < m.cols(); ++c)
      {
        if (m(r, c) < 0)
        {
          m(r, c) = std::exp(m(r, c)) - 1;
        }
      }
    }
  }
};

} // namespace eigen_impl
} // namespace deeplima

#endif
