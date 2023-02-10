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

#ifndef DEEPLIMA_LIBS_NN_STANZA_MODELS_DEPPARSE_MODEL_H
#define DEEPLIMA_LIBS_NN_STANZA_MODELS_DEPPARSE_MODEL_H

#include <torch/torch.h>
#include <torch/nn/modules/dropout.h>
#include <torch/nn/modules/embedding.h>
#include <torch/nn/modules/linear.h>
#include <torch/nn/modules/rnn.h>
#include <torch/nn/utils/rnn.h>
#include <ATen/ATen.h>
#include <ATen/Functions.h>

#include <algorithm>

namespace deeplima
{
namespace nets
{
namespace torch_modules
{

/// A word dropout layer that's designed for embedded inputs (e.g., any inputs to an LSTM layer).
/// Given a batch of embedded inputs, this layer randomly set some of them to be a replacement state.
/// Note that this layer assumes the last dimension of the input to be the hidden dimension of a unit.
class WordDropoutImpl: torch::nn::Module
{
public:
  WordDropoutImpl() = default;
  WordDropoutImpl(int64_t dropprob) :
      torch::nn::Module(),
      dropprob(dropprob)
  {
  }

  torch::Tensor forward(torch::Tensor x, torch::Tensor replacement=torch::Tensor())
  {
      if (!is_training() || dropprob == 0)
          return x;

      auto masksize = std::vector<int64_t>(x.numel());
      for (int64_t y = 0; y < x.numel(); y++) masksize[y] = y;
      masksize[-1] = 1;
      // TODO put to correct device
      auto dropmask = torch::rand(masksize) < dropprob;

      auto res = x.masked_fill(dropmask, 0);
      if (replacement.numel() == 0)
          res = res + dropmask.to(torch::kFloat) * replacement;

      return res;
  }

  std::string extra_repr()
  {
    return std::string("p=") + std::to_string(dropprob);
  }

  int64_t dropprob;
};
TORCH_MODULE(WordDropout);

/// An LSTM implementation that supports recurrent dropout
class LSTMwRecDropoutImpl: public torch::nn::Module
{
public:
  LSTMwRecDropoutImpl() = default;
  LSTMwRecDropoutImpl(int64_t input_size, int64_t hidden_size, int64_t num_layers, bool bias=true,
                  bool batch_first=false, float dropout=0, bool bidirectional=false,
                  bool pad=false, float rec_dropout=0):
      torch::nn::Module(),
      batch_first(batch_first),
      pad(pad),
      num_layers(num_layers),
      hidden_size(hidden_size),
      dropout(dropout),
      drop(torch::nn::DropoutOptions().p(dropout).inplace(true)),
      rec_drop(torch::nn::DropoutOptions().p(rec_dropout).inplace(true)),
      num_directions(bidirectional ? 2 : 1),
      cells()
  {
      for (int64_t l = 0; l < num_layers; l++)
      {
          auto in_size = l == 0 ? input_size : num_directions * hidden_size;
          for (int64_t d = 0; d < num_directions; d++)
              cells->push_back(torch::nn::LSTMCell(torch::nn::LSTMCellOptions(in_size, hidden_size).bias(bias)));
      }
  }

