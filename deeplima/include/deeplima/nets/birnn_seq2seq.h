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
