// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
