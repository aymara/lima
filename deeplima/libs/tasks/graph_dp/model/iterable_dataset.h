/*
    Copyright 2022 CEA LIST

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

#ifndef DEEPLIMA_LIBS_TASKS_GRAPH_DP_ITERABLE_DATASET_H
#define DEEPLIMA_LIBS_TASKS_GRAPH_DP_ITERABLE_DATASET_H

#include <torch/torch.h>

namespace deeplima
{
namespace graph_dp
{
namespace train
{

class BatchIterator
{
public:
  class Batch
  {
  public:
    inline bool empty() const
    {
      return m_gold.size(m_batch_dim) == 0;
    }

    inline size_t get_batch_size() const
    {
      return m_gold.size(m_batch_dim);
    }

    Batch() { }

    Batch(const torch::Tensor& trainable_input,
          const torch::Tensor& frozen_input,
          const torch::Tensor& gold,
          size_t batch_dim=0)
      : m_batch_dim(batch_dim),
        m_trainable_input(trainable_input),
        m_frozen_input(frozen_input),
        m_gold(gold)
    {
    }

    inline const torch::Tensor& trainable_input() const
    {
      return m_trainable_input;
    }

    inline const torch::Tensor& frozen_input() const
    {
      return m_frozen_input;
    }

    inline const torch::Tensor& gold() const
    {
      return m_gold;
    }

  protected:
    const size_t m_batch_dim = 0;

    const torch::Tensor m_trainable_input;
    const torch::Tensor m_frozen_input;
    const torch::Tensor m_gold;
  };
  virtual ~BatchIterator() = default;
  virtual void set_batch_size(int64_t batch_size) = 0;
  virtual void start_epoch() = 0;
  virtual bool end() = 0;
  virtual const Batch next_batch() = 0;
};

class IterableDataSet
{
public:
  virtual std::shared_ptr<BatchIterator> get_iterator() const = 0;
};

} // train
} // graph_dp
} // deeplima

#endif // DEEPLIMA_LIBS_TASKS_GRAPH_DP_ITERABLE_DATASET_H