  /// RNN loop for one layer in one direction with recurrent dropout
  /// Assumes input is PackedSequence, returns PackedSequence as well
  std::pair<torch::Tensor, std::pair<torch::Tensor, torch::Tensor>> rnn_loop(torch::Tensor x,
                                                                torch::Tensor batch_sizes,
                                                                torch::nn::LSTMCellImpl* cell,
                                                                std::vector<torch::Tensor> inits,
                                                                bool reverse=false)
  {
      auto batch_size = batch_sizes[0].item();
      std::vector<std::vector<torch::Tensor>> states;
      for (auto init: inits)
      {
        auto l = init.split(std::vector<int64_t>(batch_size.to<int64_t>(), 1));
        states.push_back(l);
      }
      auto h_drop_mask = x.new_ones({batch_size.to<int64_t>(), hidden_size});
      h_drop_mask = rec_drop(h_drop_mask);
      std::vector<torch::Tensor> resh;

      if (!reverse)
      {
          int64_t st = 0;
          for (int i = 0; i < batch_sizes.numel(); i++)
          {
              auto bs = *(batch_sizes.data_ptr<int64_t>()+i);
              // states[0][:bs],
              // TODO use C++20 ranges instead of temporary slice0 and slice1 vectors with copies
              std::vector<torch::Tensor> slice0, slice1;
              for (int64_t p = 0; p < bs; p++)
              {
                slice0.push_back(states[0][p]);
                slice1.push_back(states[1][p]);
              }
              auto s1 = cell->forward(x.index({torch::indexing::Slice(st, st+bs)}),
                             std::make_tuple(
                               torch::cat(slice0, 0) * h_drop_mask.index({torch::indexing::Slice(0, bs)}),
                               torch::cat(slice1, 0)
                                  ));
              resh.push_back(std::get<0>(s1));
              for (int64_t j = 0; j < bs; j++)
              {
                states[0][j] = std::get<0>(s1).index({j}).unsqueeze(0);
                states[1][j] = std::get<1>(s1).index({j}).unsqueeze(0);
              }
              st += bs;
          }
      }
      else
      {
          auto en = x.size(0);
          for (int64_t i = batch_sizes.size(0)-1; i > 0; i--)
          {
              auto bs = *(batch_sizes.data_ptr<int64_t>()+i);
              // TODO use C++20 ranges instead of temporary slice0 and slice1 vectors with copies
              std::vector<torch::Tensor> slice0, slice1;
              for (int64_t p = 0; p < bs; p++)
              {
                slice0.push_back(states[0][p]);
                slice1.push_back(states[1][p]);
              }
              auto s1 = cell->forward(x.index({torch::indexing::Slice(en-bs, en)}),
                             std::make_tuple<torch::Tensor, torch::Tensor>(
                               torch::cat(slice0, 0) * h_drop_mask.index({torch::indexing::Slice(0, bs)}),
                               torch::cat(slice1, 0)));
              resh.push_back(std::get<0>(s1));
              for (int64_t j = 0; j < bs; j++)
              {
                  states[0][j] = std::get<0>(s1).index({j}).unsqueeze(0);
                  states[1][j] = std::get<1>(s1).index({j}).unsqueeze(0);
              }
              en -= bs;
          }
          std::reverse(resh.begin(), resh.end());
      }

      return std::make_pair(torch::cat(resh, 0), std::make_pair(torch::cat(states[0], 0), torch::cat(states[0], 0)));
  }

  std::tuple<torch::nn::utils::rnn::PackedSequence, std::tuple<torch::Tensor, torch::Tensor>> forward(
    torch::nn::utils::rnn::PackedSequence input,
    torch::optional<std::tuple<torch::Tensor, torch::Tensor>> hx = {})
  {
    std::pair<std::vector<torch::Tensor>, std::vector<torch::Tensor>> all_states;
    auto inputdata = input.data();
    auto batch_sizes = input.batch_sizes();
    for (int64_t l = 0; l < num_layers; l++)
    {
      std::vector<torch::Tensor> new_input;

      if (dropout > 0 && l > 0)
        inputdata = drop(inputdata);
      for (int64_t d = 0; d < num_directions; d++)
      {
        auto idx = l * num_directions + d;
        auto cell = cells[idx]->as<torch::nn::LSTMCell>();
        // x.new_ones({batch_size.to<int64_t>(), hidden_size})
        std::vector<torch::Tensor> inits;
        if (hx)
          inits = {std::get<0>(*hx).index({idx}), std::get<1>(*hx).index({idx})};
        else
          inits = {
            input.data().new_zeros({input.batch_sizes().index({0}).item().to<int64_t>(), hidden_size},
                                   torch::TensorOptions().requires_grad(false)),
            input.data().new_zeros({input.batch_sizes().index({0}).item().to<int64_t>(), hidden_size},
                                   torch::TensorOptions().requires_grad(false))
          };
        auto loop_result = rnn_loop(inputdata, batch_sizes, cell, inits, (d == 1));
        auto out = std::get<0>(loop_result);
        auto states = std::get<1>(loop_result);
        new_input.push_back(out);
        std::get<0>(all_states).push_back(std::get<0>(states).unsqueeze(0));
        std::get<1>(all_states).push_back(std::get<1>(states).unsqueeze(0));
      }
      if (num_directions > 1)
        // # concatenate both directions
        inputdata = torch::cat(new_input, 1);
      else
        inputdata = new_input[0];
    }
    input = torch::nn::utils::rnn::PackedSequence(inputdata, batch_sizes);

    return {input, {torch::cat(std::get<0>(all_states), 0), torch::cat(std::get<1>(all_states))} };
  }

