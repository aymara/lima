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
#ifndef LIMA_LINGUISTICPROCESSING_ANALYSISDICTMULTILEVELANALYSISDICTIONARYENTRY_H
#define LIMA_LINGUISTICPROCESSING_ANALYSISDICTMULTILEVELANALYSISDICTIONARYENTRY_H

#include "AnalysisDictExport.h"
#include "AbstractDictionaryEntry.h"
#include "DictionaryData.h"

namespace Lima
{

namespace LinguisticProcessing
{

namespace AnalysisDict
{

class MultiLevelAnalysisDictionaryEntryPrivate;
/**
@author Benoit Mathieu
*/
class LIMA_ANALYSISDICT_EXPORT MultiLevelAnalysisDictionaryEntry : public AbstractDictionaryEntry
{
  friend class MultiLevelAnalysisDictionaryEntryPrivate;
public:

  struct LevelData
  {
    LevelData() : startEntryData(0),endEntryData(0),keys(0),dicoData(0),mainKeys(false) {};
    unsigned char* startEntryData;
    unsigned char* endEntryData;
    const Lima::Common::AbstractAccessByString* keys;
    const DictionaryData* dicoData;
    bool mainKeys;
  };

  MultiLevelAnalysisDictionaryEntry(
    StringsPoolIndex formId,
    bool isFinal,
    bool isEmpty,
    bool hasLingInfos,
    bool hasConcatenated,
    bool hasAccentedForm,
    const std::vector<LevelData>& data,
    Lima::FsaStringsPool* sp);


  virtual ~MultiLevelAnalysisDictionaryEntry() = default;

  MultiLevelAnalysisDictionaryEntry(const MultiLevelAnalysisDictionaryEntry&);
  MultiLevelAnalysisDictionaryEntry& operator=(const MultiLevelAnalysisDictionaryEntry&);

  virtual AbstractDictionaryEntry* clone() override;
  virtual void parseAccentedForms(AbstractDictionaryEntryHandler* handler) const override;
  virtual void parseConcatenated(AbstractDictionaryEntryHandler* handler) const override;
  virtual void parseLingInfos(AbstractDictionaryEntryHandler* handler) const override;


private:
  MultiLevelAnalysisDictionaryEntryPrivate* m_d;
};

}

}

}

#endif
