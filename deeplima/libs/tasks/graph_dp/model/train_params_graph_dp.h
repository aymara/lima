// Copyright 2022 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_LIBS_TASKS_GRAPH_DP_PARAMS_H
#define DEEPLIMA_LIBS_TASKS_GRAPH_DP_PARAMS_H

#include <string>

#include "tasks/common/train_params.h"

namespace deeplima
{
namespace graph_dp
{
namespace train
{

struct train_params_graph_dp_t : public deeplima::train::train_params_t
{
  std::string m_embeddings_fn;

  std::string m_tasks_string;

  std::vector<size_t> m_rnn_hidden_dims;
  size_t m_trainable_embeddings_dim;
  size_t m_trainable_embeddings_cutoff_freq; // ipm

  size_t m_sequence_length;

  bool m_input_includes_root;

  // The defaults below reproduce the models trained before they were options
  size_t m_tag_embd_dim;       // embedding dim of each tag input (upos, each feature)
  size_t m_arc_dim;            // hidden dim of the arc decoder
  size_t m_rel_dim;            // hidden dim of the label decoder (its cost: labels x dim^2 per word)
  float m_rnn_dropout;         // between BiLSTM layers
  float m_decoder_dropout;     // on the encoder output, before the decoders
  float m_word_dropout;        // probability to zero a word's form embedding
  float m_lr_decay;            // learning rate factor after an epoch without improvement
  float m_clip;                // gradient norm clipping (0: none)
  std::string m_select_on;     // dev metric that selects the model: arc (UAS) or las
  size_t m_min_sentence_len;   // shorter sentences are skipped
  uint64_t m_seed;             // of torch and of the batch sampling (0: not seeded)

  train_params_graph_dp_t()
    : m_tasks_string("arc"),
      m_rnn_hidden_dims({64}),
      m_trainable_embeddings_dim(0),
      m_trainable_embeddings_cutoff_freq(100),
      m_sequence_length(256),
      m_input_includes_root(true),
      m_tag_embd_dim(8),
      m_arc_dim(128),
      m_rel_dim(128),
      m_rnn_dropout(0),
      m_decoder_dropout(0),
      m_word_dropout(0),
      m_lr_decay(0.9),
      m_clip(0),
      m_select_on("arc"),
      m_min_sentence_len(4),
      m_seed(0) {
    m_batch_size = 8;
    m_max_epochs = 1000;
  }
};

} // namespace train
} // namespace graph_dp
} // namespace deeplima

#endif