  bool batch_first;
  bool pad;
  int64_t num_layers;
  int64_t hidden_size;

  float dropout;
  torch::nn::Dropout drop;
  torch::nn::Dropout rec_drop;

  int64_t num_directions;

  torch::nn::ModuleList cells;
};
TORCH_MODULE(LSTMwRecDropout);

class PackedLSTMImpl : public torch::nn::Module
{
public:
    PackedLSTMImpl() = default;
    PackedLSTMImpl(int64_t input_size, int64_t hidden_size, int64_t num_layers,
                   bool bias=true, bool batch_first=false,
                   float dropout=0, bool bidirectional=false,
                   bool pad=false, float rec_dropout=0):
        torch::nn::Module(),
        batch_first(batch_first),
        pad(pad)
    {
        if (rec_dropout == 0)
        {
            // # use the fast, native LSTM implementation
            lstm = std::make_shared<torch::nn::LSTMImpl>(
              torch::nn::LSTMOptions(input_size, hidden_size).num_layers(num_layers).batch_first(batch_first)
                .bidirectional(bidirectional).bias(bias).dropout(dropout));
        }
        else
        {
            lstm = std::make_shared<LSTMwRecDropoutImpl>(input_size, hidden_size, num_layers, bias, batch_first,
                                   dropout, bidirectional, rec_dropout);
        }
    }

    std::tuple<torch::Tensor, std::tuple<torch::Tensor, torch::Tensor>> forward(torch::Tensor input, torch::Tensor lengths,
                          torch::optional<std::tuple<torch::Tensor, torch::Tensor>> hx = {})
    {
      // std::tuple<Tensor, Tensor> torch::nn::utils::rnn::pad_packed_sequence(PackedSequence sequence, bool batch_first = false, double padding_value = 0.0, c10::optional<int64_t> total_length = torch::nullopt)
      // LSTM forward return type: std::tuple<Tensor, std::tuple<Tensor, Tensor>>
      auto res = lstm.forward<std::tuple<torch::Tensor, std::tuple<torch::Tensor, torch::Tensor>>>(input, hx);
      // if (pad)
      //     res = std::make_tuple(torch::nn::utils::rnn::pad_packed_sequence(res[0], batch_first=batch_first)[0], res[1]);
      return res;
    }

    std::tuple<torch::nn::utils::rnn::PackedSequence, std::tuple<torch::Tensor, torch::Tensor>> forward_with_packed_input(const torch::nn::utils::rnn::PackedSequence &packed_input, torch::Tensor lengths,
                          torch::optional<std::tuple<torch::Tensor, torch::Tensor>> hx = {})
    {
      // std::tuple<Tensor, Tensor> torch::nn::utils::rnn::pad_packed_sequence(PackedSequence sequence, bool batch_first = false, double padding_value = 0.0, c10::optional<int64_t> total_length = torch::nullopt)
      // auto input = torch::nn::utils::rnn::pack_padded_sequence(packed_input, lengths, batch_first);
      auto effective = lstm.ptr<torch::nn::LSTMImpl>();
      if (effective)
      {
        auto res = effective->forward_with_packed_input(packed_input, hx);
        // if (pad)
        //     res = std::make_tuple(std::get<0>(torch::nn::utils::rnn::pad_packed_sequence(std::get<0>(res), batch_first)),
        //                           std::get<1>(res));
        return res;
      }
      else
      {
        auto effective_rec = lstm.ptr<LSTMwRecDropoutImpl>();
        // LSTM forward_with_packed_input return type: std::tuple<torch::nn::utils::rnn::PackedSequence, std::tuple<torch::Tensor, torch::Tensor>>
        auto res = effective_rec->forward(packed_input, hx);
        // if (pad)
        //     res = std::make_tuple(std::get<0>(torch::nn::utils::rnn::pad_packed_sequence(std::get<0>(res), batch_first)),
        //                           std::get<1>(res));
        return res;
      }
    }

