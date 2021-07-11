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

#ifndef DEEPLIMA_SRC_TRAIN_BIRNN_CLASSIFIER_FOR_SEGMENTATION_H
#define DEEPLIMA_SRC_TRAIN_BIRNN_CLASSIFIER_FOR_SEGMENTATION_H

#include "deeplima/segmentation/impl/char_ngram_encoder.h"
#include "nn/birnn_seq_cls/birnn_seq_classifier.h"

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
                            const std::string& output_name,
                            DictsHolder&& classes,
                            const std::string& embd_fn)
    : BiRnnClassifierImpl(std::move(dicts), embd_descr, rnn_descr, output_name, classes[0]->size()),
      m_workers(0),
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

  size_t init_new_worker(size_t input_len)
  {
    return m_workers++;
  }

  void train(size_t epochs,
             size_t batch_size,
             size_t seq_len,
             const std::string& output_name,
             const TorchMatrix<int64_t>& train_trainable_input,
             const TorchMatrix<float>& train_non_trainable_input,
             const TorchMatrix<int64_t>& train_gold,
             const TorchMatrix<int64_t>& eval_trainable_input,
             const TorchMatrix<float>& eval_non_trainable_input,
             const TorchMatrix<int64_t>& eval_gold,
             torch::optim::Optimizer& opt,
             const std::string& model_name,
             const torch::Device& device = torch::Device(torch::kCPU));

  std::tuple<double, double, int64_t> evaluate(const std::string& output_name,
                                               const TorchMatrix<int64_t>& trainable_input,
                                               const TorchMatrix<float>& nontrainable_input,
                                               const TorchMatrix<int64_t>& gold,
                                               const torch::Device& device = torch::Device(torch::kCPU));

  void predict(size_t worker_id,
               const torch::Tensor& inputs,
               int64_t input_begin,
               int64_t input_end,
               int64_t output_begin,
               int64_t output_end,
               std::vector<std::vector<uint8_t>>& output,
               const std::vector<std::string>& outputs_names,
               const torch::Device& device = torch::Device(torch::kCPU));

  const DictsHolder& get_classes() const
  {
    return m_classes;
  }

  const std::string& get_embd_fn(size_t idx) const
  {
    assert(0 == idx);
    return m_embd_fn;
  }

protected:
  std::tuple<double, double, int64_t> train_epoch(size_t batch_size,
                                        size_t seq_len,
                                        const std::string& output_name,
                                        const torch::Tensor& trainable_input_batches,
                                        const torch::Tensor& nontrainable_input_batches,
                                        const torch::Tensor& gold_batches,
                                        torch::optim::Optimizer& opt,
                                        const torch::Device& device);

  std::tuple<double, int64_t> train_batch(size_t batch_size,
                                           size_t seq_len,
                                           const std::string& output_name,
                                           const torch::Tensor& trainable_input,
                                           const torch::Tensor& nontrainable_input,
                                           const torch::Tensor& gold,
                                           torch::optim::Optimizer& opt,
                                           const torch::Device& device);

  size_t m_workers;
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
