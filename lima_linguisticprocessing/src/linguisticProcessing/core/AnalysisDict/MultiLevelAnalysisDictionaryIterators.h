// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_LINGUISTICPROCESSING_ANALYSISDICT_MULTILEVELANALYSISDICTIONARYITERATOR_H
#define LIMA_LINGUISTICPROCESSING_ANALYSISDICT_MULTILEVELANALYSISDICTIONARYITERATOR_H

#include "AnalysisDictExport.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "AnalysisDictionaryIterators.h"
#include "MultiLevelAnalysisDictionary.h"
#include "MultiLevelAnalysisDictionaryEntry.h"
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
class LIMA_ANALYSISDICT_EXPORT MultiLevelAnalysisDictionarySubWordIterator  : public AbstractDictionarySubWordIterator
{
public:
  MultiLevelAnalysisDictionarySubWordIterator(
    const std::vector<std::pair<Common::AccessSubWordIterator,Common::AccessSubWordIterator> >& accessItr,
    const MultiLevelAnalysisDictionary& dico);

  virtual ~MultiLevelAnalysisDictionarySubWordIterator();

  virtual const std::pair<uint64_t,DictionaryEntry> operator*() const override;
  MultiLevelAnalysisDictionarySubWordIterator& operator++(int i) override;
  bool operator==(const AbstractDictionarySubWordIterator& it) const override;
  bool operator!=(const AbstractDictionarySubWordIterator& it) const override;

  virtual AbstractDictionarySubWordIterator* clone() const override;

private:

  // ce vector doit contenir un iterator par niveau dans le dictionnaire
  std::vector<std::pair<Common::AccessSubWordIterator,Common::AccessSubWordIterator> > m_accessItrs;
  const MultiLevelAnalysisDictionary& m_dico;
};

class LIMA_ANALYSISDICT_EXPORT MultiLevelAnalysisDictionarySuperWordIterator : public AbstractDictionarySuperWordIterator
{
public:

  MultiLevelAnalysisDictionarySuperWordIterator(
    const std::vector<Common::AccessSuperWordIterator>& i);
  MultiLevelAnalysisDictionarySuperWordIterator(const MultiLevelAnalysisDictionarySuperWordIterator& source);
  virtual ~MultiLevelAnalysisDictionarySuperWordIterator();

  virtual const LimaString operator*() const override;
  MultiLevelAnalysisDictionarySuperWordIterator& operator++(int i) override;
  bool operator==(const AbstractDictionarySuperWordIterator& it) const override;
  bool operator!=(const AbstractDictionarySuperWordIterator& it) const override;
  
  virtual AbstractDictionarySuperWordIterator* clone() const override;

private:

  std::vector<Common::AccessSuperWordIterator> m_accessItrs;

};

}

}

}

#endif
