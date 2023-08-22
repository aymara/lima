// Copyright 2002-2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_SRC_INFERENCE_RNN_SEQ2SEQ_H
#define DEEPLIMA_SRC_INFERENCE_RNN_SEQ2SEQ_H

#include <vector>
#include <chrono>
#include <cstdlib>

#include "deeplima/utils/std_matrix.h"
#include "deeplima/eigen_wrp/eigen_matrix.h"
#include "deeplima/eigen_wrp/lemmatization_eigen_inference_impl.h"
// #include "deeplima/lemmatization/impl/lemmatization_wrapper.h"

namespace deeplima
{

class RnnSeq2Seq : public eigen_wrp::EigenMatrixXf,
                   public lemmatization::eigen_impl::BiRnnSeq2SeqEigenInferenceForLemmatization
{
  using Model = lemmatization::eigen_impl::BiRnnSeq2SeqEigenInferenceForLemmatization;
  std::shared_ptr< StdMatrix<uint8_t> > m_output; // external - classifier id, internal - time position

public:

  std::shared_ptr< StdMatrix<uint8_t> > get_output()
  {
    return m_output;
  }

  RnnSeq2Seq()
  {
  }

  void init(uint32_t /*max_feat*/,
            uint32_t /*overlap*/,
            uint32_t /*num_slots*/,
            uint32_t /*slot_len*/,
            uint32_t /*num_threads*/)
  {
    std::cerr << "RnnSeq2Seq::init NOT IMPLEMENTED" << std::endl;
  }

  void load(const std::string& fn)
  {
    try
    {
      Model::load(fn);
    }
    catch (const std::runtime_error& e)
    {
      throw;
    }
  }

  virtual ~RnnSeq2Seq()
  {
    // std::cerr << "-> ~RnnSeq2Seq" << std::endl;
    // std::cerr << "<- ~RnnSeq2Seq" << std::endl;
  }
};

} // namespace deeplima

#endif
