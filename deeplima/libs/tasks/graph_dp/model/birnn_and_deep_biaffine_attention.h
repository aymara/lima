/*
    Copyright 2022 CEA LIST

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

#ifndef DEEPLIMA_LIBS_TASKS_GRAPH_DP_BIRNN_AND_DEEP_BIAFFINE_ATTENTION_H
#define DEEPLIMA_LIBS_TASKS_GRAPH_DP_BIRNN_AND_DEEP_BIAFFINE_ATTENTION_H

#include "deeplima/segmentation/impl/char_ngram_encoder.h"
#include "nn/birnn_seq_cls/birnn_seq_classifier.h"
#include "train_params_graph_dp.h"
#include "iterable_dataset.h"

namespace deeplima
{
namespace graph_dp
{
namespace train
{

class BiRnnAndDeepBiaffineAttentionImpl : public nets::BiRnnClassifierImpl
{
public:
  typedef torch::Tensor tensor_t;
  typedef DictsHolder dicts_holder_t;

  BiRnnAndDeepBiaffineAttentionImpl()
    : m_workers(0)
  {
  }

  BiRnnAndDeepBiaffineAttentionImpl(DictsHolder&& dicts,
                            const std::vector<nets::embd_descr_t>& embd_descr,
                            const std::vector<nets::rnn_descr_t>& rnn_descr,
                            const std::vector<nets::deep_biaffine_attention_descr_t>& decoder_descr,
                            const std::vector<std::string>& output_names,
                            DictsHolder&& classes,
                            const std::string& embd_fn,
                            bool input_includes_root)
    : BiRnnClassifierImpl(std::move(dicts),
                          embd_descr,
                          generate_script(embd_descr, rnn_descr, decoder_descr, output_names, input_includes_root)
                          /*rnn_descr, output_names, classes.get_counters()*/),
      m_workers(0),
      m_output_class_names(output_names),
      m_embd_fn(embd_fn)
  {
    m_input_classes = classes;
    m_input_class_names.reserve(embd_descr.size());
    for ( const auto& d : embd_descr )
    {
      m_input_class_names.emplace_back(d.m_name);
    }
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

  void train(const train_params_graph_dp_t& params,
             const std::vector<std::string>& output_names,
             const IterableDataSet& train_batches,
             const IterableDataSet& eval_batches,
             torch::optim::Optimizer& opt,
             double& best_eval_accuracy,
             const torch::Device& device = torch::Device(torch::kCPU));

  void evaluate(const std::vector<std::string>& output_names,
                std::shared_ptr<BatchIterator> dataset_iterator,
                nets::epoch_stat_t& stat,
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
    return m_output_classes;
  }

  const std::vector<std::string>& get_output_class_names() const
  {
    return m_output_class_names;
  }

  const std::vector<std::string>& get_input_class_names() const
  {
    return m_input_class_names;
  }

  const std::string& get_embd_fn(size_t idx) const
  {
    assert(0 == idx);
    return m_embd_fn;
  }

protected:
  void train_epoch(size_t batch_size,
                   size_t seq_len,
                   const std::vector<std::string>& output_names,
                   std::shared_ptr<BatchIterator> train_iterator,
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

  void evaluate(const std::vector<std::string>& output_names,
                const torch::Tensor& trainable_input,
                const torch::Tensor& nontrainable_input,
                const torch::Tensor& gold,
                nets::epoch_stat_t& stat,
                const torch::Device& device);

  static std::string generate_script(const std::vector<nets::embd_descr_t>& embd_descr,
                                     const std::vector<nets::rnn_descr_t>& rnn_descr,
                                     const std::vector<nets::deep_biaffine_attention_descr_t>& decoder_descr,
                                     const std::vector<std::string>& output_names,
                                     bool input_includes_root=false/*,
                                     const std::vector<uint32_t>& classes*/);

  size_t m_workers;
  std::vector<std::string> m_input_class_names;
  DictsHolder m_input_classes;
  std::vector<std::string> m_output_class_names;
  DictsHolder m_output_classes;
  std::string m_embd_fn;
};

inline torch::serialize::OutputArchive& operator<<(
    torch::serialize::OutputArchive& archive,
    const BiRnnAndDeepBiaffineAttentionImpl& module)
{
  module.save(archive);
  return archive;
}

inline torch::serialize::InputArchive& operator>>(
    torch::serialize::InputArchive& archive,
    BiRnnAndDeepBiaffineAttentionImpl& module)
{
  module.load(archive);
  return archive;
}

TORCH_MODULE(BiRnnAndDeepBiaffineAttention);

} // train
} // graph_dp
} // deeplima

#endif // DEEPLIMA_LIBS_TASKS_GRAPH_DP_BIRNN_AND_DEEP_BIAFFINE_ATTENTION_H
