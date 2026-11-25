// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_LIBS_NN_BIRNN_SEQ_CLASSIFIER_H
#define DEEPLIMA_LIBS_NN_BIRNN_SEQ_CLASSIFIER_H

#include <tuple>
#include <vector>

#include "static_graph/static_graph.h"
#include "deeplima/torch_wrp/torch_matrix.h"
#include "nn/common/layer_descr.h"
#include "deeplima/utils/std_matrix.h"

namespace deeplima
{
namespace nets
{

struct task_stat_t
{
  uint64_t m_items;
  uint64_t m_correct;
  uint32_t m_num_classes;
  double m_accuracy;
  double m_loss;
  double m_precision;
  double m_recall;
  double m_f1;
  // F1 of the "end of sentence" decisions (BiRnnClassifierImpl::set_eos_classes),
  // -1 when not computed
  double m_eos_f1;

  task_stat_t()
    : m_items(0), m_correct(0), m_num_classes(0), m_accuracy(0), m_loss(0), m_precision(0), m_recall(0), m_f1(0),
      m_eos_f1(-1) { }
};

typedef std::map<std::string, task_stat_t> epoch_stat_t;

class BiRnnClassifierImpl : public StaticGraphImpl
{
public:

  BiRnnClassifierImpl() {}

  BiRnnClassifierImpl(DictsHolder&& dicts,
                      const std::vector<embd_descr_t>& embd_descr,
                      const std::vector<rnn_descr_t>& rnn_descr,
                      const std::vector<std::string>& output_names,
                      const std::vector<uint32_t>& classes,
                      float input_dropout_prob)
    : StaticGraphImpl(dicts,
                      generate_script(embd_descr, rnn_descr, output_names, classes, input_dropout_prob)),
      m_embd_descr(embd_descr)
  {
  }

  BiRnnClassifierImpl(DictsHolder&& dicts,
                      const std::vector<embd_descr_t>& embd_descr,
                      const std::string& script)
    : StaticGraphImpl(dicts, script),
      m_embd_descr(embd_descr)
  {
  }

  const std::vector<embd_descr_t>& get_embd_descr() const
  {
    return m_embd_descr;
  }

  virtual void load(torch::serialize::InputArchive& archive);
  virtual void save(torch::serialize::OutputArchive& archive) const;

  void train(size_t epochs,
             size_t batch_size,
             size_t seq_len,
             const std::vector<std::string>& output_names,
             const TorchMatrix<int64_t>& train_input,
             const TorchMatrix<int64_t>& train_gold,
             const TorchMatrix<int64_t>& eval_input,
             const TorchMatrix<int64_t>& eval_gold,
             torch::optim::Optimizer& opt,
             const std::string& model_name = "",
             const torch::Device& device = torch::Device(torch::kCPU));

  void evaluate(const std::vector<std::string>& output_name,
                const TorchMatrix<int64_t>& input,
                const TorchMatrix<int64_t>& gold,
                epoch_stat_t& stat,
                const torch::Device& device = torch::Device(torch::kCPU));

  torch::Tensor predict(const std::string& output_name,
                        const torch::Tensor& input,
                        const torch::Device& device = torch::Device(torch::kCPU));

  torch::Tensor predict(const std::vector<std::string>& output_names,
                        const torch::Tensor& input,
                        const torch::Device& device = torch::Device(torch::kCPU));

  void predict(size_t worker_id,
               const torch::Tensor& inputs,
               int64_t input_begin,
               int64_t input_end,
               int64_t output_begin,
               int64_t output_end,
               std::shared_ptr< StdMatrix<uint8_t> >& output,
               const std::vector<std::string>& outputs_names,
               const torch::Device& device = torch::Device(torch::kCPU));

  // Training options of train() (the segmenter's loop); the defaults keep the
  // former behaviour.
  // Loss weight of each class of the main task (empty: none)
  void set_class_weights(const std::vector<float>& w) { m_class_weights = w; }
  // Classes that end a sentence: the evaluation also computes their F1
  // (m_eos_f1), and, with select_on_eos, the epoch kept is the one with the best
  // mean of accuracy and that F1 instead of the best accuracy
  void set_eos_classes(const std::vector<int64_t>& c, bool select_on_eos)
  {
    m_eos_classes = c;
    m_select_on_eos = select_on_eos;
  }
  // Learning rate factor after an epoch without improvement, and the number of
  // such epochs (with a worse dev loss) before stopping
  void set_schedule(double lr_decay, size_t patience)
  {
    m_lr_decay = lr_decay;
    m_patience = patience;
  }

protected:

  void split_input(const torch::Tensor& src,
                   std::map<std::string, torch::Tensor>& dst,
                   const torch::Device& device);

  void evaluate(const std::vector<std::string>& output_names,
                const std::map<std::string, torch::Tensor>& input,
                const torch::Tensor& target,
                epoch_stat_t& stat,
                const torch::Device& device);

  // The evaluation set used to go through the network as one sequence of all
  // its timepoints (segmentation: every character of the dev text), with
  // memory in proportion to its length x the hidden size: 26.69 GiB asked on
  // UD_German-MERGE at hiddenDim 256, on a 16 GB GPU. It now goes in windows
  // of kEvalWindow timepoints, each with kEvalContext timepoints of context
  // on both sides whose outputs are dropped (the models are trained on
  // sequences of at most 512 timepoints).
  static constexpr int64_t kEvalWindow = 4096;
  static constexpr int64_t kEvalContext = 256;
  std::map<std::string, torch::Tensor> forward_in_windows(
      const std::map<std::string, torch::Tensor>& input,
      const std::vector<std::string>& output_names);

  void train_epoch(size_t batch_size,
                   size_t seq_len,
                   const std::vector<std::string>& output_names,
                   const torch::Tensor& input_batches,
                   const torch::Tensor& gold_batches,
                   torch::optim::Optimizer& opt,
                   epoch_stat_t& stat,
                   const torch::Device& device);

  void train_batch(size_t batch_size,
                   size_t seq_len,
                   const std::vector<std::string>& output_names,
                   const torch::Tensor& input,
                   const torch::Tensor& gold,
                   torch::optim::Optimizer& opt,
                   epoch_stat_t& stat,
                   const torch::Device& device);

  void train_batch(size_t batch_size,
                   size_t seq_len,
                   const std::vector<std::string>& output_names,
                   const std::map<std::string, torch::Tensor>& input,
                   const torch::Tensor& target,
                   torch::optim::Optimizer& opt,
                   epoch_stat_t& stat,
                   const torch::Device& device);

  static std::string generate_script(const std::vector<embd_descr_t>& embd_descr,
                                     const std::vector<rnn_descr_t>& rnn_descr,
                                     const std::vector<std::string>& output_names,
                                     const std::vector<uint32_t>& classes,
                                     float input_dropout_prob);

  std::vector<embd_descr_t> m_embd_descr;
  std::vector<float> m_class_weights;
  std::vector<int64_t> m_eos_classes;
  bool m_select_on_eos = false;
  double m_lr_decay = 0.9;
  size_t m_patience = 10;
};

inline torch::serialize::OutputArchive& operator<<(
    torch::serialize::OutputArchive& archive,
    const BiRnnClassifierImpl& module)
{
  module.save(archive);
  return archive;
}

TORCH_MODULE(BiRnnClassifier);

} // namespace nets
} // namespace deeplima

#endif
