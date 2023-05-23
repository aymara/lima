// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_SRC_TRAIN_BIRNN_CLASSIFIER_FOR_SEGMENTATION_H
#define DEEPLIMA_SRC_TRAIN_BIRNN_CLASSIFIER_FOR_SEGMENTATION_H

#include "deeplima/segmentation/impl/char_ngram_encoder.h"
#include "nn/birnn_seq_cls/birnn_seq_classifier.h"
#include "train_params_tagging.h"

namespace deeplima
{
namespace tagging
{
namespace train
{

class BiRnnClassifierForNerImpl : public nets::BiRnnClassifierImpl
{
public:
  typedef torch::Tensor tensor_t;
  typedef DictsHolder dicts_holder_t;

  BiRnnClassifierForNerImpl()
    : m_workers(0)
  {
  }

  BiRnnClassifierForNerImpl(DictsHolder&& dicts,
                            const std::vector<nets::embd_descr_t>& embd_descr,
                            const std::vector<nets::rnn_descr_t>& rnn_descr,
                            const std::vector<std::string>& output_names,
                            DictsHolder&& classes,
                            const std::string& embd_fn,
                            const train_params_tagging_t& params)
    : BiRnnClassifierImpl(std::move(dicts),
                          embd_descr,
                          rnn_descr,
                          output_names,
                          classes.get_counters(),
                          params.m_input_dropout_prob),
      m_workers(0),
      m_class_names(output_names),
      m_embd_fn(embd_fn)
  {
    m_classes = classes;
  }

  virtual void load(torch::serialize::InputArchive& archive);
  virtual void save(torch::serialize::OutputArchive& archive) const;

  void load(const std::string& fn)
  {
    torch::load(*this, fn);
  }

  size_t init_new_worker(size_t )
  {
    return m_workers++;
  }

  void train(const train_params_tagging_t& params,
             const std::vector<std::string>& output_names,
             const TorchMatrix<int64_t>& train_trainable_input,
             const TorchMatrix<float>& train_non_trainable_input,
             const TorchMatrix<int64_t>& train_gold,
             const TorchMatrix<int64_t>& eval_trainable_input,
             const TorchMatrix<float>& eval_non_trainable_input,
             const TorchMatrix<int64_t>& eval_gold,
             torch::optim::Optimizer& opt,
             double& best_eval_accuracy,
             const torch::Device& device = torch::Device(torch::kCPU));

  void evaluate(const std::vector<std::string>& output_names,
                const TorchMatrix<int64_t>& trainable_input,
                const TorchMatrix<float>& nontrainable_input,
                const TorchMatrix<int64_t>& gold,
                nets::epoch_stat_t& stat,
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

  const DictsHolder& get_classes() const
  {
    return m_classes;
  }

  const std::vector<std::string>& get_class_names() const
  {
    return m_class_names;
  }

  const std::string& get_embd_fn([[maybe_unused]] size_t idx) const
  {
    assert(0 == idx);
    return m_embd_fn;
  }

protected:
  void train_epoch(int64_t batch_size,
                   size_t seq_len,
                   const std::vector<std::string>& output_names,
                   const torch::Tensor& trainable_input_batches,
                   const torch::Tensor& nontrainable_input_batches,
                   const torch::Tensor& gold_batches,
                   torch::optim::Optimizer& opt,
                   nets::epoch_stat_t& stat,
                   const torch::Device& device);

  void train_batch(size_t batch_size,
                   size_t seq_len,
                   const std::vector<std::string>& output_names,
                   const torch::Tensor& trainable_input,
                   const torch::Tensor& nontrainable_input,
                   const torch::Tensor& gold,
                   torch::optim::Optimizer& opt,
                   nets::epoch_stat_t& stat,
                   const torch::Device& device);

  size_t m_workers;
  std::vector<std::string> m_class_names;
  DictsHolder m_classes;
  std::string m_embd_fn;
};

inline torch::serialize::OutputArchive& operator<<(
    torch::serialize::OutputArchive& archive,
    const BiRnnClassifierForNerImpl& module)
{
  module.save(archive);
  return archive;
}

inline torch::serialize::InputArchive& operator>>(
    torch::serialize::InputArchive& archive,
    BiRnnClassifierForNerImpl& module)
{
  module.load(archive);
  return archive;
}

TORCH_MODULE(BiRnnClassifierForNer);

} // namespace train
} // namespace tagging
} // namespace deeplima

#endif
