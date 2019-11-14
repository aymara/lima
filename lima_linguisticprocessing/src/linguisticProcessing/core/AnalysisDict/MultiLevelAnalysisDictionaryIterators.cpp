/*
    Copyright 2002-2013 CEA LIST

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
/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                              *
 *                                                                         *
 ***************************************************************************/
#include "MultiLevelAnalysisDictionaryIterators.h"

#include <iostream>
#include <limits>
#ifdef WIN32
#undef min
#undef max
#endif
#include <cassert>

namespace Lima
{

namespace LinguisticProcessing
{

namespace AnalysisDict
{

MultiLevelAnalysisDictionarySubWordIterator::MultiLevelAnalysisDictionarySubWordIterator(
  const std::vector<std::pair<Common::AccessSubWordIterator,Common::AccessSubWordIterator> >& accessItrs,
  const MultiLevelAnalysisDictionary& dico) :
    m_accessItrs(accessItrs),
    m_dico(dico)
{}

MultiLevelAnalysisDictionarySubWordIterator::~MultiLevelAnalysisDictionarySubWordIterator()
{}

const std::pair<uint64_t,DictionaryEntry>
MultiLevelAnalysisDictionarySubWordIterator::operator*() const
{

  std::vector<uint64_t> indexes(m_dico.getDictionaryCount(),0);
  auto indexItr = indexes.begin();
  auto offset = std::numeric_limits< uint64_t >::max();
  for (auto it = m_accessItrs.cbegin();
       it != m_accessItrs.cend(); it++, indexItr++)
  {
    if (it->first != it->second)
    {
      auto data = *(it->first); // clazy:exclude=rule-of-two-soft
      if (data.first < offset)
      {
        offset = data.first;
        std::fill(indexes.begin(), indexes.end(), 0);
        *indexItr = data.second;
      }
      else if (data.first == offset)
      {
        *indexItr = data.second;
      }
    }
  }
  return std::make_pair(offset, m_dico.getEntry(indexes));
}

AbstractDictionarySubWordIterator* MultiLevelAnalysisDictionarySubWordIterator::clone() const
{
  return new MultiLevelAnalysisDictionarySubWordIterator(*this);
}


MultiLevelAnalysisDictionarySubWordIterator& MultiLevelAnalysisDictionarySubWordIterator::operator++(int)
{
  // find lower offset
  auto offset = std::numeric_limits<uint64_t>::max();
  for (auto accessItr : m_accessItrs)
  {
    if (accessItr.first != accessItr.second)
    {
      auto data = *(accessItr.first);
      if (data.first < offset)
      {
        offset = data.first;
      }
    }
  }
  // advance lower offset iteratos
  for (auto accessItr : m_accessItrs)
  {
    if (accessItr.first != accessItr.second)
    {
      auto data = *(accessItr.first);
      if (data.first == offset)
      {
        (accessItr.first)++;
      }
    }
  }
  return *this;
}

bool MultiLevelAnalysisDictionarySubWordIterator::operator==(
    const AbstractDictionarySubWordIterator& abit) const
{
  const auto& it = static_cast<const MultiLevelAnalysisDictionarySubWordIterator&>(abit);
  return m_accessItrs == it.m_accessItrs;
}

bool MultiLevelAnalysisDictionarySubWordIterator::operator!=(const AbstractDictionarySubWordIterator& abit) const
{
  const auto& it = static_cast<const MultiLevelAnalysisDictionarySubWordIterator&>(abit);
  return m_accessItrs != it.m_accessItrs;
}


MultiLevelAnalysisDictionarySuperWordIterator::MultiLevelAnalysisDictionarySuperWordIterator(
  const std::vector<Common::AccessSuperWordIterator>& it) :
    m_accessItrs(it)
{}


MultiLevelAnalysisDictionarySuperWordIterator::MultiLevelAnalysisDictionarySuperWordIterator(
  const MultiLevelAnalysisDictionarySuperWordIterator& source) :
    AbstractDictionarySuperWordIterator(),
    m_accessItrs(source.m_accessItrs)
{}

MultiLevelAnalysisDictionarySuperWordIterator::~MultiLevelAnalysisDictionarySuperWordIterator()
{}

MultiLevelAnalysisDictionarySuperWordIterator& MultiLevelAnalysisDictionarySuperWordIterator::operator++(int)
{
  return *this;
}

bool MultiLevelAnalysisDictionarySuperWordIterator::operator==(const AbstractDictionarySuperWordIterator& /*abit*/) const
{
//  const MultiLevelAnalysisDictionarySuperWordIterator& it=static_cast<const MultiLevelAnalysisDictionarySuperWordIterator&>(abit);
//  return m_accessItr.operator==(it.m_accessItr);
  std::cerr << "unimplemented method at "<<__FILE__<<", line "<<__LINE__<<std::endl;
  assert(false);
  return false;
}

bool MultiLevelAnalysisDictionarySuperWordIterator::operator!=(const AbstractDictionarySuperWordIterator& /*abit*/) const
{
//  const MultiLevelAnalysisDictionarySuperWordIterator& it=static_cast<const MultiLevelAnalysisDictionarySuperWordIterator&>(abit);
//  return m_accessItr.operator!=(it.m_accessItr);
  std::cerr << "unimplemented method at "<<__FILE__<<", line "<<__LINE__<<std::endl;
  assert(false);
  return true;
}

AbstractDictionarySuperWordIterator* MultiLevelAnalysisDictionarySuperWordIterator::clone() const
{
  return new MultiLevelAnalysisDictionarySuperWordIterator(*this);
}

const LimaString MultiLevelAnalysisDictionarySuperWordIterator::operator*() const
{
//  return *m_accessItr;
  std::cerr << "unimplemented method at "<<__FILE__<<", line "<<__LINE__<< std::endl;
  assert(false);
  LimaString res;
  return res;
}

}

}

}
