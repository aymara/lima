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

#ifndef DEEPLIMA_LIBS_NN_BIRNN_SEQ_CLASSIFIER_H
#define DEEPLIMA_LIBS_NN_BIRNN_SEQ_CLASSIFIER_H

#include <tuple>
#include <vector>
#include <sstream>

#include "static_graph/static_graph.h"
#include "deeplima/torch_wrp/torch_matrix.h"

namespace deeplima
{
namespace nets
{

struct embd_descr_t
{
  std::string m_name;
  size_t m_dim;
  unsigned char m_type; // 0 - raw, 1 - embd

  embd_descr_t(const std::string& name, size_t dim, unsigned char type=1)
    : m_name(name), m_dim(dim), m_type(type) { }

  explicit embd_descr_t(const std::string& str)
  {
    from_string(str);
  }

  std::string to_string() const
  {
    std::ostringstream s;
    s << m_name << " " << (int)m_dim << " " << (int)m_type;
    return s.str();
  }

  void from_string(const std::string& str)
  {
    std::istringstream s(str);
    s >> m_name;
    s >> m_dim;
    int t = 0;
    s >> t;
    m_type = t;
  }

  bool operator==(const embd_descr_t& other)
  {
    return (other.m_name == m_name) && (other.m_dim == m_dim) && (other.m_type == m_type);
  }
};

struct rnn_descr_t
{
  size_t m_dim;

  explicit rnn_descr_t(size_t dim)
    : m_dim(dim) { }
};

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

  task_stat_t()
    : m_items(0), m_correct(0), m_num_classes(0), m_accuracy(0), m_loss(0), m_precision(0), m_recall(0), m_f1(0) { }
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
                      const std::vector<uint32_t>& classes)
    : StaticGraphImpl(dicts,
                      generate_script(embd_descr, rnn_descr, output_names, classes)),
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
               std::vector<std::vector<uint8_t>>& output,
               const std::vector<std::string>& outputs_names,
               const torch::Device& device = torch::Device(torch::kCPU));

protected:

  void split_input(const torch::Tensor& src,
                   std::map<std::string, torch::Tensor>& dst,
                   const torch::Device& device);

  void evaluate(const std::vector<std::string>& output_names,
                const std::map<std::string, torch::Tensor>& input,
                const torch::Tensor& target,
                epoch_stat_t& stat,
                const torch::Device& device);

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
                                     const std::vector<uint32_t>& classes);

  std::vector<embd_descr_t> m_embd_descr;
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
