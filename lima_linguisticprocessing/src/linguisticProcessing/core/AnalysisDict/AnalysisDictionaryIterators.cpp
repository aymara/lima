// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                              *
 *                                                                         *
 ***************************************************************************/

#include "AnalysisDictionaryIterators.h"

namespace Lima
{
namespace LinguisticProcessing
{
namespace AnalysisDict
{

DictionarySubWordIterator::DictionarySubWordIterator(AbstractDictionarySubWordIterator* delegate) :
  m_delegate(delegate)
{}

DictionarySubWordIterator::DictionarySubWordIterator(const DictionarySubWordIterator& source)
{
  m_delegate=source.m_delegate->clone();
}
  
DictionarySubWordIterator::~DictionarySubWordIterator()
{
  delete m_delegate;
}

DictionarySubWordIterator& DictionarySubWordIterator::operator=(const DictionarySubWordIterator& source)
{
  delete m_delegate;
  m_delegate=source.m_delegate->clone();
  return *this;
}

DictionarySuperWordIterator::DictionarySuperWordIterator(AbstractDictionarySuperWordIterator* delegate) :
  m_delegate(delegate)
{}

DictionarySuperWordIterator::DictionarySuperWordIterator(const DictionarySuperWordIterator& source)
{
  m_delegate=source.m_delegate->clone();
}

DictionarySuperWordIterator::~DictionarySuperWordIterator()
{
  delete m_delegate;
}

DictionarySuperWordIterator& DictionarySuperWordIterator::operator=(const DictionarySuperWordIterator& source)
{
  delete m_delegate;
  m_delegate=source.m_delegate->clone();
  return *this;
}

} // namespace AnalysisDict
} // namespace LinguisticProcessing
} // namespace Lima
