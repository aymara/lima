// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_DUMPER_CONLLU_H
#define DEEPLIMA_DUMPER_CONLLU_H

#include <iostream>

#include "segmentation.h"

namespace deeplima
{
namespace dumper
{

class AbstractDumper
{
protected:
  uint64_t m_token_counter;

  inline void increment_token_counter()
  {
    ++m_token_counter;
  }

public:
  virtual void operator()(const std::vector<deeplima::segmentation::token_pos>& tokens, uint32_t len) = 0;

  uint64_t get_token_counter() const
  {
    return m_token_counter;
  }

  AbstractDumper()
    : m_token_counter(0) { }

  virtual ~AbstractDumper() { }
};

class Horizontal : public AbstractDumper
{
public:
  Horizontal()
  {}

  virtual void operator()(const std::vector<deeplima::segmentation::token_pos>& tokens, uint32_t len)
  {
    std::string temp;
    for (size_t i = 0; i < len; i++)
    {
      const char* ptoken = tokens[i].m_pch;
      std::ostringstream s;
      if (temp.size() > 0)
      {
        s << temp;
        temp.clear();
      }

      for (size_t j = 0; j < tokens[i].m_len; j++)
      {
        if (*ptoken == '\r' || *ptoken == '\n' || *ptoken == '\t')
        {
          s << " ";
        }
        else
        {
          s << *ptoken;
        }
        ptoken++;
      }
      std::string str = s.str();
      if (std::string::npos == str.find_first_not_of(' '))
      {
        temp = str;
        continue;
      }
      std::cout << str << " ";

      if (tokens[i].m_flags & deeplima::segmentation::token_pos::flag_t::sentence_brk ||
          tokens[i].m_flags & deeplima::segmentation::token_pos::flag_t::paragraph_brk)
      {
        std::cout << std::endl;
      }
    }
  }
};

class TokensToConllU : public AbstractDumper
{
public:
  TokensToConllU()
    : m_next_token_idx(1)
  {}

  ~TokensToConllU()
  {
    if (m_next_token_idx > 1)
    {
      std::cout << std::endl;
    }
  }

  virtual void operator()(const std::vector<deeplima::segmentation::token_pos>& tokens, uint32_t len)
  {
    std::string temp;
    for (size_t i = 0; i < len; i++)
    {
      const char* ptoken = tokens[i].m_pch;
      std::ostringstream s;
      if (temp.size() > 0)
      {
        s << temp;
        temp.clear();
      }
      for (size_t j = 0; j < tokens[i].m_len; j++)
      {
        if (*ptoken == '\r' || *ptoken == '\n' || *ptoken == '\t')
        {
          s << " ";
        }
        else
        {
          s << *ptoken;
        }
        ptoken++;
      }
      std::string str = s.str();
      if (std::string::npos == str.find_first_not_of(' '))
      {
        temp = str;
        continue;
      }
      std::cout << m_next_token_idx << "\t";
      std::cout << str;
      std::cout << "\t_\t_\t_\t_\t";
      std::cout << m_next_token_idx - 1;
      std::cout << "\t_\t_\t_" << std::endl;

      increment_token_counter();

      m_next_token_idx += 1;
      if (tokens[i].m_flags & deeplima::segmentation::token_pos::flag_t::sentence_brk ||
          tokens[i].m_flags & deeplima::segmentation::token_pos::flag_t::paragraph_brk)
      {
        std::cout << std::endl;
        m_next_token_idx = 1;
      }
    }
  }

protected:
  uint32_t m_next_token_idx;
};

template <class I>
class AnalysisToConllU
{
protected:
  uint64_t m_token_counter;
  uint32_t m_next_token_idx;

  inline void increment_token_counter()
  {
    ++m_token_counter;
  }

  std::vector<std::string> m_class_names;
  std::vector<std::vector<std::string>> m_classes;
  bool m_has_feats;

  size_t m_first_feature_to_print;

public:
  AnalysisToConllU()
    : m_token_counter(0),
      m_next_token_idx(1),
      m_has_feats(false),
      m_first_feature_to_print(0)
  {
  }

  ~AnalysisToConllU()
  {
    if (m_next_token_idx > 1)
    {
      std::cout << std::endl;
    }
  }

  uint64_t get_token_counter() const
  {
    return m_token_counter;
  }

  void set_classes(size_t idx, const std::string& class_name, const std::vector<std::string>& data)
  {
    m_class_names.push_back(class_name);

    if (idx + 1 > m_classes.size())
    {
      m_classes.resize(idx + 1);
    }
    assert(0 == m_classes[idx].size());
    m_classes[idx] = data;

    if (m_classes.size() > 1)
    {
      m_has_feats = true;
      for (size_t i = 0; i < m_class_names.size(); ++i)
      {
        const std::string& feat_name = m_class_names[i];
        if (feat_name == "upos" || feat_name == "eos")
        {
          continue;
        }
        m_first_feature_to_print = i;
        break;
      }
    }
  }

  std::string generate_feats(const I& iter)
  {
    std::string feat_str;

    for (size_t i = m_first_feature_to_print; i < m_classes.size(); ++i)
    {
      if (0 != iter.token_class(i))
      {
        if (!feat_str.empty())
        {
          feat_str += "|";
        }

        feat_str += m_class_names[i];
        feat_str += "=";
        feat_str += m_classes[i][iter.token_class(i)];
      }
    }

    return feat_str;
  }

  void operator()(I& iter)
  {
    std::string temp;
    while (!iter.end())
    {
      const char* ptoken = iter.form();
      std::ostringstream s;
      if (temp.size() > 0)
      {
        s << temp;
        temp.clear();
      }
      while (0 != *ptoken)
      {
        if (*ptoken == '\r' || *ptoken == '\n' || *ptoken == '\t')
        {
          s << " ";
        }
        else
        {
          s << *ptoken;
        }
        ptoken++;
      }
      std::string str = s.str();
      if (std::string::npos == str.find_first_not_of(' '))
      {
        temp = str;
        continue;
      }
      std::cout << m_next_token_idx << "\t";
      std::cout << str << "\t_";
      if (true)
      {
        std::cout << "\t" << m_classes[0][iter.token_class(0)];
      }
      else
      {
        std::cout << "\t_";
      }

      std::cout << "\t_\t";

      if (m_has_feats)
      {
        std::cout << generate_feats(iter);
      }
      else
      {
        std::cout << "_";
      }

      std::cout << "\t";
      std::cout << m_next_token_idx - 1;
      std::cout << "\t_\t_\t_" << std::endl;

      increment_token_counter();

      m_next_token_idx += 1;
      if (iter.flags() & deeplima::segmentation::token_pos::flag_t::sentence_brk ||
          iter.flags() & deeplima::segmentation::token_pos::flag_t::paragraph_brk)
      {
        std::cout << std::endl;
        m_next_token_idx = 1;
      }
      iter.next();
    }
  }
};

} // namespace dumper
} // namespace deeplima

#endif
