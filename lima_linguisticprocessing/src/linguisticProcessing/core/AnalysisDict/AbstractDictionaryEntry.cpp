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

#include "AbstractDictionaryEntry.h"

namespace Lima
{
namespace LinguisticProcessing
{
namespace AnalysisDict
{

AbstractDictionaryEntry::AbstractDictionaryEntry(
  StringsPoolIndex entryId,
  bool isFinal,
  bool isEmpty,
  bool hasLingInfos,
  bool hasConcatenated,
  bool hasAccentedForm) :
  m_entryId(entryId),
  m_isFinal(isFinal),
  m_isEmpty(isEmpty),
  m_hasLingInfos(hasLingInfos),
  m_hasConcatenated(hasConcatenated),
  m_hasAccentedForms(hasAccentedForm)
{}

/*
AbstractDictionaryEntry(const AbstractDictionaryEntry& de) :
  m_entryId(de.entryId),
  m_isFinal(de.isFinal),
  m_isEmpty(de.isEmpty),
  m_hasLingInfos(de.hasLingInfos),
  m_hasConcatenated(de.hasConcatenated),
  m_hasAccentedForms(de.hasAccentedForm)
{}
*/
AbstractDictionaryEntry::~AbstractDictionaryEntry()
{}

DictionaryEntry::DictionaryEntry(AbstractDictionaryEntry* delegate) :
  m_delegate(delegate)
{}

DictionaryEntry::DictionaryEntry(const DictionaryEntry& dicoEntry) :
  m_delegate(0)
{
  m_delegate=dicoEntry.m_delegate->clone();
}
  
DictionaryEntry::~DictionaryEntry()
{
  delete m_delegate;
}

DictionaryEntry& DictionaryEntry::operator=(const DictionaryEntry& dicoEntry)
{
  delete m_delegate;
  m_delegate=dicoEntry.m_delegate->clone();
  return *this;
}

} // AnalysisDict
} // LinguisticProcessing
} // Lima