    torch::nn::AnyModule lstm;
    bool batch_first;
    bool pad;
};
TORCH_MODULE(PackedLSTM);

/// Highway LSTM network, does NOT use the HLSTMCell above
/// A Highway LSTM network, as used in the original Tensorflow version of the Dozat parser. Note that this
/// is independent from the HLSTMCell above.
class HighwayLSTMImpl: public torch::nn::Module
{
public:
  HighwayLSTMImpl() = default;
  HighwayLSTMImpl(int64_t input_size, int64_t hidden_size,
                  int64_t num_layers=1, bool bias=true, bool batch_first=false,
                  float dropout=0, bool bidirectional=false, float rec_dropout=0,
                  std::function<torch::Tensor(const torch::Tensor&)> highway_func=nullptr,
                  bool pad=false):
              torch::nn::Module(),
      input_size(input_size),
      hidden_size(hidden_size),
      num_layers(num_layers),
      bias(bias),
      batch_first(batch_first),
      dropout(dropout),
      // dropout_state({}
      bidirectional(bidirectional),
      num_directions(bidirectional ? 2 : 1),
      highway_func(highway_func),
      pad(pad),
      lstm(),
      highway(),
      gate(),
      drop(torch::nn::DropoutOptions().p(dropout).inplace(true))
  {
      auto in_size = input_size;
      for (int64_t l = 0; l < num_layers; l++)
      {
          lstm->push_back(PackedLSTM(in_size, hidden_size, 1, bias,
              batch_first, 0, bidirectional, rec_dropout));
          highway->push_back(torch::nn::Linear(in_size, hidden_size * num_directions));
          gate->push_back(torch::nn::Linear(in_size, hidden_size * num_directions));
          // highway[-1]->bias.data.zero_();
          // gate[-1]->bias.data.zero_();
          in_size = hidden_size * num_directions;
      }
  }

  std::tuple<torch::Tensor, std::tuple<torch::Tensor, torch::Tensor>> forward(
    torch::Tensor input, torch::Tensor seqlens,
    std::tuple<torch::Tensor, torch::Tensor> hx={torch::Tensor(),torch::Tensor()})
  {
      highway_func = highway_func.target<torch::Tensor(const torch::Tensor&)>() == nullptr ? [](const torch::Tensor& t) { return t; } : highway_func;

      std::vector<torch::Tensor> hs;
      std::vector<torch::Tensor> cs;
      auto packed_sequence = torch::nn::utils::rnn::pack_padded_sequence(input, seqlens, batch_first);

      for (int64_t l = 0 ; l < num_layers; l++)
      {
          if (l > 0)
              packed_sequence = torch::nn::utils::rnn::PackedSequence(drop(packed_sequence.data()),
                                                                      packed_sequence.batch_sizes(),
                                                                      packed_sequence.sorted_indices(),
                                                                      packed_sequence.unsorted_indices());
          auto layer_hx = std::get<0>(hx).numel() > 0 ?
              std::make_tuple(std::get<0>(hx).index({torch::indexing::Slice(l * num_directions,(l+1)*num_directions)}),
                              std::get<1>(hx).index({torch::indexing::Slice(l * num_directions, (l+1)*num_directions)})) :
              std::make_tuple(torch::Tensor(), torch::Tensor());
          auto X = (lstm[l]->as<torch::nn::LSTMImpl>()) ?
            lstm[l]->as<torch::nn::LSTMImpl>()->forward_with_packed_input(packed_sequence, layer_hx) :
            lstm[l]->as<LSTMwRecDropoutImpl>()->forward(packed_sequence, layer_hx);
          auto h = std::get<0>(X);
          auto t = std::get<1>(X);
          auto ht = std::get<0>(t);
          auto ct = std::get<1>(t);
          hs.push_back(ht);
          cs.push_back(ct);

          packed_sequence = torch::nn::utils::rnn::PackedSequence(
            (h.data() + torch::sigmoid(gate[l]->as<torch::nn::Linear>()->forward(packed_sequence.data()))
                          * highway_func(highway[l]->as<torch::nn::Linear>()->forward(packed_sequence.data()))),
            packed_sequence.batch_sizes(),
            packed_sequence.sorted_indices(),
            packed_sequence.unsorted_indices());
      }

      // if (pad)
      //     input = pad_packed_sequence(packed_sequence, batch_first=batch_first)[0];
      return {input, {torch::cat(hs, 0), torch::cat(cs, 0)}};
  }

