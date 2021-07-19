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

#include "birnn_classifier_for_ner.h"
#include "static_graph/dict.h"

using namespace std;
using namespace torch;
using torch::indexing::Slice;

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

void BiRnnClassifierForNerImpl::train(size_t epochs,
           size_t batch_size,
           size_t seq_len,
           const std::string& output_name,
           const TorchMatrix<int64_t>& train_trainable_input,
           const TorchMatrix<float>& train_nontrainable_input,
           const TorchMatrix<int64_t>& train_gold,
           const TorchMatrix<int64_t>& eval_trainable_input,
           const TorchMatrix<float>& eval_nontrainable_input,
           const TorchMatrix<int64_t>& eval_gold,
           torch::optim::Optimizer& opt,
           const std::string& model_name = "",
           const torch::Device& device)
{
  int64_t num_batches = train_trainable_input.size() / seq_len;
  int64_t num_features = (int64_t)train_trainable_input.get_max_feat();
  int64_t seq_len_i64 = (int64_t)seq_len;

  auto aligned_trainable_input = train_trainable_input.get_tensor().index({ Slice(0, num_batches * seq_len_i64), Slice() });
  auto aligned_nontrainable_input = train_nontrainable_input.get_tensor().index({ Slice(0, num_batches * seq_len_i64), Slice() });
  auto aligned_gold = train_gold.get_tensor().index({ Slice(0, num_batches * seq_len_i64), Slice() });
  auto trainable_input_batches
      = aligned_trainable_input.reshape({ num_batches, seq_len_i64, num_features }).transpose(0, 1);
  auto nontrainable_input_batches
      = aligned_nontrainable_input.reshape({ num_batches, seq_len_i64, -1 }).transpose(0, 1);
  auto gold_batches
      = aligned_gold.reshape({ num_batches, seq_len_i64 }).transpose(0, 1);

  double best_eval_accuracy = 0;
  double best_eval_loss = numeric_limits<double>::max();
  size_t count_below_best = 0;
  for (size_t e = 0; e < epochs; e++)
  {
    double train_accuracy = 0, train_loss = 0;
    int64_t train_correct = 0;
    Module::train(true);
    tie(train_accuracy, train_loss, train_correct) = train_epoch(batch_size,
                                                                 seq_len,
                                                                 output_name,
                                                                 trainable_input_batches,
                                                                 nontrainable_input_batches,
                                                                 gold_batches,
                                                                 opt,
                                                                 device);

    double eval_accuracy = 0, eval_loss = 0;
    int64_t eval_correct = 0;
    tie(eval_accuracy, eval_loss, eval_correct) = evaluate(output_name, eval_trainable_input, eval_nontrainable_input, eval_gold, device);

    cout << "EPOCH " << e
         << " | TRAIN LOSS=" << train_loss << " ACC=" << train_accuracy
         << " | EVAL LOSS=" << eval_loss << " ACC=" << eval_accuracy << endl;

    best_eval_loss = min(best_eval_loss, eval_loss);
    if (eval_accuracy > best_eval_accuracy)
    {
      best_eval_accuracy = eval_accuracy;
      if (model_name.size() > 0)
      {
        torch::save(*this, model_name + ".pt");
      }
      count_below_best = 0;
    }
    else if (eval_accuracy < best_eval_accuracy)
    {
      for (auto &group : opt.param_groups())
      {
          if (group.has_options())
          {
              auto &options = static_cast<torch::optim::AdamOptions &>(group.options());
              options.lr(options.lr() * (0.9));
          }
      }
      count_below_best++;
      if (eval_loss > best_eval_loss && count_below_best > 3)
      {
        return;
      }
    }
  }
}

