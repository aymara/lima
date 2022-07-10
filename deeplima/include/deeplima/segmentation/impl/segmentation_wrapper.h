// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_SRC_TRAIN_SEGMENTATION_WRAPPER_H
#define DEEPLIMA_SRC_TRAIN_SEGMENTATION_WRAPPER_H

#include <string>

#include "deeplima/segmentation/impl/char_ngram.h"

namespace deeplima
{
namespace segmentation
{
namespace impl
{

// This class does nothing.
// It only defines the interface of the inference module

template <class M>
class SegmentationInferenceWrapper
{
public:
  SegmentationInferenceWrapper()
  {
  }

  inline void load(const std::string& fn)
  {
    m_impl.load(fn);
  }

  inline size_t init_new_worker(size_t input_len, bool precomputed_input=false)
  {
    return m_impl.init_new_worker(input_len, precomputed_input);
  }

  inline void predict(
      size_t worker_id,
      const typename M::tensor_t& inputs,
      int64_t input_begin,
      int64_t input_end,
      int64_t output_begin,
      int64_t output_end,
      std::vector<std::vector<uint8_t>>& output,
      const std::vector<std::string>& output_names
      )
  {
    m_impl.predict(worker_id, inputs,
                   input_begin, input_end,
                   output_begin, output_end,
                   output,
                   output_names);
  }

  inline const std::vector<ngram_descr_t>& get_ngram_descr() const
  {
    return m_impl.get_ngram_descr();
  }

  inline const typename M::dicts_holder_t& get_dicts() const
  {
    return m_impl.get_uint_dicts();
  }

  inline const std::vector<std::vector<std::string>>& get_classes() const
  {
    return m_impl.get_classes();
  }

protected:

  M m_impl;
};

} // namespace impl
} // namespace segmenation
} // namespace deeplima

#endif
