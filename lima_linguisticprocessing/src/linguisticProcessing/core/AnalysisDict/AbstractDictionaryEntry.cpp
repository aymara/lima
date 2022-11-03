// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