tuple<double, double, int64_t> BiRnnClassifierForNerImpl::train_epoch(size_t batch_size,
                                      size_t seq_len,
                                      const string& output_name,
                                      const torch::Tensor& trainable_input_batches,
                                      const torch::Tensor& nontrainable_input_batches,
                                      const torch::Tensor& gold_batches,
                                      torch::optim::Optimizer& opt,
                                      const torch::Device& device)
{
  double running_loss = 0.0;
  int64_t num_correct = 0;

  for (size_t b = 0; b < trainable_input_batches.size(1); b += batch_size)
  {
    int64_t current_batch_size
        = (b + batch_size > trainable_input_batches.size(1)) ? trainable_input_batches.size(1) - b : batch_size;

    double batch_loss = 0;
    double batch_correct = 0;

    tie(batch_loss, batch_correct) = train_batch(current_batch_size, seq_len, output_name,
                trainable_input_batches.index({Slice(), Slice(b, b + current_batch_size), Slice()}),
                nontrainable_input_batches.index({Slice(), Slice(b, b + current_batch_size), Slice()}),
                gold_batches.index({Slice(), Slice(b, b + current_batch_size) }),
                opt, device);
    running_loss += batch_loss / current_batch_size;
    num_correct += batch_correct;
  }

  running_loss = running_loss / (trainable_input_batches.size(1) / batch_size + 1);
  double accuracy = double(num_correct) / (trainable_input_batches.size(0) * trainable_input_batches.size(1));
  return make_tuple(accuracy, running_loss, num_correct);
}

tuple<double, int64_t> BiRnnClassifierForNerImpl::train_batch(size_t batch_size,
                                                             size_t seq_len,
                                                             const string& output_name,
                                                             const torch::Tensor& trainable_input,
                                                             const torch::Tensor& nontrainable_input,
                                                             const torch::Tensor& gold,
                                                             torch::optim::Optimizer& opt,
                                                             const torch::Device& device)
{
  map<string, torch::Tensor> current_batch_inputs;
  split_input(trainable_input, current_batch_inputs, device);
  current_batch_inputs["raw"] = nontrainable_input.to(device);

  auto target = gold.reshape({-1}).to(device);

  opt.zero_grad();

  auto output_map = forward(current_batch_inputs, { output_name });
  auto output = output_map[output_name];

  //std::cerr << "output.sizes() == " << output.sizes() << std::endl;
  auto o = output.reshape({-1, output.size(2)});
  torch::Tensor loss_tensor = torch::nn::functional::nll_loss(o, target);
  double loss_value = loss_tensor.item<double>();
  //std::cerr << "o.sizes() == " << o.sizes() << std::endl;
  auto prediction = o.argmax(1);
  int64_t correct_predictions = prediction.eq(target).sum().item<int64_t>();

  loss_tensor.backward();
  opt.step();

  return std::make_tuple(loss_value, correct_predictions);
}

tuple<double, double, int64_t> BiRnnClassifierForNerImpl::evaluate(const std::string& output_name,
                                                                   const TorchMatrix<int64_t>& trainable_input,
                                                                   const TorchMatrix<float>& nontrainable_input,
                                                                   const TorchMatrix<int64_t>& gold,
                                                                   const torch::Device& device)
{
  map<string, torch::Tensor> current_inputs;
  split_input(trainable_input.get_tensor(), current_inputs, device);
  current_inputs["raw"] = nontrainable_input.get_tensor().reshape({ 1, nontrainable_input.get_tensor().size(0), -1 }).transpose(0, 1).to(device);

  auto target = gold.get_tensor().reshape({-1}).to(device);

  eval();
  auto output_map = forward(current_inputs, { output_name });
  torch::Tensor& output = output_map[output_name];

  torch::Tensor o = output.reshape({-1, output.size(2)});
  torch::Tensor loss_tensor = torch::nn::functional::nll_loss(o, target);
  double loss_value = loss_tensor.item<double>() ;/// input.get_tensor().size(0);
  auto prediction = o.argmax(1);
  int64_t correct_predictions = prediction.eq(target).sum().item<int64_t>();

  double accuracy = double(correct_predictions) / trainable_input.get_tensor().size(0);

  return make_tuple(accuracy, loss_value, correct_predictions);
}

void BiRnnClassifierForNerImpl::predict(size_t worker_id,
             const torch::Tensor& inputs,
             //const torch::Tensor& trainable_input,
             //const torch::Tensor& nontrainable_input,
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

} // namespace train
} // namespace tagging
} // namespace deeplima

