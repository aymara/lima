// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include <iostream>
#include <map>
#include <string>

#include "birnn_seq_classifier.h"

using namespace std;
using torch::indexing::Slice;

namespace deeplima
{
namespace nets
{

torch::Tensor BiRnnClassifierImpl::predict(const string& output_name,
                                           const torch::Tensor& input,
                                           const torch::Device& device)
{
  vector<string> output_names = { output_name };
  return predict(output_names, input, device);
}

torch::Tensor BiRnnClassifierImpl::predict(const vector<string>& output_names,
                                           const torch::Tensor& input,
                                           const torch::Device& device)
{
  map<string, torch::Tensor> current_inputs;
  split_input(input, current_inputs, device);

  auto output_map = forward(current_inputs, output_names.begin(), output_names.end());

  torch::Tensor prediction = torch::zeros({ input.size(0), long(output_names.size()) },
                                          torch::TensorOptions().dtype(torch::kInt64));
  for (size_t i = 0; i < output_names.size(); ++i)
  {
    const string& task_name = output_names[i];
    torch::Tensor& output = output_map[task_name];
    torch::Tensor o = output.reshape({-1, output.size(2)});
    prediction.index({ Slice(), Slice(i, i+1) }) = o.argmax(1);
  }

  return prediction;
}

void BiRnnClassifierImpl::predict(size_t /*worker_id*/,
                                  const torch::Tensor& inputs,
                                  int64_t input_begin,
                                  int64_t input_end,
                                  int64_t output_begin,
                                  int64_t output_end,
                                  std::vector<std::vector<uint8_t>>& output,
                                  const std::vector<std::string>& outputs_names,
                                  const torch::Device& device)
{
  assert(output.size() == outputs_names.size());

  int64_t start_shift = output_begin - input_begin;
  assert(start_shift >= 0);
  int64_t end_shift = input_end - output_end;
  assert(end_shift >= 0);

  const torch::Tensor inputs_slice = inputs.index({ Slice(input_begin, input_end), Slice() });

  map<string, torch::Tensor> current_inputs;
  split_input(inputs_slice, current_inputs, device);

  auto output_map = forward(current_inputs, outputs_names.begin(), outputs_names.end());
  for (size_t i = 0; i < outputs_names.size(); i++)
  {
    torch::Tensor& one_task_output = output_map[outputs_names[i]];
    torch::Tensor o = one_task_output.reshape({-1, one_task_output.size(2)});
    const torch::Tensor output_tensor = o.argmax(1);
    torch::TensorAccessor<int64_t, 1> accessor = output_tensor.accessor<int64_t, 1>();
    for (int64_t p = start_shift; p < output_tensor.size(0) - end_shift; p++)
    {
      output[i][input_begin + p] = accessor[p];
    }
  }
}

void BiRnnClassifierImpl::split_input(const torch::Tensor& src,
                                      map<string, torch::Tensor>& dst,
                                      const torch::Device& device)
{
  int64_t c = 0;
  for (size_t i = 0; i < m_embd_descr.size(); i++)
  {
    if (dst.end() != dst.find(m_embd_descr[i].m_name))
    {
      throw std::runtime_error("Duplicates in embd list.");
    }
    if (src.sizes().size() == 2)
    {
      if (m_embd_descr[i].m_type == 1)
      {
        dst[m_embd_descr[i].m_name]
            = src.index({Slice(), Slice(c, c+1) }).reshape({ (int64_t)src.size(0), 1 }).to(device);
        c++;
      }
      else if (m_embd_descr[i].m_type == 0)
      {
        //dst[m_embd_descr[i].m_name]
        //    = src.index({Slice(), Slice(i, i+m_embd_descr[i].m_dim) }).reshape({ (int64_t)src.size(0), m_embd_descr[i].m_dim });
      }
    }
    else if (src.sizes().size() == 3)
    {
      //std::cout << "src.sizes() == " << src.sizes() << std::endl;
      if (m_embd_descr[i].m_type == 1)
      {
        dst[m_embd_descr[i].m_name]
            = src.index({Slice(), Slice(), Slice(c, c+1) }).reshape({ (int64_t)src.size(0), -1 }).to(device);
        c++;
      }
      else if (m_embd_descr[i].m_type == 0)
      {
        //dst[m_embd_descr[i].m_name]
        //    = src.index({Slice(), Slice(), Slice(i, i+m_embd_descr[i].m_dim) }).reshape({ (int64_t)src.size(0), m_embd_descr[i].m_dim });
      }
    }
    //std::cout << dst[m_embd_descr[i].m_name].sizes() << std::endl;
  }
}

void BiRnnClassifierImpl::evaluate(const vector<string>& output_names,
                                   const TorchMatrix<int64_t>& input,
                                   const TorchMatrix<int64_t>& gold,
                                   epoch_stat_t& stat,
                                   const torch::Device& device)
{
  map<string, torch::Tensor> current_inputs;
  split_input(input.get_tensor(), current_inputs, device);

  auto target = gold.get_tensor().reshape({ -1, long(output_names.size()) }).to(device);

  evaluate(output_names, current_inputs, target, stat, device);
}

void BiRnnClassifierImpl::evaluate(const vector<string>& output_names,
                                   const map<string, torch::Tensor>& input,
                                   const torch::Tensor& target,
                                   epoch_stat_t& stat,
                                   const torch::Device& /*device*/)
{
  eval();
  auto output_map = forward(input, output_names.begin(), output_names.end());

  for (size_t i = 0; i < output_names.size(); ++i)
  {
    const string& task_name = output_names[i];
    task_stat_t& task_stat = stat[task_name];

    torch::Tensor& output = output_map[task_name];
    torch::Tensor o = output.reshape({-1, output.size(2)});
    auto this_task_target = target.index({ Slice(), Slice(i, i+1) }).reshape({ -1 });
    //cerr << o.sizes() << endl;
    //cerr << this_task_target.sizes() << endl;

    torch::Tensor loss_tensor = torch::nn::functional::nll_loss(o, this_task_target);
    task_stat.m_loss = loss_tensor.sum().item<double>();
    auto prediction = o.argmax(1);
    task_stat.m_correct = prediction.eq(this_task_target).sum().item<int64_t>();
    task_stat.m_items = this_task_target.size(0);
    task_stat.m_accuracy = double(task_stat.m_correct) / task_stat.m_items;

    if (o.size(-1) == 2)
    {
      //cerr << this_task_target.sizes() << endl;
      //cerr << prediction.sizes() << endl;
      //cerr << this_task_target << endl;
      //cerr << prediction << endl;
      task_stat.m_num_classes = o.size(-1);
      double tp = 0, fp = 0, fn = 0;
      auto gold_positive = prediction.eq(1);
      auto gold_negative = prediction.eq(0);
      auto pred_positive = this_task_target.eq(1);
      auto pred_negative = this_task_target.eq(0);
      tp = torch::logical_and(gold_positive, pred_positive).count_nonzero().item<int64_t>();
      fp = torch::logical_and(gold_negative, pred_positive).count_nonzero().item<int64_t>();
      fn = torch::logical_and(gold_positive, pred_negative).count_nonzero().item<int64_t>();
      task_stat.m_precision = tp / (prediction.eq(1).sum().item<int64_t>() + 0.00001);
      task_stat.m_recall = tp / (this_task_target.eq(1).sum().item<int64_t>() + 0.00001);
      task_stat.m_f1 = (2 * task_stat.m_precision * task_stat.m_recall) / (task_stat.m_precision + task_stat.m_recall + 0.00001);
    }
  }
}

void BiRnnClassifierImpl::train(size_t epochs,
                                size_t batch_size,
                                size_t seq_len,
                                const vector<string>& output_names,
                                const TorchMatrix<int64_t>& train_input,
                                const TorchMatrix<int64_t>& train_gold,
                                const TorchMatrix<int64_t>& eval_input,
                                const TorchMatrix<int64_t>& eval_gold,
                                torch::optim::Optimizer& opt,
                                const std::string& model_name,
                                const torch::Device& device)
{
  int64_t num_batches = train_input.size() / seq_len;
  int64_t num_features = (int64_t)train_input.get_max_feat();
  int64_t seq_len_i64 = (int64_t)seq_len;

  auto aligned_input = train_input.get_tensor().index({ Slice(0, num_batches * seq_len_i64), Slice() });
  auto aligned_gold = train_gold.get_tensor().index({ Slice(0, num_batches * seq_len_i64), Slice() });
  auto input_batches
      = aligned_input.reshape({ num_batches, seq_len_i64, num_features }).transpose(0, 1);
  auto gold_batches
      = aligned_gold.reshape({ num_batches, seq_len_i64 }).transpose(0, 1);

  double best_eval_accuracy = 0;
  double best_eval_loss = numeric_limits<double>::max();
  size_t count_below_best = 0;
  double lr_copy = 0;
  for (size_t e = 0; e < epochs; e++)
  {
    epoch_stat_t train_stat, eval_stat;

    Module::train(true);
    train_epoch(batch_size, seq_len, output_names, input_batches, gold_batches, opt, train_stat, device);

    evaluate(output_names, eval_input, eval_gold, eval_stat, device);

    cout << "EPOCH " << e << " | LR=" << lr_copy << endl;
    for (const string& task_name : output_names)
    {
      const task_stat_t& train = train_stat[task_name];
      const task_stat_t& eval = eval_stat[task_name];
      cout << task_name << " ";
      cout << " | TRAIN LOSS=" << train.m_loss << " ACC=" << train.m_accuracy
           << " | EVAL LOSS=" << eval.m_loss << " ACC=" << eval.m_accuracy
           << endl;
    }

    task_stat_t& main_task_eval = eval_stat[output_names[0]];

    best_eval_loss = min(best_eval_loss, main_task_eval.m_loss);
    if (main_task_eval.m_accuracy > best_eval_accuracy)
    {
      best_eval_accuracy = main_task_eval.m_accuracy;
      if (model_name.size() > 0)
      {
        torch::save(*this, model_name + ".pt");
      }
      count_below_best = 0;

      if (1 == main_task_eval.m_accuracy)
      {
        return;
      }
    }
    else if (main_task_eval.m_accuracy < best_eval_accuracy)
    {
      for (auto &group : opt.param_groups())
      {
          if (group.has_options())
          {
              auto &options = static_cast<torch::optim::AdamOptions &>(group.options());
              options.lr(options.lr() * (0.9));
              lr_copy = options.lr();
          }
      }
      if (lr_copy < 0.0000001)
      {
        return;
      }
      count_below_best++;
      if (main_task_eval.m_loss > best_eval_loss && count_below_best > 10)
      {
        return;
      }
      if (count_below_best > 50)
      {
        return;
      }
    }
  }
}

void BiRnnClassifierImpl::train_epoch(size_t batch_size,
                                      size_t seq_len,
                                      const vector<string>& output_names,
                                      const torch::Tensor& input_batches,
                                      const torch::Tensor& gold_batches,
                                      torch::optim::Optimizer& opt,
                                      epoch_stat_t& stat,
                                      const torch::Device& device)
{
  for (int64_t b = 0; b < input_batches.size(1); b += batch_size)
  {
    int64_t current_batch_size
        = ((b + batch_size) > input_batches.size(1)) ? input_batches.size(1) - b : batch_size;

    train_batch(current_batch_size, seq_len, output_names,
                input_batches.index({Slice(), Slice(b, b + current_batch_size), Slice()}),
                gold_batches.index({Slice(), Slice(b, b + current_batch_size) }),
                opt, stat, device);
  }
}

void BiRnnClassifierImpl::train_batch(size_t batch_size,
                                      size_t seq_len,
                                      const vector<string>& output_names,
                                      const torch::Tensor& input,
                                      const torch::Tensor& gold,
                                      torch::optim::Optimizer& opt,
                                      epoch_stat_t& stat,
                                      const torch::Device& device)
{
  map<string, torch::Tensor> current_batch_inputs;
  split_input(input, current_batch_inputs, device);

  auto target = gold.reshape({-1, 1}).to(device);

  train_batch(batch_size, seq_len, output_names, current_batch_inputs, target, opt, stat, device);
}

void BiRnnClassifierImpl::train_batch(size_t /*batch_size*/,
                                      size_t /*seq_len*/,
                                      const vector<string>& output_names,
                                      const map<string, torch::Tensor>& input,
                                      const torch::Tensor& target,
                                      torch::optim::Optimizer& opt,
                                      epoch_stat_t& stat,
                                      const torch::Device& /*device*/)
{
  opt.zero_grad();

  auto output_map = forward(input, output_names.begin(), output_names.end());

  //torch::Tensor loss_tensor = torch::zeros({ batch_size * seq_len },
  //                                         torch::TensorOptions().dtype(torch::kFloat64));
  //cerr << loss_tensor.sizes() << endl;

  for (size_t i = 0; i < output_names.size(); ++i)
  {
    const string& task_name = output_names[i];
    task_stat_t& task_stat = stat[task_name];

    auto output = output_map[task_name];

    auto o = output.reshape({ -1, output.size(2) });
    auto this_task_target = target.index({ Slice(), Slice(i, i+1) }).reshape({ -1 });
    //cerr << o.sizes() << endl;
    //cerr << this_task_target.sizes() << endl;
    torch::Tensor loss_tensor = torch::nn::functional::nll_loss(o, this_task_target);
    double loss_value = loss_tensor.sum().item<double>();
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

void BiRnnClassifierImpl::load(torch::serialize::InputArchive& archive)
{
  StaticGraphImpl::load(archive);

  assert(m_embd_descr.size() == 0);

  c10::IValue v;
  if (archive.try_read("embd_descr", v))
  {
    if (!v.isList())
    {
      throw std::runtime_error("embd_descr must be a list.");
    }

    const c10::List<c10::IValue>& l = v.toList();
    m_embd_descr.reserve(l.size());
    for (size_t i = 0; i < l.size(); i++)
    {
      if (!l.get(i).isString())
      {
        throw std::runtime_error("embd_descr must be a list of strings.");
      }
      const string& str = l.get(i).toStringRef();
      m_embd_descr.emplace_back(embd_descr_t(str));
    }
  }
  else
  {
    throw std::runtime_error("Can't load embd_descr.");
  }
}

void BiRnnClassifierImpl::save(torch::serialize::OutputArchive& archive) const
{
  StaticGraphImpl::save(archive);

  // Save embd descriptions
  c10::List<std::string> embd_descr_list;
  for (size_t i = 0; i < m_embd_descr.size(); i++)
  {
    string embd_str = m_embd_descr[i].to_string();
    assert(embd_descr_t(embd_str) == m_embd_descr[i]);
    embd_descr_list.push_back(embd_str);
  }

  archive.write("embd_descr", embd_descr_list);
}

} // namespace nets
} // namespace deeplima

