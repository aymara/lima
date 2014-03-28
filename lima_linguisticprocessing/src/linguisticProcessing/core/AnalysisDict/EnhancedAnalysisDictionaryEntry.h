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
#ifndef LIMA_LINGUISTICPROCESSING_ANALYSISDICTENHANCEDANALYSISDICTIONARYENTRY_H
#define LIMA_LINGUISTICPROCESSING_ANALYSISDICTENHANCEDANALYSISDICTIONARYENTRY_H

#include "AnalysisDictExport.h"
#include "DictionaryData.h"
#include "AbstractDictionaryEntry.h"
#include "AbstractDictionaryEntryHandler.h"

namespace Lima
{

namespace LinguisticProcessing
{

namespace AnalysisDict
{

// forward declaration
class NotMainKeysDictionaryEntryHandler;

/**
@author Benoit Mathieu
*/
class LIMA_ANALYSISDICT_EXPORT EnhancedAnalysisDictionaryEntry : public AbstractDictionaryEntry
{
public:
  EnhancedAnalysisDictionaryEntry(
    StringsPoolIndex formId,
    bool isFinal,
    bool isEmpty,
    bool hasLingInfos,
    bool hasConcatenated,
    bool hasAccentedForm,
    unsigned char* startEntryData,
    unsigned char* endEntryData,
    const DictionaryData* dicoData,
    bool isMainKeys,
    Lima::Common::AbstractAccessByString* access,
    Lima::FsaStringsPool* sp
  );
  EnhancedAnalysisDictionaryEntry(const EnhancedAnalysisDictionaryEntry& eade);
  virtual ~EnhancedAnalysisDictionaryEntry();

  virtual AbstractDictionaryEntry* clone();
  virtual void parseLingInfos(AbstractDictionaryEntryHandler* handler) const;
  virtual void parseConcatenated(AbstractDictionaryEntryHandler* handler) const;
  virtual void parseAccentedForms(AbstractDictionaryEntryHandler* handler) const;

private:
  static void parseLingInfos(unsigned char* startEntry,unsigned char* endEntry,const DictionaryData* dicoData,AbstractDictionaryEntryHandler* handler);
  static void parseConcatenated(unsigned char* startEntry,unsigned char* endEntry,const DictionaryData* dicoData,AbstractDictionaryEntryHandler* handler);

  unsigned char* m_startEntryData;
  unsigned char* m_endEntryData;
  const DictionaryData* m_dicoData;
  NotMainKeysDictionaryEntryHandler* m_notMainKeysHandler;
  Lima::FsaStringsPool* m_stringsPool;
};

inline AbstractDictionaryEntry* EnhancedAnalysisDictionaryEntry::clone()
{
  return new EnhancedAnalysisDictionaryEntry(*this);
}

/**
*  DictionaryEntryHandler used to convert dico ids into stringPool ids
*  when dictionary keys are different from stringPool ones.
*/
class LIMA_ANALYSISDICT_EXPORT NotMainKeysDictionaryEntryHandler : public AbstractDictionaryEntryHandler
{
public:

  NotMainKeysDictionaryEntryHandler(
    Lima::Common::AbstractAccessByString* access,
    Lima::FsaStringsPool* sp) :
    m_access(access),
    m_sp(sp) {}
    
  virtual ~NotMainKeysDictionaryEntryHandler() {}
  
  void setDelegate(AbstractDictionaryEntryHandler* delegate);
    
  void startEntry(StringsPoolIndex form);
  void endEntry();

  void foundLingInfos(StringsPoolIndex lemma,StringsPoolIndex norm);
  void deleteLingInfos(StringsPoolIndex lemma,StringsPoolIndex norm);
  void endLingInfos();
  
  void foundConcatenated();
  void deleteConcatenated();
  void foundComponent(uint64_t position, uint64_t length,StringsPoolIndex form);
  void endComponent();
  void endConcatenated();

  void foundAccentedForm(StringsPoolIndex form);
  void deleteAccentedForm(StringsPoolIndex form);
  void endAccentedForm();
  
  void foundProperties(LinguisticCode lings);
  
private:
  Lima::Common::AbstractAccessByString* m_access;
  Lima::FsaStringsPool* m_sp;
  AbstractDictionaryEntryHandler* m_delegate;
};

inline void NotMainKeysDictionaryEntryHandler::setDelegate(AbstractDictionaryEntryHandler* delegate)
{
  m_delegate=delegate;
}

inline void NotMainKeysDictionaryEntryHandler::startEntry(StringsPoolIndex form)
{
//  m_delegate->startEntry((*m_sp)[m_access->getSpelling(form)]);
  m_delegate->startEntry(form);
}

inline void NotMainKeysDictionaryEntryHandler::endEntry()
{
  m_delegate->endEntry();
}


inline void NotMainKeysDictionaryEntryHandler::endLingInfos()
{
  m_delegate->endLingInfos();
}
  
inline void NotMainKeysDictionaryEntryHandler::foundConcatenated()
{
  m_delegate->foundConcatenated();
}

inline void NotMainKeysDictionaryEntryHandler::deleteConcatenated()
{
  m_delegate->deleteConcatenated();
}

inline void NotMainKeysDictionaryEntryHandler::foundComponent(uint64_t position, uint64_t length,StringsPoolIndex form)
{
//  m_delegate->foundComponent(position,length,(*m_sp)[m_access->getSpelling(form)]);
  m_delegate->foundComponent(position,length,form);
}

inline void NotMainKeysDictionaryEntryHandler::endComponent()
{
  m_delegate->endComponent();
}

inline void NotMainKeysDictionaryEntryHandler::endConcatenated()
{
  m_delegate->endConcatenated();
}

inline void NotMainKeysDictionaryEntryHandler::foundAccentedForm(StringsPoolIndex form)
{
//  m_delegate->foundAccentedForm((*m_sp)[m_access->getSpelling(form)]);
  m_delegate->foundAccentedForm(form);
}

inline void NotMainKeysDictionaryEntryHandler::deleteAccentedForm(StringsPoolIndex form)
{
//  m_delegate->deleteAccentedForm((*m_sp)[m_access->getSpelling(form)]);
  m_delegate->deleteAccentedForm(form);
}

inline void NotMainKeysDictionaryEntryHandler::endAccentedForm()
{
  m_delegate->endAccentedForm();
}
  
inline void NotMainKeysDictionaryEntryHandler::foundProperties(LinguisticCode lings)
{
  m_delegate->foundProperties(lings);
}


}

}

}

#endif
