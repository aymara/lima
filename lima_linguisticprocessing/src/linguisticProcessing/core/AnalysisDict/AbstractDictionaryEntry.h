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
    DictionaryEntry(AbstractDictionaryEntry* delegate);
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
    AbstractDictionaryEntry* m_delegate;
};

} // AnalysisDict
} // LinguisticProcessing
} // Lima

#include "AbstractDictionaryEntry.tcc"

#endif
