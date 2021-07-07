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
public:
  virtual void operator()(const std::vector<deeplima::segmentation::token_pos>& tokens, uint32_t len) = 0;

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
  std::vector<std::vector<std::string>> m_classes;

public:
  AnalysisToConllU()
    : m_next_token_idx(1)
  {
  }

  ~AnalysisToConllU()
  {
    if (m_next_token_idx > 1)
    {
      std::cout << std::endl;
    }
  }

  void set_classes(size_t idx, const std::vector<std::string>& data)
  {
    if (idx + 1 > m_classes.size())
    {
      m_classes.resize(idx + 1);
    }
    assert(0 == m_classes[idx].size());
    m_classes[idx] = data;
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
      std::cout << "\t_\t_\t";
      std::cout << m_next_token_idx - 1;
      std::cout << "\t_\t_\t_" << std::endl;
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

protected:
  uint32_t m_next_token_idx;
};

} // namespace dumper
} // namespace deeplima

#endif
