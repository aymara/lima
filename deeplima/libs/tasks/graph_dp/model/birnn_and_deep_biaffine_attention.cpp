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

#include <chrono>
#include <iostream>

#include "birnn_and_deep_biaffine_attention.h"
#include "static_graph/dict.h"

using namespace std;
using namespace torch;
using torch::indexing::Slice;

using namespace deeplima::nets;

namespace deeplima
{
namespace graph_dp
{
namespace train
{

void BiRnnAndDeepBiaffineAttentionImpl::load(serialize::InputArchive& archive)
{
  BiRnnClassifierImpl::load(archive);
}

void BiRnnAndDeepBiaffineAttentionImpl::save(serialize::OutputArchive& archive) const
{
  BiRnnClassifierImpl::save(archive);
}

void BiRnnAndDeepBiaffineAttentionImpl::train(const train_params_graph_dp_t& params,
                                      const std::vector<std::string>& output_names,
                                      const IterableDataSet& train_batches,
                                      const IterableDataSet& eval_batches,
                                      torch::optim::Optimizer& opt,
                                      const torch::Device& device)
{
  shared_ptr<BatchIterator> train_iterator = train_batches.get_iterator();
  train_iterator->set_batch_size(params.m_batch_size);

  while (true)
  {
    for (auto &group : opt.param_groups())
    {
        if (group.has_options())
        {
            auto &options = static_cast<torch::optim::AdamOptions &>(group.options());
            cout << "LR == " << options.get_lr() << endl;
        }
    }
    train_iterator->start_epoch();

    nets::epoch_stat_t train_stat, eval_stat;
    Module::train(true);

    chrono::steady_clock::time_point begin = chrono::steady_clock::now();

    while (!train_iterator->end())
    {
      const BatchIterator::Batch batch = train_iterator->next_batch();
      if (batch.empty())
      {
        continue;
      }
      train_batch(params.m_batch_size,
                  0,
                  output_names,
                  batch.trainable_input,
                  batch.non_trainable_input,
                  batch.gold,
                  opt,
                  train_stat,
                  device);
    }

    train_stat["arc"].m_accuracy = double(train_stat["arc"].m_correct) / train_stat["arc"].m_items;

    chrono::steady_clock::time_point train_end = chrono::steady_clock::now();

    //evaluate(output_names, eval_trainable_input, eval_nontrainable_input, eval_gold, eval_stat, device);

    chrono::steady_clock::time_point eval_end = chrono::steady_clock::now();

    auto train_duration = std::chrono::duration_cast<std::chrono::milliseconds>(train_end - begin).count();
    auto eval_duration = std::chrono::duration_cast<std::chrono::milliseconds>(eval_end - train_end).count();

    char buff[128];
    for (const string& task_name : output_names)
    {
      const task_stat_t& train = train_stat[task_name];
      const task_stat_t& eval = eval_stat[task_name];
      if (2 == eval.m_num_classes)
      {
        snprintf(buff, 128, "%16s | TRAIN LOSS=%4.4f ACC=%.4f | EVAL LOSS=%4.4f ACC=%.4f PR=%.4f RC=%.4f F1=%.4f",
                 task_name.c_str(),
                 train.m_loss, train.m_accuracy, eval.m_loss, eval.m_accuracy,
                 eval.m_precision, eval.m_recall,
                 eval.m_f1);
      }
      else
      {
        snprintf(buff, 128, "%16s | TRAIN LOSS=%4.4f ACC=%.4f | EVAL LOSS=%4.4f ACC=%.4f",
                 task_name.c_str(),
                 train.m_loss, train.m_accuracy, eval.m_loss, eval.m_accuracy);
      }
      cout << buff << endl;
    }
    cout << "TIME: train=" << train_duration << "[ms] eval=" << eval_duration << "[ms]" << endl;
  }

}

void BiRnnAndDeepBiaffineAttentionImpl::train_epoch(size_t batch_size,
                                            size_t seq_len,
                                            const vector<string>& output_names,
                                            const torch::Tensor& trainable_input_batches,
                                            const torch::Tensor& nontrainable_input_batches,
                                            const torch::Tensor& gold_batches,
                                            torch::optim::Optimizer& opt,
                                            epoch_stat_t& stat,
                                            const torch::Device& device)
{
}

void BiRnnAndDeepBiaffineAttentionImpl::train_batch(size_t batch_size,
                                            size_t seq_len,
                                            const vector<string>& output_names,
                                            const torch::Tensor& trainable_input,
                                            const torch::Tensor& nontrainable_input,
                                            const torch::Tensor& gold,
                                            torch::optim::Optimizer& opt,
                                            epoch_stat_t& stat,
                                            const torch::Device& device)
{
  map<string, torch::Tensor> current_batch_inputs;
  split_input(trainable_input, current_batch_inputs, device);
  current_batch_inputs["raw"] = nontrainable_input.to(device);

  //cerr << "gold.sizes() == " << gold.sizes() << endl;
  //cerr << gold << endl;
  auto target = gold.reshape({-1, gold.size(2)}).to(device);

  BiRnnClassifierImpl::train_batch(batch_size, seq_len, output_names, current_batch_inputs, target, opt, stat, device);
}

void BiRnnAndDeepBiaffineAttentionImpl::evaluate(const vector<string>& output_names,
                                         const TorchMatrix<int64_t>& trainable_input,
                                         const TorchMatrix<float>& nontrainable_input,
                                         const TorchMatrix<int64_t>& gold,
                                         epoch_stat_t& stat,
                                         const torch::Device& device)
{
}

void BiRnnAndDeepBiaffineAttentionImpl::predict(size_t /*worker_id*/,
             const torch::Tensor& inputs,
             int64_t input_begin,
             int64_t input_end,
             int64_t output_begin,
             int64_t output_end,
             std::vector<std::vector<uint8_t>>& output,
             const std::vector<std::string>& outputs_names,
             const torch::Device& device)
{
}

} // namespace train
} // namespace graph_dp
} // namespace deeplima
