// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
  const std::pair<uint64_t,uint64_t> pos = *m_accessItr; // clazy:exclude=rule-of-two-soft
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
