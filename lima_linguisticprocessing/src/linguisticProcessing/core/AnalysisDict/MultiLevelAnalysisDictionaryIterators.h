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
#ifndef LIMA_LINGUISTICPROCESSING_ANALYSISDICT_MULTILEVELNALYSISDICTIONARYITERATOR_H
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

  virtual const std::pair<uint64_t,DictionaryEntry> operator*() const;
  MultiLevelAnalysisDictionarySubWordIterator& operator++(int i);
  bool operator==(const AbstractDictionarySubWordIterator& it) const;
  bool operator!=(const AbstractDictionarySubWordIterator& it) const;

  virtual AbstractDictionarySubWordIterator* clone() const;

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

  virtual const LimaString operator*() const;
  MultiLevelAnalysisDictionarySuperWordIterator& operator++(int i);
  bool operator==(const AbstractDictionarySuperWordIterator& it) const;
  bool operator!=(const AbstractDictionarySuperWordIterator& it) const;
  
  virtual AbstractDictionarySuperWordIterator* clone() const;

private:

  std::vector<Common::AccessSuperWordIterator> m_accessItrs;

};

}

}

}

#endif
