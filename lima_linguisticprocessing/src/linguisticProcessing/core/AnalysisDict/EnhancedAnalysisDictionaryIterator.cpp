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
#include "EnhancedAnalysisDictionaryIterator.h"

namespace Lima
{

namespace LinguisticProcessing
{

namespace AnalysisDict
{

EnhancedAnalysisDictionarySubWordIterator::EnhancedAnalysisDictionarySubWordIterator(
  const Common::AccessSubWordIterator& accessItr,
  const EnhancedAnalysisDictionary& dico) :
    m_accessItr(accessItr),
    m_dico(dico)
{}

EnhancedAnalysisDictionarySubWordIterator::~EnhancedAnalysisDictionarySubWordIterator()
{}

const std::pair<uint64_t,DictionaryEntry>
EnhancedAnalysisDictionarySubWordIterator::operator*() const
{
  const std::pair<uint64_t,uint64_t> pos = *m_accessItr;
  return std::make_pair(pos.first,m_dico.getEntryData(static_cast<StringsPoolIndex>(pos.second)));
}

AbstractDictionarySubWordIterator* EnhancedAnalysisDictionarySubWordIterator::clone() const
{
  return new EnhancedAnalysisDictionarySubWordIterator(*this);
}

EnhancedAnalysisDictionarySuperWordIterator::EnhancedAnalysisDictionarySuperWordIterator(
  const Common::AccessSuperWordIterator& it) :
  m_accessItr(it)
{}


EnhancedAnalysisDictionarySuperWordIterator::EnhancedAnalysisDictionarySuperWordIterator(const EnhancedAnalysisDictionarySuperWordIterator& source) :
  AbstractDictionarySuperWordIterator(),
  m_accessItr(source.m_accessItr)
{}

EnhancedAnalysisDictionarySuperWordIterator::~EnhancedAnalysisDictionarySuperWordIterator()
{}


}

}

}
