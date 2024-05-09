// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_SEGMENTATION_IMPL_H
#define DEEPLIMA_SEGMENTATION_IMPL_H

#include "deeplima/utils/locked_buffer.h"

#include "deeplima/nets/birnn_seq_cls.h"

#include "deeplima/eigen_wrp/eigen_matrix.h"
#include "deeplima/eigen_wrp/embd_dict.h"
#include "deeplima/eigen_wrp/dict_embd_vectorizer.h"
#include "deeplima/eigen_wrp/segmentation_eigen_inference_impl.h"

// #include "config.h"

#include "utf8_reader.h"
#include "char_ngram_encoder.h"
#include "segmentation_decoder.h"
#include "segmentation_wrapper.h"


namespace deeplima
{
namespace segmentation
{

class ISegmentation
{
public:
  typedef std::function < bool (uint8_t* buffer,
                                int32_t& read,
                                int32_t max) > read_callback_t;

  virtual void parse_from_stream(const read_callback_t fn) = 0;
  virtual void register_handler(const segmentation_callback_t fn) = 0;
    /** Cleanup all remaining locks if any. */
  virtual void finalize() = 0;

  virtual ~ISegmentation() { }
};

namespace eigen_impl
{
  typedef impl::SegmentationInferenceWrapper<BiRnnEigenInferenceForSegmentation> Model;
  typedef DictEmbdVectorizer<EmbdUInt64FloatHolder, EmbdUInt64Float, eigen_wrp::EigenMatrixXf> EmbdVectorizer;
}

namespace impl {
  using CharNgramEncoderFromUtf8 = CharNgramEncoder< Utf8Reader<> > ;

  using SegmentationClassifier = RnnSequenceClassifier<eigen_impl::Model, eigen_impl::EmbdVectorizer, uint8_t> ;
  using InputEncoder = CharNgramEncoderFromUtf8;
  using OutputDecoder = SegmentationDecoder;

class SegmentationImpl: public ISegmentation, public SegmentationClassifier
{
public:

  SegmentationImpl();

  SegmentationImpl(
      const std::vector<ngram_descr_t>& ngram_descr,
      size_t threads,
      size_t buffer_size_per_thread
    );

  virtual ~SegmentationImpl() = default;

  virtual void load(const std::string& fn);

  void init(size_t threads, size_t buffer_size_per_thread);

  virtual void parse_from_stream(const read_callback_t fn) override;

  virtual void register_handler(const segmentation_callback_t fn) override;

  /** Cleanup all remaining locks if any. */
  virtual void finalize() override;

protected:

  void vectorize_timepoint(uint64_t timepoint);

  void increment_timepoint(uint64_t& timepoint);

  void send_results(int32_t slot_idx);

  void send_next_results();

  void acquire_slot();

  void handle_timepoint();

  void no_more_data();

protected:
  std::vector<uint8_t> m_char_len;

  InputEncoder m_input_encoder;
  OutputDecoder m_decoder;

  uint64_t m_current_timepoint;
  uint32_t m_current_slot_timepoints;

  int32_t m_current_slot_no;
  int32_t m_last_completed_slot;

  locked_buffer_set_t m_buff_set;
  size_t m_curr_buff_idx;
};

} // namespace impl
} // namespace segmentation
} // namespace deeplima

#endif
