// Copyright 2002-2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_LIBS_NN_COMMON_LAYER_DESCR_H
#define DEEPLIMA_LIBS_NN_COMMON_LAYER_DESCR_H

#include <sstream>

namespace deeplima
{
namespace nets
{

struct embd_descr_t
{
  std::string m_name;
  size_t m_dim;
  unsigned char m_type; // 0 - raw, 1 - embd

  embd_descr_t(const std::string& name, size_t dim, unsigned char type=1)
    : m_name(name), m_dim(dim), m_type(type) { }

  explicit embd_descr_t(const std::string& str)
  {
    from_string(str);
  }

  std::string to_string() const
  {
    std::ostringstream s;
    s << m_name << " " << (int)m_dim << " " << (int)m_type;
    return s.str();
  }

  void from_string(const std::string& str)
  {
    std::istringstream s(str);
    s >> m_name;
    s >> m_dim;
    int t = 0;
    s >> t;
    m_type = t;
  }

  bool operator==(const embd_descr_t& other)
  {
    return (other.m_name == m_name) && (other.m_dim == m_dim) && (other.m_type == m_type);
  }
};

struct rnn_descr_t
{
  size_t m_dim;

  explicit rnn_descr_t(size_t dim)
    : m_dim(dim) { }
};

struct deep_biaffine_attention_descr_t
{
  size_t m_arc_dim;
  // size_t m_input_rnn_layer; // 0 - embd, 1 - 1st RNN layer, ...

  explicit deep_biaffine_attention_descr_t(size_t arc_dim)
    : m_arc_dim(arc_dim) { }
};

} // nets
} // deeplima

#endif // DEEPLIMA_LIBS_NN_COMMON_LAYER_DESCR_H

