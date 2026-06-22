// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include <algorithm>
#include <chrono>
#include <iostream>
#include <limits>

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

#define SERIALIZATION_KEY_TASKS "tasks"
#define SERIALIZATION_KEY_INPUT_FEATURES "input_features"
#define SERIALIZATION_KEY_INPUT_FEATURES_NAMES "input_features_names"
#define SERIALIZATION_KEY_EMBD_FN "embd_fn"

void BiRnnAndDeepBiaffineAttentionImpl::load(serialize::InputArchive& archive)
{
  BiRnnClassifierImpl::load(archive);

  //assert(m_classes.size() == 0);

  c10::IValue v;
  if (archive.try_read(SERIALIZATION_KEY_TASKS, v))
  {
    if (!v.isList())
    {
      throw std::runtime_error("List of tasks must be a list.");
    }

    const c10::List<c10::IValue>& l = v.toList();
    m_output_class_names.reserve(l.size());

    for (size_t i = 0; i < l.size(); i++)
    {
      if (!l.get(i).isString())
      {
        throw std::runtime_error("List of tasks must be a list of strings.");
      }

      m_output_class_names.push_back(l.get(i).toStringRef());
    }
  }
  else
  {
    throw std::runtime_error("Can't load list of tasks.");
  }

  if (archive.try_read(SERIALIZATION_KEY_INPUT_FEATURES_NAMES, v))
  {
    if (!v.isList())
    {
      throw std::runtime_error("List of input feature names must be a list.");
    }

    const c10::List<c10::IValue>& l = v.toList();
    m_input_class_names.reserve(l.size());

    for (size_t i = 0; i < l.size(); i++)
    {
      if (!l.get(i).isString())
      {
        throw std::runtime_error("List of input features names must be a list of strings.");
      }

      m_input_class_names.push_back(l.get(i).toStringRef());
    }
  }
  else
  {
    throw std::runtime_error("Can't load list of input feature names.");
  }

  if (archive.try_read(SERIALIZATION_KEY_INPUT_FEATURES, v))
  {
    if (!v.isList())
    {
      throw std::runtime_error("List of input features must be a list.");
    }

    const c10::List<c10::IValue>& l = v.toList();
    m_input_classes.reserve(l.size());

    for (size_t i = 0; i < l.size(); i++)
    {
      if (!l.get(i).isList())
      {
        throw std::runtime_error("List of input features must be a list of lists of strings.");
      }

      auto d = std::make_shared<StringDict>();
      d->fromIValue(l.get(i));
      m_input_classes.push_back(d);
    }
  }
  else
  {
    throw std::runtime_error("Can't list of input features.");
  }

  if (archive.try_read(SERIALIZATION_KEY_EMBD_FN, v))
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

void BiRnnAndDeepBiaffineAttentionImpl::save(serialize::OutputArchive& archive) const
{
  BiRnnClassifierImpl::save(archive);

  // Save output class names
  {
  c10::List<std::string> list_of_class_names;
  list_of_class_names.reserve(m_output_class_names.size());
  for (size_t i = 0; i < m_output_class_names.size(); i++)
  {
    list_of_class_names.push_back(m_output_class_names[i]);
  }
  archive.write(SERIALIZATION_KEY_TASKS, list_of_class_names);
  }

  // Save input class names
  {
  c10::List<std::string> list_of_class_names;
  list_of_class_names.reserve(m_input_class_names.size());
  for (size_t i = 0; i < m_input_class_names.size(); i++)
  {
    list_of_class_names.push_back(m_input_class_names[i]);
  }
  archive.write(SERIALIZATION_KEY_INPUT_FEATURES_NAMES, list_of_class_names);
  }

  // Save input classes
  c10::List<c10::List<std::string>> list_of_classes;
  for (size_t i = 0; i < m_input_classes.size(); i++)
  {
    c10::List<std::string> current_class;
    shared_ptr<StringDict> d = dynamic_pointer_cast<StringDict, DictBase>(m_input_classes[i]);
    for (size_t j = 0; j < d->size(); j++)
    {
      current_class.push_back(d->get_value(j));
    }
    list_of_classes.push_back(current_class);
  }
  archive.write(SERIALIZATION_KEY_INPUT_FEATURES, list_of_classes);

  // Save embeddings file names
  c10::List<std::string> list_of_embd_fn;
  list_of_embd_fn.reserve(1);
  list_of_embd_fn.push_back(m_embd_fn);
  archive.write(SERIALIZATION_KEY_EMBD_FN, list_of_embd_fn);
}

void BiRnnAndDeepBiaffineAttentionImpl::train(const train_params_graph_dp_t& params,
                                              const std::vector<std::string>& output_names,
                                              const IterableDataSet& train_batches,
                                              const IterableDataSet& eval_batches,
                                              torch::optim::Optimizer& opt,
                                              double& best_eval_accuracy,
                                              const torch::Device& device)
{
  auto train_iterator = train_batches.get_iterator();
  train_iterator->set_batch_size(params.m_batch_size);

  double best_eval_loss = std::numeric_limits<double>::max();
  size_t count_below_best = 0;
  double lr_copy = 0;
  unsigned int epoch = 0;
  while (true)
  {
    for (auto &group : opt.param_groups())
    {
        if (group.has_options())
        {
            auto &options = static_cast<torch::optim::AdamOptions &>(group.options());
            // cout << "LR == " << options.get_lr() << endl;
            lr_copy = options.get_lr();
        }
    }

    nets::epoch_stat_t train_stat, eval_stat;

    chrono::steady_clock::time_point begin = chrono::steady_clock::now();

    train_epoch(params.m_batch_size,
                0,
                output_names,
                train_iterator,
                opt,
                train_stat,
                device);

    for (const std::string& tn : output_names)
    {
      if (train_stat[tn].m_items > 0)
      {
        train_stat[tn].m_accuracy = double(train_stat[tn].m_correct) / train_stat[tn].m_items;
        train_stat[tn].m_loss /= train_stat[tn].m_items;
      }
    }

    chrono::steady_clock::time_point train_end = chrono::steady_clock::now();

    evaluate(output_names, eval_batches.get_iterator(), eval_stat, device);

    for (const std::string& tn : output_names)
    {
      if (eval_stat[tn].m_items > 0)
      {
        eval_stat[tn].m_accuracy = double(eval_stat[tn].m_correct) / eval_stat[tn].m_items;
        eval_stat[tn].m_loss /= eval_stat[tn].m_items;
      }
    }

    chrono::steady_clock::time_point eval_end = chrono::steady_clock::now();

    auto train_duration = std::chrono::duration_cast<std::chrono::milliseconds>(train_end - begin).count();
    auto eval_duration = std::chrono::duration_cast<std::chrono::milliseconds>(eval_end - train_end).count();

    for (const string& task_name : output_names)
    {
      const auto& train = train_stat[task_name];
      const auto& eval = eval_stat[task_name];
      std::cout << "EPOCH " << epoch << " | " << task_name << " | LR=" << lr_copy
                << " | TRAIN LOSS=" << train.m_loss << " ACC=" << train.m_accuracy
                << " | EVAL LOSS=" << eval.m_loss << " ACC=" << eval.m_accuracy;
      if (2 == eval.m_num_classes)
      {
        std::cout << " P=" << eval.m_precision << " R=" << eval.m_recall<< " F1=" << eval.m_f1;
      }
      else
      {
        std::cout << " CORRECT=" << eval.m_correct;
      }
      std::cout << std::endl << std::flush;
    }
    cout << "TIME: train=" << train_duration << "[ms] eval=" << eval_duration << "[ms]" << endl;

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
    epoch++;
  }
}

void BiRnnAndDeepBiaffineAttentionImpl::train_epoch(size_t /*batch_size*/,
                                                    size_t /*seq_len*/,
                                                    const vector<string>& output_names,
                                                    shared_ptr<BatchIterator> dataset_iterator,
                                                    torch::optim::Optimizer& opt,
                                                    epoch_stat_t& stat,
                                                    const torch::Device& device)
{
  Module::train(true);

  dataset_iterator->start_epoch();

  while (!dataset_iterator->end())
  {
    const BatchIterator::Batch batch = dataset_iterator->next_batch();
    if (batch.empty())
    {
      continue;
    }
    train_batch(batch.get_batch_size(),
                0,
                output_names,
                batch.trainable_input(),
                batch.frozen_input(),
                batch.gold(),
                opt,
                stat,
                device);
  }
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
  using torch::indexing::Slice;
  static constexpr int64_t kIgnore = -100;

  map<string, torch::Tensor> current_batch_inputs;
  split_input(trainable_input, current_batch_inputs, device);
  current_batch_inputs["raw"] = nontrainable_input.to(device);

  const int64_t B = gold.size(0);
  const int64_t D = gold.size(1);
  auto target = gold.reshape({-1, gold.size(2)}).to(device); // [B*D, ncols]

  const bool train_rel = (m_num_labels > 0)
      && (std::find(output_names.begin(), output_names.end(), std::string("rel")) != output_names.end());

  std::vector<std::string> requested = { "arc" };
  if (train_rel) { requested.push_back("rel_logits"); }

  opt.zero_grad();
  auto out = forward(current_batch_inputs, requested.begin(), requested.end());

  // ---- arc (head prediction) ----
  {
    torch::Tensor o = out["arc"].reshape({ -1, out["arc"].size(2) }); // [N, heads]
    torch::Tensor tgt = target.index({ Slice(), Slice(0, 1) }).reshape({ -1 });
    torch::Tensor loss = torch::nn::functional::nll_loss(
        o, tgt, torch::nn::functional::NLLLossFuncOptions().ignore_index(kIgnore));
    torch::Tensor valid = tgt.ne(kIgnore);
    torch::Tensor pred = o.argmax(1);
    task_stat_t& s = stat["arc"];
    s.m_loss += loss.sum().item<double>();
    s.m_correct += pred.eq(tgt).logical_and(valid).sum().item<int64_t>();
    s.m_items += valid.sum().item<int64_t>();
    loss.backward({}, /*retain_graph=*/train_rel);
  }

  // ---- rel (deprel label), scored at the gold head ----
  if (train_rel)
  {
    torch::Tensor rel_logits = out["rel_logits"]; // [B, dep, head, num_labels]
    const int64_t L = rel_logits.size(3);
    torch::Tensor gold_head = target.index({ Slice(), 0 }).reshape({ B, D });
    torch::Tensor gh = gold_head.clamp_min(0); // ignored positions -> 0 (masked by loss)
    torch::Tensor idx = gh.unsqueeze(-1).unsqueeze(-1).expand({ B, D, 1, L });
    torch::Tensor gathered = rel_logits.gather(2, idx).squeeze(2); // [B, dep, num_labels]
    torch::Tensor rel_log = torch::log_softmax(gathered, 2).reshape({ -1, L });
    torch::Tensor tgt = target.index({ Slice(), Slice(1, 2) }).reshape({ -1 });
    torch::Tensor loss = torch::nn::functional::nll_loss(
        rel_log, tgt, torch::nn::functional::NLLLossFuncOptions().ignore_index(kIgnore));
    torch::Tensor valid = tgt.ne(kIgnore);
    torch::Tensor pred = rel_log.argmax(1);
    task_stat_t& s = stat["rel"];
    s.m_loss += loss.sum().item<double>();
    s.m_correct += pred.eq(tgt).logical_and(valid).sum().item<int64_t>();
    s.m_items += valid.sum().item<int64_t>();
    loss.backward();
  }

  opt.step();
}

void BiRnnAndDeepBiaffineAttentionImpl::evaluate(const vector<string>& output_names,
                                                 shared_ptr<BatchIterator> dataset_iterator,
                                                 epoch_stat_t& stat,
                                                 const torch::Device& device)
{
  eval();
  dataset_iterator->set_batch_size(-1);

  dataset_iterator->start_epoch();

  while (!dataset_iterator->end())
  {
    const BatchIterator::Batch batch = dataset_iterator->next_batch();
    if (batch.empty())
    {
      continue;
    }

    epoch_stat_t t;

    evaluate(output_names,
             batch.trainable_input(),
             batch.frozen_input(),
             batch.gold(),
             t,
             device);
    for (const std::string& task_name : output_names)
    {
      stat[task_name].m_correct += t[task_name].m_correct;
      stat[task_name].m_items += t[task_name].m_items;
      stat[task_name].m_loss += t[task_name].m_loss;
    }
  }
}

void BiRnnAndDeepBiaffineAttentionImpl::evaluate(const vector<string>& output_names,
                                                 const torch::Tensor& trainable_input,
                                                 const torch::Tensor& nontrainable_input,
                                                 const torch::Tensor& gold,
                                                 epoch_stat_t& stat,
                                                 const torch::Device& device)
{
  using torch::indexing::Slice;
  static constexpr int64_t kIgnore = -100;

  map<string, torch::Tensor> current_inputs;
  split_input(trainable_input, current_inputs, device);
  current_inputs["raw"] = nontrainable_input.to(device);

  const int64_t B = gold.size(0);
  const int64_t D = gold.size(1);
  auto target = gold.reshape({-1, gold.size(-1)}).to(device);

  const bool eval_rel = (m_num_labels > 0)
      && (std::find(output_names.begin(), output_names.end(), std::string("rel")) != output_names.end());

  std::vector<std::string> requested = { "arc" };
  if (eval_rel) { requested.push_back("rel_logits"); }

  torch::NoGradGuard no_grad;
  auto out = forward(current_inputs, requested.begin(), requested.end());

  // ---- arc (UAS) ----
  {
    torch::Tensor o = out["arc"].reshape({ -1, out["arc"].size(2) });
    torch::Tensor tgt = target.index({ Slice(), Slice(0, 1) }).reshape({ -1 });
    torch::Tensor loss = torch::nn::functional::nll_loss(
        o, tgt, torch::nn::functional::NLLLossFuncOptions().ignore_index(kIgnore));
    torch::Tensor valid = tgt.ne(kIgnore);
    torch::Tensor pred = o.argmax(1);
    task_stat_t& s = stat["arc"];
    s.m_loss = loss.sum().item<double>();
    s.m_correct = pred.eq(tgt).logical_and(valid).sum().item<int64_t>();
    s.m_items = valid.sum().item<int64_t>();
  }

  // ---- rel (label accuracy at the gold head) ----
  if (eval_rel)
  {
    torch::Tensor rel_logits = out["rel_logits"]; // [B, dep, head, num_labels]
    const int64_t L = rel_logits.size(3);
    torch::Tensor gold_head = target.index({ Slice(), 0 }).reshape({ B, D });
    torch::Tensor gh = gold_head.clamp_min(0);
    torch::Tensor idx = gh.unsqueeze(-1).unsqueeze(-1).expand({ B, D, 1, L });
    torch::Tensor gathered = rel_logits.gather(2, idx).squeeze(2);
    torch::Tensor rel_log = torch::log_softmax(gathered, 2).reshape({ -1, L });
    torch::Tensor tgt = target.index({ Slice(), Slice(1, 2) }).reshape({ -1 });
    torch::Tensor loss = torch::nn::functional::nll_loss(
        rel_log, tgt, torch::nn::functional::NLLLossFuncOptions().ignore_index(kIgnore));
    torch::Tensor valid = tgt.ne(kIgnore);
    torch::Tensor pred = rel_log.argmax(1);
    task_stat_t& s = stat["rel"];
    s.m_loss = loss.sum().item<double>();
    s.m_correct = pred.eq(tgt).logical_and(valid).sum().item<int64_t>();
    s.m_items = valid.sum().item<int64_t>();
  }
}

void BiRnnAndDeepBiaffineAttentionImpl::predict(size_t /*worker_id*/,
             const torch::Tensor& /*inputs*/,
             int64_t /*input_begin*/,
             int64_t /*input_end*/,
             int64_t /*output_begin*/,
             int64_t /*output_end*/,
             std::shared_ptr< StdMatrix<uint8_t> >& /*output*/,
             const std::vector<std::string>& /*outputs_names*/,
             const torch::Device& /*device*/)
{
//   TODO should it be implemented?
}

} // namespace train
} // namespace graph_dp
} // namespace deeplima
