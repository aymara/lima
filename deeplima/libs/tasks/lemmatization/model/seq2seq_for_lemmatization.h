// Copyright 2002-2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
                        const std::vector<nets::embd_descr_t>& cat_embd_descr,
                        const std::string& fixed_upos)
    : BiRnnSeq2SeqImpl(std::move(dicts),
                       encoder_embd_descr, encoder_rnn_descr,
                       decoder_embd_descr, decoder_rnn_descr,
                       cat_embd_descr),
      m_morph_model(lang_morph_model),
      m_fixed_upos(fixed_upos)
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

  const std::string& get_fixed_upos() const
  {
    return m_fixed_upos;
  }

protected:

  morph_model::morph_model_t m_morph_model;
  std::string m_fixed_upos;
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