  int64_t input_size;
  int64_t hidden_size;
  int64_t num_layers;
  bool bias;
  bool batch_first;
  float dropout;
  // dropout_state;
  bool bidirectional;
  int64_t num_directions;
  std::function<torch::Tensor(const torch::Tensor&)> highway_func;
  bool pad;

  torch::nn::ModuleList lstm;
  torch::nn::ModuleList highway;
  torch::nn::ModuleList gate;
  torch::nn::Dropout drop;

};
TORCH_MODULE(HighwayLSTM);

/// A bilinear module that deals with broadcasting for efficient memory usage.
/// Input: tensors of sizes (N x L1 x D1) and (N x L2 x D2)
/// Output: tensor of size (N x L1 x L2 x O)
class PairwiseBilinearImpl : public torch::nn::Module
{
public:
    PairwiseBilinearImpl() = default;
    PairwiseBilinearImpl(int64_t input1_size, int64_t input2_size, int output_size, bool bias=true):
        torch::nn::Module()
    {
        // TODO check lines below wrt to original Stanza's python code
        // this->input1_size = input1_size;
        // this->input2_size = input2_size;
        this->output_size = output_size;

        this->register_parameter("weight", torch::zeros({input1_size, input2_size, output_size}), false);
        register_parameter("bias", bias ? torch::zeros({output_size}) :  torch::zeros({}), false);
    }

    torch::Tensor forward(torch::Tensor input1, torch::Tensor input2)
    {
        auto input1_size = input1.sizes();
        auto input2_size = input2.sizes();
        // TODO check lines below wrt to original Stanza's python code
        // std::vector<uint64_t> output_size = {input1_size[0], input1_size[1], input2_size[1], this->output_size};

        // # ((N x L1) x D1) * (D1 x (D2 x O)) -> (N x L1) x (D2 x O)
        auto intermediate = at::mm(input1.view({-1, input1_size[-1]}),
                                     named_parameters()["weight"].view({-1, this->input2_size * this->output_size}));
        // # (N x L2 x D2) -> (N x D2 x L2)
        input2 = input2.transpose(1, 2);
        // # (N x (L1 x O) x D2) * (N x D2 x L2) -> (N x (L1 x O) x L2)
        auto output = intermediate.view({input1_size[0], input1_size[1] * this->output_size, input2_size[2]}).bmm(input2);
        // # (N x (L1 x O) x L2) -> (N x L1 x L2 x O)
        output = output.view({input1_size[0], input1_size[1], this->output_size, input2_size[1]}).transpose(2, 3);

        return output;
    }
    int64_t input1_size, input2_size;
    int64_t output_size;
};
TORCH_MODULE(PairwiseBilinear);

class BiaffineScorerImpl : public torch::nn::Module
{
public:
    BiaffineScorerImpl() = default;
    BiaffineScorerImpl(int64_t input1_size, int64_t input2_size, int64_t output_size):
        torch::nn::Module(),
        W_bilin(input1_size + 1, input2_size + 1, output_size)
    {

        W_bilin->weight.data().zero_();
        W_bilin->bias.data().zero_();
    }

    torch::Tensor forward(torch::Tensor input1, torch::Tensor input2)
    {
        // TODO uncomment and correct below
        // input1 = torch::cat([input1, input1.new_ones(*input1.sizes()[:-1], 1)], input1.sizes().size()-1);
        // input2 = torch::cat([input2, input2.new_ones(*input2.sizes()[:-1], 1)], input2.sizes().size()-1);
        return W_bilin(input1, input2);
    }
    torch::nn::Bilinear W_bilin;
};
TORCH_MODULE(BiaffineScorer);

class PairwiseBiaffineScorerImpl : public torch::nn::Module
{
public:
    PairwiseBiaffineScorerImpl() = default;
    PairwiseBiaffineScorerImpl(int64_t input1_size, int64_t input2_size, int64_t output_size):
        torch::nn::Module(),
        W_bilin(input1_size + 1, input2_size + 1, output_size)
    {
        // TODO uncomment and correct below
        // W_bilin->weight.data.zero_()
        // W_bilin->bias.data.zero_()
    }

