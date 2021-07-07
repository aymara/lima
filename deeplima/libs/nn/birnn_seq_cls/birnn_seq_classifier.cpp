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

torch::Tensor BiRnnClassifierImpl::predict(const std::string& output_name,
                                           const torch::Tensor& input,
                                           const torch::Device& device)
{
  map<string, torch::Tensor> current_inputs;
  split_input(input, current_inputs);

  auto output_map = forward(current_inputs, { output_name });
  torch::Tensor& output = output_map[output_name];
  torch::Tensor o = output.reshape({-1, output.size(2)});
  torch::Tensor prediction = o.argmax(1);
  return prediction;
}

void BiRnnClassifierImpl::predict(size_t worker_id,
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
  split_input(inputs_slice, current_inputs);

  auto output_map = forward(current_inputs, set<string>(outputs_names.begin(), outputs_names.end()));
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

void BiRnnClassifierImpl::split_input(const torch::Tensor& src, map<string, torch::Tensor>& dst)
{
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
            = src.index({Slice(), Slice(i, i+1) }).reshape({ (int64_t)src.size(0), 1 });
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
            = src.index({Slice(), Slice(), Slice(i, i+1) }).reshape({ (int64_t)src.size(0), -1 });
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

tuple<double, double, int64_t> BiRnnClassifierImpl::evaluate(const std::string& output_name,
                                                                  const TorchMatrix<int64_t>& input,
                                                                  const TorchMatrix<int64_t>& gold,
                                                                  const torch::Device& device)
{
  map<string, torch::Tensor> current_inputs;
  split_input(input.get_tensor(), current_inputs);

  auto target = gold.get_tensor().reshape({-1}).to(device);

  eval();
  auto output_map = forward(current_inputs, { output_name });
  torch::Tensor& output = output_map[output_name];

  torch::Tensor o = output.reshape({-1, output.size(2)});
  torch::Tensor loss_tensor = torch::nn::functional::nll_loss(o, target);
  double loss_value = loss_tensor.item<double>() ;/// input.get_tensor().size(0);
  auto prediction = o.argmax(1);
  int64_t correct_predictions = prediction.eq(target).sum().item<int64_t>();

  double accuracy = double(correct_predictions) / input.get_tensor().size(0);

  return make_tuple(accuracy, loss_value, correct_predictions);
}

void BiRnnClassifierImpl::train(size_t epochs,
                                size_t batch_size,
                                size_t seq_len,
                                const std::string& output_name,
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
    double train_accuracy = 0, train_loss = 0;
    int64_t train_correct = 0;
    Module::train(true);
    tie(train_accuracy, train_loss, train_correct) = train_epoch(batch_size,
                                                                 seq_len,
                                                                 output_name,
                                                                 input_batches,
                                                                 gold_batches,
                                                                 opt,
                                                                 device);

    double eval_accuracy = 0, eval_loss = 0;
    int64_t eval_correct = 0;
    tie(eval_accuracy, eval_loss, eval_correct) = evaluate(output_name, eval_input, eval_gold, device);

    cout << "EPOCH " << e
         << " | TRAIN LOSS=" << train_loss << " ACC=" << train_accuracy
         << " | EVAL LOSS=" << eval_loss << " ACC=" << eval_accuracy
         << " | LR=" << lr_copy
         << endl;

    best_eval_loss = min(best_eval_loss, eval_loss);
    if (eval_accuracy > best_eval_accuracy)
    {
      best_eval_accuracy = eval_accuracy;
      if (model_name.size() > 0)
      {
        torch::save(*this, model_name + ".pt");
      }
      count_below_best = 0;

      if (1 == eval_accuracy)
      {
        return;
      }
    }
    else if (eval_accuracy < best_eval_accuracy)
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
      if (eval_loss > best_eval_loss && count_below_best > 3)
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

tuple<double, double, int64_t> BiRnnClassifierImpl::train_epoch(size_t batch_size,
                                      size_t seq_len,
                                      const string& output_name,
                                      const torch::Tensor& input_batches,
                                      const torch::Tensor& gold_batches,
                                      torch::optim::Optimizer& opt,
                                      const torch::Device& device)
{
  double running_loss = 0.0;
  int64_t num_correct = 0;

  for (size_t b = 0; b < input_batches.size(1); b += batch_size)
  {
    int64_t current_batch_size
        = (b + batch_size > input_batches.size(1)) ? input_batches.size(1) - b : batch_size;

    double batch_loss = 0;
    double batch_correct = 0;

    tie(batch_loss, batch_correct) = train_batch(current_batch_size, seq_len, output_name,
                input_batches.index({Slice(), Slice(b, b + current_batch_size), Slice()}),
                gold_batches.index({Slice(), Slice(b, b + current_batch_size) }),
                opt, device);
    running_loss += batch_loss / current_batch_size;
    num_correct += batch_correct;
  }

  running_loss = running_loss / (input_batches.size(1) / batch_size + 1);
  double accuracy = double(num_correct) / (input_batches.size(0) * input_batches.size(1));
  return make_tuple(accuracy, running_loss, num_correct);
}

std::tuple<double, int64_t> BiRnnClassifierImpl::train_batch(size_t batch_size,
                                                             size_t seq_len,
                                                             const string& output_name,
                                                             const torch::Tensor& input,
                                                             const torch::Tensor& gold,
                                                             torch::optim::Optimizer& opt,
                                                             const torch::Device& device)
{
  map<string, torch::Tensor> current_batch_inputs;
  split_input(input, current_batch_inputs);

  auto target = gold.reshape({-1}).to(device);

  auto output_map = forward(current_batch_inputs, { output_name });
  auto output = output_map[output_name];

  //std::cerr << "output.sizes() == " << output.sizes() << std::endl;
  auto o = output.reshape({-1, output.size(2)});
  torch::Tensor loss_tensor = torch::nn::functional::nll_loss(o, target);
  double loss_value = loss_tensor.item<double>();
  //std::cerr << "o.sizes() == " << o.sizes() << std::endl;
  auto prediction = o.argmax(1);
  int64_t correct_predictions = prediction.eq(target).sum().item<int64_t>();

  opt.zero_grad();
  loss_tensor.backward();
  opt.step();

  return std::make_tuple(loss_value, correct_predictions);
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

