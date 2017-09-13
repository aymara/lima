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
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_LINGUISTICPROCESSING_ANALYSISDICTENHANCEDANALYSISDICTIONARYITERATOR_H
#define LIMA_LINGUISTICPROCESSING_ANALYSISDICTENHANCEDANALYSISDICTIONARYITERATOR_H

#include "AnalysisDictExport.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "AnalysisDictionaryIterators.h"
#include "EnhancedAnalysisDictionary.h"
#include "EnhancedAnalysisDictionaryEntry.h"
#include "common/misc/AbstractAccessIterators.h"

namespace Lima
{

namespace LinguisticProcessing
{

namespace AnalysisDict
{

/**
@author Benoit Mathieu
*/
class LIMA_ANALYSISDICT_EXPORT EnhancedAnalysisDictionarySubWordIterator  : public AbstractDictionarySubWordIterator
{
public:
  EnhancedAnalysisDictionarySubWordIterator(
    const Common::AccessSubWordIterator& accessItr,
    const EnhancedAnalysisDictionary& dico);

  virtual ~EnhancedAnalysisDictionarySubWordIterator();

  virtual const std::pair<uint64_t,DictionaryEntry> operator*() const override;
  EnhancedAnalysisDictionarySubWordIterator& operator++(int i) override;
  bool operator==(const AbstractDictionarySubWordIterator& it) const override;
  bool operator!=(const AbstractDictionarySubWordIterator& it) const override;

  virtual AbstractDictionarySubWordIterator* clone() const override;

private:
  
  Common::AccessSubWordIterator m_accessItr;
  const EnhancedAnalysisDictionary& m_dico;

};

class LIMA_ANALYSISDICT_EXPORT EnhancedAnalysisDictionarySuperWordIterator : public AbstractDictionarySuperWordIterator
{
public:

  EnhancedAnalysisDictionarySuperWordIterator(
    const Common::AccessSuperWordIterator& i);
  EnhancedAnalysisDictionarySuperWordIterator(const EnhancedAnalysisDictionarySuperWordIterator& source);
  virtual ~EnhancedAnalysisDictionarySuperWordIterator();

  virtual const LimaString operator*() const override;
  EnhancedAnalysisDictionarySuperWordIterator& operator++(int i) override;
  bool operator==(const AbstractDictionarySuperWordIterator& it) const override;
  bool operator!=(const AbstractDictionarySuperWordIterator& it) const override;
  
  virtual AbstractDictionarySuperWordIterator* clone() const override;

private:

  Common::AccessSuperWordIterator m_accessItr;

};

inline EnhancedAnalysisDictionarySubWordIterator& EnhancedAnalysisDictionarySubWordIterator::operator++(int)
{
  m_accessItr++;
  return *this;
}

inline bool EnhancedAnalysisDictionarySubWordIterator::operator==(const AbstractDictionarySubWordIterator& abit) const
{
  const EnhancedAnalysisDictionarySubWordIterator& it=static_cast<const EnhancedAnalysisDictionarySubWordIterator&>(abit);
  return m_accessItr==it.m_accessItr;
}

inline bool EnhancedAnalysisDictionarySubWordIterator::operator!=(const AbstractDictionarySubWordIterator& abit) const
{
  const EnhancedAnalysisDictionarySubWordIterator& it=static_cast<const EnhancedAnalysisDictionarySubWordIterator&>(abit);
  return m_accessItr!=it.m_accessItr;
}

inline EnhancedAnalysisDictionarySuperWordIterator& EnhancedAnalysisDictionarySuperWordIterator::operator++(int i)
{
  m_accessItr.operator++(i);
  return *this;
}

inline bool EnhancedAnalysisDictionarySuperWordIterator::operator==(const AbstractDictionarySuperWordIterator& abit) const
{
  const EnhancedAnalysisDictionarySuperWordIterator& it=static_cast<const EnhancedAnalysisDictionarySuperWordIterator&>(abit);
  return m_accessItr.operator==(it.m_accessItr);
}

inline bool EnhancedAnalysisDictionarySuperWordIterator::operator!=(const AbstractDictionarySuperWordIterator& abit) const
{
  const EnhancedAnalysisDictionarySuperWordIterator& it=static_cast<const EnhancedAnalysisDictionarySuperWordIterator&>(abit);
  return m_accessItr.operator!=(it.m_accessItr);
}

inline AbstractDictionarySuperWordIterator* EnhancedAnalysisDictionarySuperWordIterator::clone() const
{
  return new EnhancedAnalysisDictionarySuperWordIterator(*this);
}

inline const LimaString EnhancedAnalysisDictionarySuperWordIterator::operator*() const
{
  return *m_accessItr;
}


}

}

}

#endif