    torch::Tensor forward(torch::Tensor input1, torch::Tensor input2)
    {
        // TODO uncomment and correct below
        // auto input1 = torch::cat([input1, input1.new_ones(*input1.size()[:-1], 1)], len(input1.size())-1)
        // auto input2 = torch::cat([input2, input2.new_ones(*input2.size()[:-1], 1)], len(input2.size())-1)
        return W_bilin(input1, input2);
    }

    PairwiseBilinear W_bilin;
};
TORCH_MODULE(PairwiseBiaffineScorer);

class DeepBiaffineScorerImpl : public torch::nn::Module
{
public:
    DeepBiaffineScorerImpl():
        scorer(),
        W1(0,0), W2(0,0) {}
    DeepBiaffineScorerImpl(int64_t input1_size, int64_t input2_size,
                       int64_t hidden_size, int64_t output_size,
                       float dropout_value=0, bool pairwise=true,
                       std::function<torch::Tensor(const torch::Tensor&)> hidden_func=at::relu):
        torch::nn::Module(),
        scorer(),
        dropout(dropout_value),
        W1(input1_size, hidden_size),
        W2(input2_size, hidden_size),
        m_hidden_func(hidden_func)
    {
        if (pairwise)
            scorer = std::make_shared<PairwiseBiaffineScorerImpl>(hidden_size, hidden_size, output_size);
        else
            scorer = std::make_shared<BiaffineScorerImpl>(hidden_size, hidden_size, output_size);
    }

    torch::Tensor forward(torch::Tensor input1, torch::Tensor input2)
    {
        return  scorer.forward(dropout(m_hidden_func(W1(input1))), dropout(m_hidden_func(W2(input2))));
    }

    torch::nn::AnyModule scorer;
    torch::nn::Dropout dropout;
    torch::nn::Linear W1, W2;
    std::function<torch::Tensor(const torch::Tensor&)> m_hidden_func;
};
TORCH_MODULE(DeepBiaffineScorer);

class StanzaDepparseParserImpl : public torch::nn::Module
{
public:
  StanzaDepparseParserImpl() = default;
  StanzaDepparseParserImpl(int64_t word_emb_dim, int64_t tag_emb_dim, int64_t hidden_dim,
                           int64_t num_layers, float dropout, float rec_dropout,
                           std::shared_ptr<std::map<std::string, std::vector<std::string>>> vocab,
                           std::shared_ptr<std::vector<std::vector<std::string>>> feats_vocabs,
                           int64_t deep_biaff_hidden_dim,
                           bool linearize,
                           bool dist,
                           int64_t word_dropout);

  std::pair<float, std::vector<torch::Tensor>> forward(
    torch::Tensor word, torch::Tensor word_mask, torch::Tensor wordchars, torch::Tensor wordchars_mask,
    torch::Tensor upos, torch::Tensor xpos, torch::Tensor ufeats, torch::Tensor pretrained,
    torch::Tensor lemma, torch::Tensor head, torch::Tensor deprels, torch::Tensor word_orig_idx,
    torch::Tensor sentlens, torch::Tensor wordlens);

private:
  torch::nn::utils::rnn::PackedSequence pack(torch::Tensor x, torch::Tensor sentlens);

  std::shared_ptr<std::map<std::string, std::vector<std::string>>> m_vocab;
  std::shared_ptr<std::vector<std::vector<std::string>>> m_feats_vocabs;
  std::vector<std::string> m_unsaved_modules;
  torch::nn::Embedding word_emb, lemma_emb, upos_emb, xpos_emb;
  torch::nn::ModuleList ufeats_emb;
  HighwayLSTM parserlstm;
  torch::Tensor drop_replacement, parserlstm_h_init, parserlstm_c_init;
  DeepBiaffineScorer unlabeled;
  DeepBiaffineScorer deprel;
  std::unique_ptr<DeepBiaffineScorer> linearization;
  std::unique_ptr<DeepBiaffineScorer> distance;
  torch::nn::CrossEntropyLoss crit;
  torch::nn::Dropout drop;
  WordDropout worddrop;
  int64_t word_emb_dim;
  int64_t num_layers;
  int64_t hidden_dim;
};

TORCH_MODULE(StanzaDepparseParser);

} // torch_modules
} // nets
} // deeplima

#endif // DEEPLIMA_LIBS_NN_STANZA_MODELS_DEPPARSE_MODEL_H
