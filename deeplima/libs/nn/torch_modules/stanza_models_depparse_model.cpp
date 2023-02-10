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

#include "stanza_models_depparse_model.h"
#include <torch/nn/functional/activation.h>
#include <torch/nn/modules/embedding.h>

using namespace torch;
using namespace torch::indexing;
using namespace torch::nn;
using namespace torch::nn::functional;
using namespace torch::nn::utils::rnn;

namespace deeplima
{
namespace nets
{
namespace torch_modules
{

// def add_unsaved_module(name, module):
//     unsaved_modules += [name]
//     setattr(self, name, module)

StanzaDepparseParserImpl::StanzaDepparseParserImpl(int64_t word_emb_dim, int64_t tag_emb_dim, int64_t hidden_dim,
                                                   int64_t num_layers, float dropout, float rec_dropout,
                                                   std::shared_ptr<std::map<std::string, std::vector<std::string>>> vocab,
                                                   std::shared_ptr<std::vector<std::vector<std::string>>> feats_vocabs,
                                                   int64_t deep_biaff_hidden_dim,
                                                   bool linearize,
                                                   bool dist,
                                                   int64_t word_dropout)
  : torch::nn::Module(),
  m_vocab(vocab),
  m_feats_vocabs(feats_vocabs),
  m_unsaved_modules(),
  word_emb(EmbeddingOptions(0,0)),
  lemma_emb(EmbeddingOptions(0,0)),
  upos_emb(EmbeddingOptions(0,0)),
  xpos_emb(EmbeddingOptions(0,0)),
  ufeats_emb(),
  parserlstm(0, 0),
  drop_replacement(), parserlstm_h_init(), parserlstm_c_init(),
  unlabeled(2 * hidden_dim, 2 * hidden_dim, deep_biaff_hidden_dim, 1, dropout),
  deprel(2 * hidden_dim, 2 * hidden_dim, deep_biaff_hidden_dim, (*m_vocab)["deprel"].size(), dropout),
  linearization(),
  distance(),
  // criterion
  crit(CrossEntropyLossOptions().ignore_index(-1).reduction(torch::kSum)), // ignore padding
  drop(Dropout(dropout)),
  worddrop(word_dropout),
  word_emb_dim(word_emb_dim),
  num_layers(num_layers),
  hidden_dim(hidden_dim)
{
  // // self.args = args
  // // self.share_hid = share_hid
  // // self.unsaved_modules = []
  //
  // input layers
  int64_t input_size = 0;
  if (word_emb_dim > 0)
  {
      // frequent word embeddings
      word_emb = nn::Embedding(EmbeddingOptions((*m_vocab)["word"].size(), word_emb_dim).padding_idx(0));
      lemma_emb = nn::Embedding(EmbeddingOptions((*m_vocab)["lemma"].size(), word_emb_dim).padding_idx(0));
      input_size += word_emb_dim * 2;
  }
  if (tag_emb_dim > 0)
  {
      upos_emb = nn::Embedding(EmbeddingOptions((*m_vocab)["upos"].size(), tag_emb_dim).padding_idx(0));

      auto V = (*m_vocab)["xpos"];
      // if not isinstance((*m_vocab)["xpos"], CompositeVocab):
           xpos_emb = nn::Embedding(EmbeddingOptions((*m_vocab)["xpos"].size(), tag_emb_dim).padding_idx(0));
      // else:
      //     xpos_emb = nn.ModuleList()
      //
      //     for l in vocab["xpos"].lens():
      //         xpos_emb.append(nn::Embedding(EmbeddingOptions(l, tag_emb_dim).padding_idx(0));
      //

      for (const auto& vocab: *m_feats_vocabs)
              ufeats_emb->push_back(nn::Embedding(EmbeddingOptions(vocab.size(), tag_emb_dim).padding_idx(0)));

      input_size += tag_emb_dim * 2;
  }
  // if self.args["char"] and self.args["char_emb_dim"] > 0:
  //     charmodel = CharacterModel(args, vocab)
  //     trans_char = nn::Linear(self.args["char_hidden_dim"], self.args["transformed_dim"], bias=False)
  //     input_size += self.args["transformed_dim"]
  //
  // if self.args["pretrain"]:
  //     # pretrained embeddings, by default this won't be saved into model file
  //     add_unsaved_module("pretrained_emb", nn::Embedding.from_pretrained(torch.from_numpy(emb_matrix), freeze=True))
  //     trans_pretrained = nn.Linear(emb_matrix.shape[1], self.args["transformed_dim"], bias=False)
  //     input_size += self.args["transformed_dim"]

  // recurrent layers
  parserlstm = HighwayLSTM(input_size, hidden_dim, num_layers, true, true, dropout, true,
                           rec_dropout, torch::tanh);
  parserlstm->register_parameter("drop_replacement", torch::randn(input_size) / sqrt(input_size), false);
  parserlstm->register_parameter("parserlstm_h_init", torch::zeros({2 * num_layers, 1, hidden_dim}), false);
  parserlstm->register_parameter("parserlstm_c_init", torch::zeros({2 * num_layers, 1, hidden_dim}), false);

  // classifiers
  if (linearize)
      linearization = std::make_unique<DeepBiaffineScorer>(2 * hidden_dim, 2 * hidden_dim, deep_biaff_hidden_dim, 1, dropout);
  if (dist)
      distance = std::make_unique<DeepBiaffineScorer>(2 * hidden_dim, 2 * hidden_dim, deep_biaff_hidden_dim, 1, dropout);
}

PackedSequence StanzaDepparseParserImpl::pack(torch::Tensor x, torch::Tensor sentlens)
{
    return pack_padded_sequence(x, sentlens, /*batch_first=T*/true);
}

std::pair<float, std::vector<torch::Tensor>> StanzaDepparseParserImpl::forward(
    torch::Tensor word, torch::Tensor word_mask, torch::Tensor wordchars, torch::Tensor wordchars_mask,
    torch::Tensor upos, torch::Tensor xpos, torch::Tensor ufeats, torch::Tensor pretrained,
    torch::Tensor lemma, torch::Tensor head, torch::Tensor deprels, torch::Tensor word_orig_idx,
    torch::Tensor sentlens, torch::Tensor wordlens)
{
  std::vector<std::tuple<PackedSequence, PackedSequence>> inputs;
  // if self.args["pretrain"]:
  //     pretrained_emb = pretrained_emb(pretrained)
  //     pretrained_emb = trans_pretrained(pretrained_emb)
  //     pretrained_emb = pack(pretrained_emb)
  //     inputs += [pretrained_emb]
  //
  // #def pad(x):
  // #    return pad_packed_sequence(PackedSequence(x, pretrained_emb.batch_sizes), batch_first=True)[0]
  //
  if (word_emb_dim > 0)
  {
      auto word_embed = word_emb(word);
      auto packed_word_embed = pack(word_embed, sentlens);
      auto lemma_embed = lemma_emb(lemma);
      auto packed_lemma_embed = pack(lemma_embed, sentlens);
      inputs.push_back({packed_word_embed, packed_lemma_embed});
  }
  //
  // if tag_emb_dim > 0:
  //     pos_emb = upos_emb(upos)
  //
  //     if isinstance((*m_vocab)["xpos"], CompositeVocab):
  //         for i in range(len((*m_vocab)["xpos"])):
  //             pos_emb += xpos_emb[i](xpos[:, :, i])
  //     else:
  //         pos_emb += xpos_emb(xpos)
  //     pos_emb = pack(pos_emb)
  //
  //     feats_emb = 0
  //     for i in range(len((*m_vocab)["feats"])):
  //         feats_emb += ufeats_emb[i](ufeats[:, :, i])
  //     feats_emb = pack(feats_emb)
  //
  //     inputs += [pos_emb, feats_emb]
  //
  // if self.args["char"] and self.args["char_emb_dim"] > 0:
  //     char_reps = charmodel(wordchars, wordchars_mask, word_orig_idx, sentlens, wordlens)
  //     char_reps = PackedSequence(trans_char(drop(char_reps.data)), char_reps.batch_sizes)
  //     inputs += [char_reps]
  //
  std::vector<torch::Tensor> lstm_inputs_vec;
  for (auto x: inputs)
  {
    // lstm_inputs_vec.push_back(x.data());
  }
  auto lstm_inputs = torch::cat(lstm_inputs_vec, 1);
  //
  lstm_inputs = worddrop(lstm_inputs, drop_replacement);
  // lstm_inputs = drop(lstm_inputs);
  //
  // lstm_inputs = PackedSequence(lstm_inputs, inputs.index({0}).batch_sizes());
  //
// torch::Tensor input, torch::Tensor seqlens, torch::Tensor hx=torch::Tensor()
  auto [lstm_outputs, none] = parserlstm(lstm_inputs, sentlens,
                                         std::make_tuple(
                                           parserlstm_h_init.expand({2 * num_layers, word.size(0), hidden_dim}).contiguous(),
                                           parserlstm_c_init.expand({2 * num_layers, word.size(0), hidden_dim}).contiguous()));
  // lstm_outputs, _ = pad_packed_sequence(lstm_outputs, batch_first=True)
  //
  auto unlabeled_scores = unlabeled(drop(lstm_outputs), drop(lstm_outputs)).squeeze(3);
  auto deprel_scores = deprel(drop(lstm_outputs), drop(lstm_outputs));
  //
  // #goldmask = head.new_zeros(*head.size(), head.size(-1)+1, dtype=torch.uint8)
  // #goldmask.scatter_(2, head.unsqueeze(2), 1)
  //
  // if self.args["linearization"] or self.args["distance"]:
  //     head_offset = torch.arange(word.size(1), device=head.device).view(1, 1, -1).expand(word.size(0), -1, -1) - torch.arange(word.size(1), device=head.device).view(1, -1, 1).expand(word.size(0), -1, -1)
  //
  // if self.args["linearization"]:
  //     lin_scores = linearization(drop(lstm_outputs), drop(lstm_outputs)).squeeze(3)
  //     unlabeled_scores += F.logsigmoid(lin_scores * torch.sign(head_offset).float()).detach()
  //
  // if self.args["distance"]:
  //     dist_scores = distance(drop(lstm_outputs), drop(lstm_outputs)).squeeze(3)
  //     dist_pred = 1 + F.softplus(dist_scores)
  //     dist_target = torch.abs(head_offset)
  //     dist_kld = -torch.log((dist_target.float() - dist_pred)**2/2 + 1)
  //     unlabeled_scores += dist_kld.detach()
  //
  auto diag = torch::eye(head.size(-1)+1, torch::dtype(torch::kUInt8).device(head.device())).unsqueeze(0);
  unlabeled_scores.masked_fill_(diag, -std::numeric_limits<float>::infinity());

  float loss;
  std::vector<torch::Tensor> preds;

  if (is_training())
  {
      unlabeled_scores = unlabeled_scores.index({None, Slice(1), None}); // exclude attachment for the root symbol
      unlabeled_scores = unlabeled_scores.masked_fill(word_mask.unsqueeze(1), -std::numeric_limits<float>::infinity());
      auto unlabeled_target = head.masked_fill(word_mask.index({None, Slice(1)}), -1);
      loss = crit(unlabeled_scores.contiguous().view({-1, unlabeled_scores.size(2)}), unlabeled_target.view(-1)).item<float>();

      deprel_scores = deprel_scores.index({None, Slice(1)}); // exclude attachment for the root symbol
      // #deprel_scores = deprel_scores.masked_select(goldmask.unsqueeze(3)).view(-1, len((*m_vocab)["deprel"]))
      deprel_scores = torch::gather(deprel_scores, 2, head.unsqueeze(2).unsqueeze(3).expand({-1, -1, -1, (*m_vocab)["deprel"].size()})).view({-1, (*m_vocab)["deprel"].size()});
      auto deprel_target = deprels.masked_fill(word_mask.index({None, Slice(1)}), -1);
      loss += crit(deprel_scores.contiguous(), deprel_target.view(-1)).item<float>();

      // if (self.args["linearization"])
      // {
      //     // #lin_scores = lin_scores[:, 1:].masked_select(goldmask)
      //     lin_scores = torch::gather(lin_scores[:, 1:], 2, head.unsqueeze(2)).view(-1)
      //     lin_scores = torch::cat([-lin_scores.unsqueeze(1)/2, lin_scores.unsqueeze(1)/2], 1)
      //     // #lin_target = (head_offset[:, 1:] > 0).long().masked_select(goldmask)
      //     lin_target = torch::gather((head_offset[:, 1:] > 0).long(), 2, head.unsqueeze(2))
      //     loss += crit(lin_scores.contiguous(), lin_target.view(-1))
      // }
      //
      // if (self.args["distance"])
      // {
      //     // #dist_kld = dist_kld[:, 1:].masked_select(goldmask)
      //     dist_kld = torch::gather(dist_kld[:, 1:], 2, head.unsqueeze(2))
      //     loss -= dist_kld.sum()
      // }

      loss /= wordchars.size(0); // # number of words
  }
  else
  {
      loss = 0;
      auto X = log_softmax(unlabeled_scores, 2).detach().cpu();
      preds.push_back(log_softmax(unlabeled_scores, 2).detach().cpu());
      preds.push_back(std::get<1>(torch::max(deprel_scores, 3)).detach().cpu());
  }
  return {loss, preds};
}

} // torch_modules
} // nets
} // deeplima
