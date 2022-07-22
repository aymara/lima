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

namespace deeplima
{

template <class Model, class InputVectorizer/*=TorchMatrix<int64_t>*/ >
class RnnSeq2Seq : public InputVectorizer,
                   public Model
{
  typedef RnnSeq2Seq<Model, InputVectorizer> ThisClass;

  std::vector<std::vector<uint8_t>> m_output; // external - classifier id, internal - time position

public:

  typedef StdMatrix<uint8_t> OutputMatrix;

  const OutputMatrix get_output() const
  {
    return OutputMatrix(m_output);
  }

  RnnSeq2Seq()
  {
  }

  void init(uint32_t max_feat,
            uint32_t overlap,
            uint32_t num_slots,
            uint32_t slot_len,
            uint32_t num_threads)
  {
  }

  void load(const std::string& fn)
  {
    Model::load(fn);
  }

  virtual ~RnnSeq2Seq()
  {
    std::cerr << "-> ~RnnSeq2Seq" << std::endl;
    std::cerr << "<- ~RnnSeq2Seq" << std::endl;
  }
};

} // namespace deeplima

#endif
