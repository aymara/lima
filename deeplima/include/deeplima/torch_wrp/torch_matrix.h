// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_SRC_INFERENCE_TORCH_VECTORIZER_H
#define DEEPLIMA_SRC_INFERENCE_TORCH_VECTORIZER_H

#include <torch/torch.h>

namespace deeplima
{

template <class T>
class TorchMatrix
{
public:
  typedef T value_t;
  typedef torch::Tensor tensor_t;

  TorchMatrix()
    : m_start_time(0),
      m_max_time(0),
      m_max_feat(0),
      m_tensor(create_empty_tensor()),
      m_accessor(m_tensor.accessor<T, 2>())
  {}

  TorchMatrix(int64_t max_time, int64_t max_feat)
    : m_start_time(0),
      m_max_time(max_time),
      m_max_feat(max_feat),
      m_tensor(create_tensor(max_time, max_feat)),
      m_accessor(m_tensor.accessor<T, 2>())
  {
    assert(m_start_time >= 0);
    assert(m_max_time > m_start_time);
    //assert(m_max_feat > 0);
    assert(m_max_feat < std::numeric_limits<int64_t>::max());
  }

  TorchMatrix(torch::Tensor input, int64_t start_time, int64_t max_time)
    : m_start_time(start_time),
      m_max_time(max_time),
      m_max_feat(input.size(1)),
      m_tensor(input),
      m_accessor(m_tensor.accessor<T, 2>())
  {
    assert(m_start_time >= 0);
    assert(m_max_time > m_start_time);
    assert(m_max_feat > 0);
    assert(m_max_feat < std::numeric_limits<int64_t>::max());
  }

  void init(int64_t max_time, int64_t max_feat)
  {
    assert(m_start_time >= 0);
    assert(max_time > m_start_time);
    assert(max_feat > 0);
    assert(max_feat < std::numeric_limits<int64_t>::max());

    m_max_time = max_time;
    m_max_feat = max_feat;
    m_tensor = create_tensor(max_time, max_feat);
    m_accessor = m_tensor.accessor<T, 2>();
  }

  inline void set(int64_t time, uint64_t feat, T value)
  {
    assert(feat < std::numeric_limits<uint64_t>::max());
    assert(time < std::numeric_limits<int64_t>::max() - m_start_time);
    assert(time < m_tensor.size(0));

    assert(0 == m_start_time);
    m_accessor[m_start_time + time][feat] = value;
    assert(0 == m_start_time);
  }

  inline uint64_t get(uint64_t time, uint64_t feat)
  {
    assert(feat < std::numeric_limits<uint64_t>::max());
    assert(time < std::numeric_limits<uint64_t>::max() - m_start_time);

    return m_accessor[m_start_time + time][feat];
  }

  inline uint64_t size() const
  {
    return m_max_time - m_start_time;
  }

  inline uint64_t get_max_feat() const
  {
    return m_max_feat;
  }

  const torch::Tensor& get_tensor() const
  {
    return m_tensor;
  }

  void to(torch::Device& device)
  {
    m_tensor.to(device);
  }

protected:

  void create();
  torch::Tensor create_empty_tensor()
  {
    return torch::zeros({0, 0}, torch::TensorOptions().dtype(torch::kInt64));
  }
  torch::Tensor create_tensor(int64_t max_time, int64_t max_feat);

  int64_t m_start_time;
  int64_t m_max_time;
  int64_t m_max_feat;

  torch::Tensor m_tensor;
  torch::TensorAccessor<T, 2> m_accessor;
};

template<>
inline torch::Tensor TorchMatrix<int64_t>::create_tensor(int64_t max_time, int64_t max_feat)
{
  assert(max_time > 0);
  //assert(max_feat > 0);

  return torch::zeros({max_time, max_feat}, torch::TensorOptions().dtype(torch::kInt64));
}

template<>
inline torch::Tensor TorchMatrix<float>::create_tensor(int64_t max_time, int64_t max_feat)
{
  assert(max_time > 0);
  assert(max_feat > 0);

  return torch::zeros({max_time, max_feat}, torch::TensorOptions().dtype(torch::kFloat32));
}

template <class D, class Matrix>
class DictAdapter
{
public:

  DictAdapter() {}

  inline void set(Matrix& target, uint64_t time, uint64_t feat, const typename D::value_t value)
  {
    assert(feat < m_dicts.size());
    int k = m_dicts[feat]->get_idx(value);
    target.set(time, feat, k);
  }

protected:
  std::vector<D*> m_dicts;
};

template<>
inline void TorchMatrix<int64_t>::create()
{
  assert(m_max_time > 0);
  assert(m_max_feat > 0);

  m_tensor = torch::zeros({m_max_time, m_max_feat}, torch::TensorOptions().dtype(torch::kInt64));
  m_accessor = m_tensor.accessor<int64_t, 2>();
}

template<>
inline void TorchMatrix<float>::create()
{
  assert(m_max_time > 0);
  assert(m_max_feat > 0);

  m_tensor = torch::zeros({m_max_time, m_max_feat}, torch::TensorOptions().dtype(torch::kFloat32));
  m_accessor = m_tensor.accessor<float, 2>();
}

} // namespace deeplima

#endif
