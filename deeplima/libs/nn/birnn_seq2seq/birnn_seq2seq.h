// Copyright 2002-2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_LIBS_NN_BIRNN_SEQ2SEQ_H
#define DEEPLIMA_LIBS_NN_BIRNN_SEQ2SEQ_H

#include "nn/birnn_seq_cls/birnn_seq_classifier.h"

namespace deeplima
{
namespace nets
{

class BiRnnSeq2SeqImpl : public BiRnnClassifierImpl
{
public:
  typedef torch::Tensor tensor_t;

  BiRnnSeq2SeqImpl() {}

  BiRnnSeq2SeqImpl(DictsHolder&& dicts,
                        const std::vector<embd_descr_t>& encoder_embd_descr,
                        const std::vector<rnn_descr_t>& encoder_rnn_descr,
                        const std::vector<embd_descr_t>& decoder_embd_descr,
                        const std::vector<rnn_descr_t>& decoder_rnn_descr,
                        const std::vector<embd_descr_t>& cat_embd_descr)
    : BiRnnClassifierImpl(std::move(dicts), concatenate(encoder_embd_descr, decoder_embd_descr, cat_embd_descr),
                      generate_script(encoder_embd_descr, encoder_rnn_descr,
                                      decoder_embd_descr, decoder_rnn_descr,
                                      cat_embd_descr, dicts[1]->size())),
      m_cat_embd_descr(cat_embd_descr)
  {
  }

  virtual void load(torch::serialize::InputArchive& archive);
  virtual void save(torch::serialize::OutputArchive& archive) const;

  void evaluate(const std::vector<std::string>& output_names,
                const TorchMatrix<int64_t>& input,
                const TorchMatrix<int64_t>& gold,
                epoch_stat_t& stat,
                const torch::Device& device = torch::Device(torch::kCPU));

protected:
  std::string generate_script(const std::vector<embd_descr_t>& encoder_embd_descr,
                              const std::vector<rnn_descr_t>& encoder_rnn_descr,
                              const std::vector<embd_descr_t>& decoder_embd_descr,
                              const std::vector<rnn_descr_t>& decoder_rnn_descr,
                              const std::vector<embd_descr_t>& cat_embd_descr,
                              size_t n_output_classes);

  void train_batch(const std::vector<std::string>& output_names,
                   const torch::Tensor& input,
                   const std::vector<torch::Tensor>& input_cat,
                   const torch::Tensor& target,
                   torch::optim::Optimizer& opt,
                   epoch_stat_t& stat,
                   const torch::Device& device);

  template <class T>
  static std::vector<T> concatenate(const std::vector<T>& a,
                                    const std::vector<T>& b,
                                    const std::vector<T>& c = {})
  {
    std::vector<T> out = a;
    out.insert(out.end(), b.begin(), b.end());
    out.insert(out.end(), c.begin(), c.end());
    return out;
  }

  std::vector<embd_descr_t> m_cat_embd_descr;

};

inline torch::serialize::OutputArchive& operator<<(
    torch::serialize::OutputArchive& archive,
    const BiRnnSeq2SeqImpl& module)
{
  module.save(archive);
  return archive;
}

inline torch::serialize::InputArchive& operator>>(
    torch::serialize::InputArchive& archive,
    BiRnnSeq2SeqImpl& module)
{
  module.load(archive);
  return archive;
}

TORCH_MODULE(BiRnnSeq2Seq);

} // namespace nets
} // namespace deeplima

#endif // DEEPLIMA_LIBS_NN_BIRNN_SEQ2SEQ_H
