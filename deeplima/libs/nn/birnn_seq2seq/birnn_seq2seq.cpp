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

#include "birnn_seq2seq.h"

using namespace std;
using namespace torch;
using torch::indexing::Slice;

namespace deeplima
{
namespace nets
{

void BiRnnSeq2SeqImpl::train_batch(const std::vector<std::string>& output_names,
                                   const torch::Tensor& input,
                                   const vector<torch::Tensor>& input_cat,
                                   const torch::Tensor& target,
                                   torch::optim::Optimizer& opt,
                                   epoch_stat_t& stat,
                                   const torch::Device& device)
{
  opt.zero_grad();
  int64_t l = target.sizes()[0];
  torch::Tensor gold_input = torch::empty({1, target.sizes()[1]},
                                          torch::TensorOptions().dtype(torch::kInt64)).fill_(2);
  gold_input = torch::cat({gold_input, target.index({Slice(0, l-1), Slice()})}, 0);
  //cerr << input.sizes() << " " << gold_input.sizes() << endl;
  //cerr << gold_input << endl;
  //cerr << target << endl;
  map<string, torch::Tensor> input_map
      = { { "enc_chars", input }, { "dec_chars", gold_input } };

  assert(input_cat.size() == m_cat_embd_descr.size());
  for (size_t feat_idx = 0; feat_idx < m_cat_embd_descr.size(); ++feat_idx)
  {
    //std::cerr << input_cat[feat_idx].sizes() << std::endl;
    input_map[m_cat_embd_descr[feat_idx].m_name] = torch::squeeze(input_cat[feat_idx], 0);
  }
  auto output_map = forward(input_map, output_names.begin(), output_names.end());

  for (size_t i = 0; i < output_names.size(); ++i)
  {
    const string& task_name = output_names[i];
    task_stat_t& task_stat = stat[task_name];

    auto output = output_map[task_name];

    //cerr << "output.sizes() == " << output.sizes() << std::endl;
    ////cerr << target << endl;
    ////cerr << std::get<1>(output.topk(1, 2)).reshape(target.sizes()) << endl;
    auto o = output.reshape({ -1, output.size(2) });
    //cerr << target.sizes() << endl;
    auto this_task_target = target.reshape({ -1 }); //.index({ Slice(), Slice(i, i+1) });
    //cerr << o.sizes() << endl;
    //cerr << this_task_target.sizes() << endl;
    torch::Tensor loss_tensor = torch::nn::functional::nll_loss(o, this_task_target);
    //cerr << loss_tensor.sizes() << endl;
    double loss_value = loss_tensor.mean().item<double>();
    task_stat.m_loss += loss_value;
    //std::cerr << "o.sizes() == " << o.sizes() << std::endl;
    auto prediction = o.argmax(1);
    int64_t correct_predictions = prediction.eq(this_task_target).sum().item<int64_t>();
    task_stat.m_correct += correct_predictions;
    task_stat.m_items += this_task_target.size(0);

    loss_tensor.backward({}, true);
  }
  opt.step();
}

void BiRnnSeq2SeqImpl::evaluate(const vector<string>& output_names,
                                const TorchMatrix<int64_t>& input,
                                const TorchMatrix<int64_t>& gold,
                                epoch_stat_t& stat,
                                const torch::Device& device)
{
  eval();
}

void BiRnnSeq2SeqImpl::load(torch::serialize::InputArchive& archive)
{
  BiRnnClassifierImpl::load(archive);
}

void BiRnnSeq2SeqImpl::save(torch::serialize::OutputArchive& archive) const
{
  BiRnnClassifierImpl::save(archive);
}

} // namespace lemmatization
} // namespace deeplima

