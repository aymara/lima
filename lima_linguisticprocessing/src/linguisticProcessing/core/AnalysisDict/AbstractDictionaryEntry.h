// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/

#ifndef LIMA_LINGUISTICPROCESSING_ANALYSISDICT_ABSTRACTDICTIONARYENTRY_H
#define LIMA_LINGUISTICPROCESSING_ANALYSISDICT_ABSTRACTDICTIONARYENTRY_H

#include "AnalysisDictExport.h"
#include "AbstractDictionaryEntryHandler.h"
#include "common/misc/fsaStringsPool.h"
 
namespace Lima {
namespace LinguisticProcessing {
namespace AnalysisDict {

class LIMA_ANALYSISDICT_EXPORT AbstractDictionaryEntry
{
  public:
    AbstractDictionaryEntry(
      StringsPoolIndex entryId,
      bool isFinal,
      bool isEmpty,
      bool hasLingInfos,
      bool hasConcatenated,
      bool hasAccentedForm);
      
//    AbstractDictionaryEntry(const AbstractDictionaryEntry& de);
      
    virtual ~AbstractDictionaryEntry();
    
    virtual AbstractDictionaryEntry* clone() = 0;
  
    inline StringsPoolIndex getEntryId() const;
    inline bool isFinal() const;
    inline bool isEmpty() const;
    inline bool hasLingInfos() const;
    virtual void parseLingInfos(AbstractDictionaryEntryHandler* handler) const = 0;
    inline bool hasConcatenated() const;
    virtual void parseConcatenated(AbstractDictionaryEntryHandler* handler) const = 0;
    inline bool hasAccentedForms() const;
    virtual void parseAccentedForms(AbstractDictionaryEntryHandler* handler) const = 0;
    
  protected:
    
    StringsPoolIndex m_entryId;
    bool m_isFinal;
    bool m_isEmpty;
    bool m_hasLingInfos;
    bool m_hasConcatenated;
    bool m_hasAccentedForms;

};

class LIMA_ANALYSISDICT_EXPORT DictionaryEntry
{
  public:
    DictionaryEntry(std::shared_ptr<AbstractDictionaryEntry> delegate);
    DictionaryEntry(const DictionaryEntry& dicoEntry);
    virtual ~DictionaryEntry();
    DictionaryEntry& operator=(const DictionaryEntry& dicoEntry);
  
    inline StringsPoolIndex getEntryId() const;
    inline bool isFinal() const;
    inline bool isEmpty() const;
    inline bool hasLingInfos() const;
    inline void parseLingInfos(AbstractDictionaryEntryHandler* handler) const;
    inline bool hasConcatenated() const;
    inline void parseConcatenated(AbstractDictionaryEntryHandler* handler) const;
    inline bool hasAccentedForms() const;
    inline void parseAccentedForms(AbstractDictionaryEntryHandler* handler) const;
    
  private:
    std::shared_ptr<AbstractDictionaryEntry> m_delegate;
};

} // AnalysisDict
} // LinguisticProcessing
} // Lima

#include "AbstractDictionaryEntry.tcc"

#endif
