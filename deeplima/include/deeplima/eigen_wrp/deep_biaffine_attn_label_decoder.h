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
  V m_root;           // head-side <ROOT> row, used when !m_input_includes_root
  bool m_input_includes_root = false;
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
    const std::vector<M> logits = compute_logits(p, input);
    const Eigen::Index n_dep = logits.empty() ? 0 : logits[0].rows();
    const size_t n_labels = logits.size();
    for (Eigen::Index i = 0; i < n_dep; ++i)
    {
      const Eigen::Index head = (Eigen::Index) heads[input_begin + i];
      Eigen::Index best = 0;
      T best_score = -std::numeric_limits<T>::infinity();
      for (size_t l = 0; l < n_labels; ++l)
      {
        const T s = logits[l](i, head);
        if (s > best_score)
        {
          best_score = s;
          best = (Eigen::Index) l;
        }
      }
      output[input_begin + i] = (uint32_t) best;
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
