// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
