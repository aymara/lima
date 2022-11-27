// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include <chrono>
#include <iostream>

#include "birnn_classifier_for_tag.h"
#include "static_graph/dict.h"
#include "deeplima/utils/std_matrix.h"

using namespace std;
using namespace torch;
using torch::indexing::Slice;

using namespace deeplima::nets;

namespace deeplima
{
namespace tagging
{
namespace train
{

void BiRnnClassifierForNerImpl::load(serialize::InputArchive& archive)
{
  BiRnnClassifierImpl::load(archive);

  assert(m_classes.size() == 0);

  c10::IValue v;
  if (archive.try_read("class_names", v))
  {
    if (!v.isList())
    {
      throw std::runtime_error("class names must be a list.");
    }

    const c10::List<c10::IValue>& l = v.toList();
    m_class_names.reserve(l.size());

    for (size_t i = 0; i < l.size(); i++)
    {
      if (!l.get(i).isString())
      {
        throw std::runtime_error("class names must be a list of strings.");
      }

      m_class_names.push_back(l.get(i).toStringRef());
    }
  }
  else
  {
    throw std::runtime_error("Can't load class names.");
  }

  if (archive.try_read("classes", v))
  {
    if (!v.isList())
    {
      throw std::runtime_error("classes must be a list.");
    }

    const c10::List<c10::IValue>& l = v.toList();
    m_classes.reserve(l.size());

    for (size_t i = 0; i < l.size(); i++)
    {
      if (!l.get(i).isList())
      {
        throw std::runtime_error("classes must be a list of lists of strings.");
      }

      shared_ptr<StringDict> d = shared_ptr<StringDict>(new StringDict());
      d->fromIValue(l.get(i));
      m_classes.push_back(d);
    }
  }
  else
  {
    throw std::runtime_error("Can't load classes.");
  }

  if (archive.try_read("embd_fn", v))
  {
    if (!v.isList())
    {
      throw std::runtime_error("embd_fn must be a list.");
    }

    const c10::List<c10::IValue>& l = v.toList();
    const c10::IValue& s = l.get(0);

    if (!s.isString())
    {
      throw std::runtime_error("embd_fm must be a list of strings.");
    }

    m_embd_fn = s.toStringRef();
  }
  else
  {
    throw std::runtime_error("Can't load embd_fn.");
  }
}

void BiRnnClassifierForNerImpl::save(serialize::OutputArchive& archive) const
{
  BiRnnClassifierImpl::save(archive);

  // Save class names
  c10::List<std::string> list_of_class_names;
  list_of_class_names.reserve(m_class_names.size());
  for (size_t i = 0; i < m_class_names.size(); i++)
  {
    list_of_class_names.push_back(m_class_names[i]);
  }
  archive.write("class_names", list_of_class_names);

  // Save classes
  c10::List<c10::List<std::string>> list_of_classes;
  for (size_t i = 0; i < m_classes.size(); i++)
  {
    c10::List<std::string> current_class;
    shared_ptr<StringDict> d = dynamic_pointer_cast<StringDict, DictBase>(m_classes[i]);
    for (size_t j = 0; j < d->size(); j++)
    {
      current_class.push_back(d->get_value(j));
    }
    list_of_classes.push_back(current_class);
  }
  archive.write("classes", list_of_classes);

  // Save embeddings file names
  c10::List<std::string> list_of_embd_fn;
  list_of_embd_fn.reserve(1);
  list_of_embd_fn.push_back(m_embd_fn);
  archive.write("embd_fn", list_of_embd_fn);
}

void BiRnnClassifierForNerImpl::train(const train_params_tagging_t& params,
                                      const std::vector<std::string>& output_names,
                                      const TorchMatrix<int64_t>& train_trainable_input,
                                      const TorchMatrix<float>& train_nontrainable_input,
                                      const TorchMatrix<int64_t>& train_gold,
                                      const TorchMatrix<int64_t>& eval_trainable_input,
                                      const TorchMatrix<float>& eval_nontrainable_input,
                                      const TorchMatrix<int64_t>& eval_gold,
                                      torch::optim::Optimizer& opt,
                                      double& best_eval_accuracy,
                                      const torch::Device& device)
{
  set_tags(params.m_tags);
  /*int64_t num_batches = train_trainable_input.size() / seq_len;
  cerr << train_trainable_input.size() << endl;
  int64_t num_features = (int64_t)train_trainable_input.get_max_feat();
  int64_t seq_len_i64 = (int64_t)seq_len;

  auto aligned_trainable_input = train_trainable_input.get_tensor().index({ Slice(0, num_batches * seq_len_i64), Slice() });
  auto aligned_nontrainable_input = train_nontrainable_input.get_tensor().index({ Slice(0, num_batches * seq_len_i64), Slice() });
  std::cerr << train_gold.get_tensor().sizes() << std::endl;
  auto aligned_gold = train_gold.get_tensor().index({ Slice(0, num_batches * seq_len_i64), Slice() });
  std::cerr << aligned_gold.sizes() << std::endl;
  auto trainable_input_batches
      = aligned_trainable_input.reshape({ num_batches, seq_len_i64, num_features }).transpose(0, 1);
  auto nontrainable_input_batches
      = aligned_nontrainable_input.reshape({ num_batches, seq_len_i64, -1 }).transpose(0, 1);
  auto gold_batches
      = aligned_gold.reshape({ num_batches, seq_len_i64, -1 }).transpose(0, 1);
  std::cerr << gold_batches.sizes() << std::endl;*/

  double best_eval_loss = numeric_limits<double>::max();
  size_t count_below_best = 0;
  double lr_copy = 0;
  for (size_t e = 1; e < params.m_max_epochs; e++)
  {
    nets::epoch_stat_t train_stat, eval_stat;
    Module::train(true);

    //{
      int64_t num_batches = (train_trainable_input.size() - e) / params.m_sequence_length;
      // std::cerr << "train_trainable_input.size() == " << train_trainable_input.size() << endl;
      int64_t num_features = (int64_t)train_trainable_input.get_max_feat();
      int64_t seq_len_i64 = (int64_t)params.m_sequence_length;

      auto aligned_trainable_input = train_trainable_input.get_tensor().index({ Slice(0 + e, num_batches * seq_len_i64 + e), Slice() });
      auto aligned_nontrainable_input = train_nontrainable_input.get_tensor().index({ Slice(0 + e, num_batches * seq_len_i64 + e), Slice() });
      //std::cerr << train_gold.get_tensor().sizes() << std::endl;
      auto aligned_gold = train_gold.get_tensor().index({ Slice(0 + e, num_batches * seq_len_i64 + e), Slice() });
      //std::cerr << aligned_gold.sizes() << std::endl;

      auto trainable_input_batches
          = aligned_trainable_input.reshape({ num_batches, seq_len_i64, num_features }).transpose(0, 1);
      cerr << "aligned_non_trainable_input.sizes() == " << aligned_nontrainable_input.sizes() << endl;
      auto nontrainable_input_batches
          = aligned_nontrainable_input.reshape({ num_batches, seq_len_i64, -1 }).transpose(0, 1);
      auto gold_batches
          = aligned_gold.reshape({ num_batches, seq_len_i64, -1 }).transpose(0, 1);
      std::cerr << gold_batches.sizes() << std::endl;
    //}

    std::cout << "EPOCH " << e << " | LR=" << lr_copy << " " << std::endl << std::flush;

    chrono::steady_clock::time_point begin = chrono::steady_clock::now();

    train_epoch(params.m_batch_size,
                params.m_sequence_length,
                output_names,
                trainable_input_batches,
                nontrainable_input_batches,
                gold_batches,
                opt,
                train_stat,
                device);

    chrono::steady_clock::time_point train_end = chrono::steady_clock::now();

    evaluate(output_names, eval_trainable_input, eval_nontrainable_input, eval_gold, eval_stat, device);

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
      std::cout << buff << std::endl << std::flush;
    }
    std::cout << "TIME: train=" << train_duration << "[ms] eval=" << eval_duration << "[ms]" << std::endl << std::flush;

    task_stat_t& main_task_eval = eval_stat[output_names[0]];

    best_eval_loss = min(best_eval_loss, main_task_eval.m_loss);
    if (main_task_eval.m_accuracy > best_eval_accuracy)
    {
      best_eval_accuracy = main_task_eval.m_accuracy;
      if (params.m_output_model_name.size() > 0)
      {
        torch::save(*this, params.m_output_model_name + ".pt");
      }
      count_below_best = 0;
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
      if (main_task_eval.m_loss > best_eval_loss && count_below_best > params.m_max_epochs_without_improvement)
      {
        return;
      }
    }
  }
}

void BiRnnClassifierForNerImpl::train_epoch(int64_t batch_size,
                                            size_t seq_len,
                                            const vector<string>& output_names,
                                            const torch::Tensor& trainable_input_batches,
                                            const torch::Tensor& nontrainable_input_batches,
                                            const torch::Tensor& gold_batches,
                                            torch::optim::Optimizer& opt,
                                            epoch_stat_t& stat,
                                            const torch::Device& device)
{
  // double running_loss = 0.0;
  // int64_t num_correct = 0;

  for (int64_t b = 0; b < trainable_input_batches.size(1); b += batch_size)
  {
    int64_t current_batch_size
        = (b + batch_size > trainable_input_batches.size(1)) ? trainable_input_batches.size(1) - b : batch_size;

    // double batch_loss = 0;
    // double batch_correct = 0;

    train_batch(current_batch_size, seq_len, output_names,
                trainable_input_batches.index({Slice(), Slice(b, b + current_batch_size), Slice()}),
                nontrainable_input_batches.index({Slice(), Slice(b, b + current_batch_size), Slice()}),
                gold_batches.index({Slice(), Slice(b, b + current_batch_size) }),
                opt, stat, device);
    // running_loss += batch_loss / current_batch_size;
    // num_correct += batch_correct;

    std::cout << ".";
    std::cout.flush();
  }
  std::cout << endl;

  // running_loss = running_loss / (trainable_input_batches.size(1) / batch_size + 1);
  // double accuracy = double(num_correct) / (trainable_input_batches.size(0) * trainable_input_batches.size(1));
}

void BiRnnClassifierForNerImpl::train_batch(size_t batch_size,
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
  //torch::Tensor nontrainable_input_with_noise = nontrainable_input.to(device) + (0.05) * torch::randn(nontrainable_input.sizes());
  current_batch_inputs["raw"] = nontrainable_input.to(device);

  auto target = gold.reshape({-1, gold.size(2)}).to(device);

  BiRnnClassifierImpl::train_batch(batch_size, seq_len, output_names, current_batch_inputs, target, opt, stat, device);
}

void BiRnnClassifierForNerImpl::evaluate(const vector<string>& output_names,
                                         const TorchMatrix<int64_t>& trainable_input,
                                         const TorchMatrix<float>& nontrainable_input,
                                         const TorchMatrix<int64_t>& gold,
                                         epoch_stat_t& stat,
                                         const torch::Device& device)
{
  map<string, torch::Tensor> current_inputs;
  split_input(trainable_input.get_tensor(), current_inputs, device);
  current_inputs["raw"] = nontrainable_input.get_tensor().reshape({ 1, nontrainable_input.get_tensor().size(0), -1 }).transpose(0, 1).to(device);

  auto target = gold.get_tensor().reshape({-1, gold.get_tensor().size(-1)}).to(device);

  BiRnnClassifierImpl::evaluate(output_names, current_inputs, target, stat, device);
}

void BiRnnClassifierForNerImpl::predict(size_t /*worker_id*/,
             const torch::Tensor& inputs,
             //const torch::Tensor& trainable_input,
             //const torch::Tensor& nontrainable_input,
             int64_t input_begin,
             int64_t input_end,
             int64_t output_begin,
             int64_t output_end,
             std::shared_ptr< StdMatrix<uint8_t> >& output,
             const std::vector<std::string>& outputs_names,
             const torch::Device& device)
{
  assert(output->size() == outputs_names.size());

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
      (*output)[i][input_begin + p] = accessor[p];
    }
  }
}

} // namespace train
} // namespace tagging
} // namespace deeplima

