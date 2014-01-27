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
