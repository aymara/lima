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

/**
@author Benoit Mathieu
*/
class LIMA_ANALYSISDICT_EXPORT MultiLevelAnalysisDictionaryEntry : public AbstractDictionaryEntry
{
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


  virtual ~MultiLevelAnalysisDictionaryEntry();

  virtual AbstractDictionaryEntry* clone();
  virtual void parseAccentedForms(AbstractDictionaryEntryHandler* handler) const;
  virtual void parseConcatenated(AbstractDictionaryEntryHandler* handler) const;
  virtual void parseLingInfos(AbstractDictionaryEntryHandler* handler) const;


private:
  std::vector<LevelData> m_data;
  Lima::FsaStringsPool* m_sp;

  class LingInfoLevelState
  {
  public:
  
    // position attributes
    unsigned char* pos;
    unsigned char* posEnd;

    // state attributes
    StringsPoolIndex currentLemma;
    LimaString lemmaStr;
    StringsPoolIndex currentNorm;
    LimaString normStr;
    uint64_t lingInfoOffset;
    bool final;

    // data attributes
    bool mainKeys;
    const Lima::Common::AbstractAccessByString* keys;
    const DictionaryData* dicoData;

    LingInfoLevelState();
    virtual ~LingInfoLevelState();
   
    void next(FsaStringsPool& sp);
    bool end() const;
    bool operator<(const LingInfoLevelState& lis) const;
    bool operator==(const LingInfoLevelState& lis) const;

  };
  
  class ConcatenatedLevelState
  {
  public:

    class Component
    {
    public:
      StringsPoolIndex form;
      LimaString formStr;
      uint64_t pos;
      uint64_t len;
      LingInfoLevelState liState;
      Component();
      virtual ~Component();
      bool operator<(const Component& c) const { return formStr<c.formStr;};
      bool operator==(const Component& c) const { return form==c.form;};
    };

    // position attributes
    unsigned char* pos;
    unsigned char* posEnd;

    // state attributes
    bool final;
    std::vector<Component> components;

    // data attributes
    bool mainKeys;
    const Lima::Common::AbstractAccessByString* keys;
    const DictionaryData* dicoData;

    ConcatenatedLevelState();
    virtual ~ConcatenatedLevelState();
    bool operator<(const ConcatenatedLevelState& cls) const { return (cls.components.empty() || ( !components.empty() && (components < cls.components) ) ); };
    bool operator==(const ConcatenatedLevelState& cls) const { return (components == cls.components); };

    void next(FsaStringsPool& sp);
    bool end() const;

  };

  class AccentedLevelState
  {
  public:

    // position attributes
    unsigned char* pos;
    unsigned char* posEnd;

    // state attributes
    StringsPoolIndex accentedForm;
    LimaString accentedFormStr;
    StringsPoolIndex accentedEntry;
    bool final;

    // data attributes
    bool mainKeys;
    const Lima::Common::AbstractAccessByString* keys;
    const DictionaryData* dicoData;

    void next(FsaStringsPool& sp);
    bool end() const;
    bool operator<(const AccentedLevelState& lis) const;
    bool operator==(const AccentedLevelState& lis) const;
  };
  
  
  static void parseLingInfos(std::vector<MultiLevelAnalysisDictionaryEntry::LingInfoLevelState>& data,Lima::FsaStringsPool* sp,AbstractDictionaryEntryHandler* handler);
  static void parseConcatenated(std::vector<MultiLevelAnalysisDictionaryEntry::ConcatenatedLevelState>& data,Lima::FsaStringsPool* sp,AbstractDictionaryEntryHandler* handler);

};

}

}

}

#endif
