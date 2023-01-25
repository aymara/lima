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
  std::shared_ptr<BatchIterator> train_iterator = train_batches.get_iterator();
  train_iterator->set_batch_size(params.m_batch_size);

  double best_eval_loss = numeric_limits<double>::max();
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

    train_stat["arc"].m_accuracy = double(train_stat["arc"].m_correct) / train_stat["arc"].m_items;
    train_stat["arc"].m_loss /= train_stat["arc"].m_items;

    chrono::steady_clock::time_point train_end = chrono::steady_clock::now();

    evaluate(output_names, eval_batches.get_iterator(), eval_stat, device);

    eval_stat["arc"].m_accuracy = double(eval_stat["arc"].m_correct) / eval_stat["arc"].m_items;
    eval_stat["arc"].m_loss /= eval_stat["arc"].m_items;

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
  map<string, torch::Tensor> current_batch_inputs;
  split_input(trainable_input, current_batch_inputs, device);
  current_batch_inputs["raw"] = nontrainable_input.to(device);

  //cerr << "gold.sizes() == " << gold.sizes() << endl;
  //cerr << gold << endl;
  auto target = gold.reshape({-1, gold.size(2)}).to(device);

  BiRnnClassifierImpl::train_batch(batch_size, seq_len, output_names, current_batch_inputs, target, opt, stat, device);
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
    stat["arc"].m_correct += t["arc"].m_correct;
    stat["arc"].m_items += t["arc"].m_items;
    stat["arc"].m_loss += t["arc"].m_loss;
  }
}

void BiRnnAndDeepBiaffineAttentionImpl::evaluate(const vector<string>& output_names,
                                                 const torch::Tensor& trainable_input,
                                                 const torch::Tensor& nontrainable_input,
                                                 const torch::Tensor& gold,
                                                 epoch_stat_t& stat,
                                                 const torch::Device& device)
{
  map<string, torch::Tensor> current_inputs;
  split_input(trainable_input, current_inputs, device);
  current_inputs["raw"] = nontrainable_input.to(device);

  auto target = gold.reshape({-1, gold.size(-1)}).to(device);

  BiRnnClassifierImpl::evaluate(output_names, current_inputs, target, stat, device);
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
