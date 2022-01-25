/*
    Copyright 2021 CEA LIST

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

#include <string>

#include "seq2seq_for_lemmatization.h"

using namespace std;
using namespace torch;
using torch::indexing::Slice;

namespace deeplima
{
namespace lemmatization
{
namespace train
{

void Seq2SeqLemmatizerImpl::train(const train_params_lemmatization_t& params,
                                  const vector<TorchMatrix<int64_t>>& train_input,
                                  const vector<vector<TorchMatrix<int64_t>>>& train_input_cat,
                                  const vector<TorchMatrix<int64_t>>& train_gold,
                                  const vector<TorchMatrix<int64_t>>& eval_input,
                                  const vector<vector<TorchMatrix<int64_t>>>& eval_input_cat,
                                  const vector<TorchMatrix<int64_t>>& eval_gold,
                                  torch::optim::Optimizer& opt,
                                  const torch::Device& device)
{
  assert(train_input.size() == train_gold.size());
  assert(eval_input.size() == eval_gold.size());

  set_tags(params.m_tags);

  double best_eval_accuracy = 0;
  double best_eval_loss = numeric_limits<double>::max();
  size_t count_below_best = 0;
  double lr_copy = 0;
  for (size_t e = 1; e < params.m_max_epochs; e++)
  {
    cerr << endl << "********** EPOCH " << e << " **********" << endl;
    nets::epoch_stat_t train_stat, eval_stat;
    Module::train(true);

    for (size_t i = 0; i < train_input.size(); ++i)
    {
      const TorchMatrix<int64_t>& input = train_input[i];
      const vector<TorchMatrix<int64_t>> input_cat = train_input_cat[i];
      const TorchMatrix<int64_t>& gold = train_gold[i];

      assert(input.get_max_feat() == gold.get_max_feat());
      assert(input.get_max_feat() == input_cat[0].get_max_feat());
      train_on_subset(params, input, input_cat, gold, opt, device);
    }

    //evaluate(eval_input, eval_gold, eval_stat, device);

    if (!params.m_output_model_name.empty())
    {
      torch::save(*this, params.m_output_model_name + ".pt");
    }
  }
}

void Seq2SeqLemmatizerImpl::train_on_subset(const train_params_lemmatization_t& params,
                                            const TorchMatrix<int64_t>& train_input,
                                            const vector<TorchMatrix<int64_t>> train_input_cat,
                                            const TorchMatrix<int64_t>& train_gold,
                                            torch::optim::Optimizer& opt,
                                            const torch::Device& device)
{
  //cerr << train_input.get_tensor().sizes() << endl;
  //cerr << train_gold.get_tensor().sizes() << endl;
  //cerr << train_input.size() << endl;
  const auto& input_tensor = train_input.get_tensor();
  const auto& gold_tensor = train_gold.get_tensor();
  int64_t n_samples = input_tensor.sizes()[1];
  nets::epoch_stat_t stat;
  for (int64_t i = 0; i < n_samples; i += params.m_batch_size)
  {
    int64_t end_sample = params.m_batch_size > n_samples ? n_samples : i + params.m_batch_size;
    assert(i < end_sample);
    const auto batch_input = input_tensor.index({ Slice(), Slice(i, end_sample) });
    const auto batch_gold = gold_tensor.index({ Slice(), Slice(i, end_sample) });

    vector<TorchMatrix<int64_t>::tensor_t> batch_input_cat(train_input_cat.size());
    for (size_t feat_idx = 0; feat_idx < train_input_cat.size(); ++feat_idx)
    {
      const auto& input_cat_tensor = train_input_cat[feat_idx].get_tensor();
      batch_input_cat[feat_idx] = input_cat_tensor.index({ Slice(), Slice(i, end_sample) });
    }

    train_batch({ "output" }, batch_input, batch_input_cat, batch_gold, opt, stat, device);
  }

  if (stat["output"].m_items > 0)
  {
    stat["output"].m_accuracy = float(stat["output"].m_correct) / stat["output"].m_items;
  }
  cerr << " LEN: " << input_tensor.sizes()[0]
       << " LOSS: " << stat["output"].m_loss
       << " ACC: " << stat["output"].m_accuracy
       << " CORRECT: " << stat["output"].m_correct
       << " TOTAL: " << stat["output"].m_items
       << endl;
}

void Seq2SeqLemmatizerImpl::evaluate(const vector<TorchMatrix<int64_t>>& input,
                                     const vector<TorchMatrix<int64_t>>& gold,
                                     nets::epoch_stat_t& stat,
                                     const torch::Device& device)
{
  for (size_t i = 0; i < input.size(); ++i)
  {
    const TorchMatrix<int64_t>& input_bucket = input[i];
    const TorchMatrix<int64_t>& gold_bucket = gold[i];
    assert(input_bucket.get_max_feat() == gold_bucket.get_max_feat());

    BiRnnSeq2SeqImpl::evaluate({ "output" }, input_bucket, gold_bucket, stat, device);
  }
}

void Seq2SeqLemmatizerImpl::load(torch::serialize::InputArchive& archive)
{
  BiRnnSeq2SeqImpl::load(archive);
  c10::IValue val;
  archive.read("morph_model", val);
  string serialized_morph_model = *(val.toString().get());
  m_morph_model = morph_model::morph_model_t(serialized_morph_model);
}

void Seq2SeqLemmatizerImpl::save(torch::serialize::OutputArchive& archive) const
{
  BiRnnSeq2SeqImpl::save(archive);
  string serialized_morph_model = m_morph_model.to_string();
  archive.write("morph_model", serialized_morph_model);
}

} // namespace train
} // namespace lemmatization
} // namespace deeplima
