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
namespace segmentation
{
namespace train
{

class BiRnnClassifierForSegmentationImpl : public nets::BiRnnClassifierImpl
{
public:
  typedef torch::Tensor tensor_t;
  typedef DictsHolder dicts_holder_t;

  BiRnnClassifierForSegmentationImpl()
    : m_workers(0)
  {
  }

  BiRnnClassifierForSegmentationImpl(DictsHolder&& dicts,
                                     const std::vector<impl::ngram_descr_t>& ngram_descr,
                                     const std::vector<nets::embd_descr_t>& embd_descr,
                                     const std::vector<nets::rnn_descr_t>& rnn_descr,
                                     const std::string& output_name,
                                     size_t num_classes)
    : BiRnnClassifierImpl(std::move(dicts), embd_descr, rnn_descr, { output_name }, { num_classes }),
      m_ngram_descr(ngram_descr),
      m_workers(0)
  {
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

  const std::vector<impl::ngram_descr_t>& get_ngram_descr() const
  {
    return m_ngram_descr;
  }

protected:
  std::vector<impl::ngram_descr_t> m_ngram_descr;
  size_t m_workers;
};

inline torch::serialize::OutputArchive& operator<<(
    torch::serialize::OutputArchive& archive,
    const BiRnnClassifierForSegmentationImpl& module)
{
  module.save(archive);
  return archive;
}

inline torch::serialize::InputArchive& operator>>(
    torch::serialize::InputArchive& archive,
    BiRnnClassifierForSegmentationImpl& module)
{
  module.load(archive);
  return archive;
}

TORCH_MODULE(BiRnnClassifierForSegmentation);

} // namespace train
} // namespace segmentation
} // namespace deeplima

#endif

