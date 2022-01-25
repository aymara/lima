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

#ifndef DEEPLIMA_LIBS_TASKS_LEMMATIZATION_MODEL_SEQ2SEQ_LEMMATIZER_H
#define DEEPLIMA_LIBS_TASKS_LEMMATIZATION_MODEL_SEQ2SEQ_LEMMATIZER_H

#include "static_graph/static_graph.h"
#include "nn/birnn_seq2seq/birnn_seq2seq.h"

#include "train_params_lemmatization.h"
#include "morph_model/morph_model.h"

namespace deeplima
{
namespace lemmatization
{
namespace train
{

class Seq2SeqLemmatizerImpl : public nets::BiRnnSeq2SeqImpl
{
public:
  typedef torch::Tensor tensor_t;

  Seq2SeqLemmatizerImpl()
  {
  }

  Seq2SeqLemmatizerImpl(DictsHolder&& dicts,
                        const morph_model::morph_model_t& lang_morph_model,
                        const std::vector<nets::embd_descr_t>& encoder_embd_descr,
                        const std::vector<nets::rnn_descr_t>& encoder_rnn_descr,
                        const std::vector<nets::embd_descr_t>& decoder_embd_descr,
                        const std::vector<nets::rnn_descr_t>& decoder_rnn_descr,
                        const std::vector<nets::embd_descr_t>& cat_embd_descr)
    : BiRnnSeq2SeqImpl(std::move(dicts),
                       encoder_embd_descr, encoder_rnn_descr,
                       decoder_embd_descr, decoder_rnn_descr,
                       cat_embd_descr),
      m_morph_model(lang_morph_model)
  {
  }

  void train(const train_params_lemmatization_t& params,
             const std::vector<TorchMatrix<int64_t>>& train_input,
             const std::vector<std::vector<TorchMatrix<int64_t>>>& train_input_cat,
             const std::vector<TorchMatrix<int64_t>>& train_gold,
             const std::vector<TorchMatrix<int64_t>>& eval_input,
             const std::vector<std::vector<TorchMatrix<int64_t>>>& eval_input_cat,
             const std::vector<TorchMatrix<int64_t>>& eval_gold,
             torch::optim::Optimizer& opt,
             const torch::Device& device = torch::Device(torch::kCPU));

  void evaluate(const std::vector<TorchMatrix<int64_t>>& input,
                const std::vector<TorchMatrix<int64_t>>& gold,
                nets::epoch_stat_t& stat,
                const torch::Device& device = torch::Device(torch::kCPU));

  void train_on_subset(const train_params_lemmatization_t& params,
                       const TorchMatrix<int64_t>& train_input,
                       const std::vector<TorchMatrix<int64_t>> train_input_cat,
                       const TorchMatrix<int64_t>& train_gold,
                       torch::optim::Optimizer& opt,
                       const torch::Device& device = torch::Device(torch::kCPU));

  virtual void load(torch::serialize::InputArchive& archive);
  virtual void save(torch::serialize::OutputArchive& archive) const;

  void load(const std::string& fn)
  {
    torch::load(*this, fn);
  }

  const morph_model::morph_model_t& get_morph_model() const
  {
    return m_morph_model;
  }

protected:

  morph_model::morph_model_t m_morph_model;
};

inline torch::serialize::OutputArchive& operator<<(
    torch::serialize::OutputArchive& archive,
    const Seq2SeqLemmatizerImpl& module)
{
  module.save(archive);
  return archive;
}

inline torch::serialize::InputArchive& operator>>(
    torch::serialize::InputArchive& archive,
    Seq2SeqLemmatizerImpl& module)
{
  module.load(archive);
  return archive;
}

TORCH_MODULE(Seq2SeqLemmatizer);


} // namespace train
} // namespace lemmatization
} // namespace deeplima

#endif // DEEPLIMA_LIBS_TASKS_LEMMATIZATION_MODEL_SEQ2SEQ_LEMMATIZER_H
