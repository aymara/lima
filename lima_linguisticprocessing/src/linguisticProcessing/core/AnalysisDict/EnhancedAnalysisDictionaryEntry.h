// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
    std::shared_ptr<Lima::Common::AbstractAccessByString> access,
    Lima::FsaStringsPool* sp
  );
  EnhancedAnalysisDictionaryEntry(const EnhancedAnalysisDictionaryEntry& eade);
  virtual ~EnhancedAnalysisDictionaryEntry();

  virtual AbstractDictionaryEntry* clone() override;
  virtual void parseLingInfos(AbstractDictionaryEntryHandler* handler) const override;
  virtual void parseConcatenated(AbstractDictionaryEntryHandler* handler) const override;
  virtual void parseAccentedForms(AbstractDictionaryEntryHandler* handler) const override;

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
    std::shared_ptr<Lima::Common::AbstractAccessByString> access,
    Lima::FsaStringsPool* sp) :
    m_access(access),
    m_sp(sp) {}
    
  virtual ~NotMainKeysDictionaryEntryHandler() {}
  
  void setDelegate(AbstractDictionaryEntryHandler* delegate);
    
  void startEntry(StringsPoolIndex form) override;
  void endEntry() override;

  void foundLingInfos(StringsPoolIndex lemma,StringsPoolIndex norm) override;
  void deleteLingInfos(StringsPoolIndex lemma,StringsPoolIndex norm) override;
  void endLingInfos() override;
  
  void foundConcatenated() override;
  void deleteConcatenated() override;
  void foundComponent(uint64_t position, uint64_t length,StringsPoolIndex form) override;
  void endComponent() override;
  void endConcatenated() override;

  void foundAccentedForm(StringsPoolIndex form) override;
  void deleteAccentedForm(StringsPoolIndex form) override;
  void endAccentedForm() override;
  
  void foundProperties(LinguisticCode lings) override;
  
private:
  std::shared_ptr<Lima::Common::AbstractAccessByString> m_access;
  Lima::FsaStringsPool* m_sp;
  AbstractDictionaryEntryHandler* m_delegate;
};

inline void NotMainKeysDictionaryEntryHandler::setDelegate(AbstractDictionaryEntryHandler* delegate)
{
  m_delegate=delegate;
}

inline void NotMainKeysDictionaryEntryHandler::startEntry(StringsPoolIndex form)
{
  m_delegate->startEntry((*m_sp)[m_access->getSpelling(form)]);
// This hack fix bug 1630 but introduce regression in HyperwordStemmer
//  m_delegate->startEntry(form);
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
  m_delegate->foundComponent(position,length,(*m_sp)[m_access->getSpelling(form)]);
// This hack fix bug 1630 but introduce regression in HyperwordStemmer
//  m_delegate->foundComponent(position,length,form);
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
  m_delegate->foundAccentedForm((*m_sp)[m_access->getSpelling(form)]);
// This hack fix bug 1630 but introduce regression in HyperwordStemmer
//  m_delegate->foundAccentedForm(form);
}

inline void NotMainKeysDictionaryEntryHandler::deleteAccentedForm(StringsPoolIndex form)
{
  m_delegate->deleteAccentedForm((*m_sp)[m_access->getSpelling(form)]);
// This hack fix bug 1630 but introduce regression in HyperwordStemmer
//  m_delegate->deleteAccentedForm(form);
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
