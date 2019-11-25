/*
    Copyright 2002-2019 CEA LIST

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
#include "EnhancedAnalysisDictionaryEntry.h"

#include <iostream>
#include <cassert>
#include "linguisticProcessing/LinguisticProcessingCommon.h"

namespace Lima
{

namespace LinguisticProcessing
{

namespace AnalysisDict
{

EnhancedAnalysisDictionaryEntry::EnhancedAnalysisDictionaryEntry(
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
  Lima::FsaStringsPool* sp) :
    AbstractDictionaryEntry(formId,isFinal,isEmpty,hasLingInfos,
                            hasConcatenated,hasAccentedForm),
    m_startEntryData(startEntryData),
    m_endEntryData(endEntryData),
    m_dicoData(dicoData),
    m_notMainKeysHandler(0),
    m_stringsPool(sp)
{
  ANALYSISDICTLOGINIT;
  LDEBUG << "EnhancedAnalysisDictionaryEntry::EnhancedAnalysisDictionaryEntry"
          << m_stringsPool;
  if (!isMainKeys) {
    m_notMainKeysHandler=new NotMainKeysDictionaryEntryHandler(access,sp);
  }
}

EnhancedAnalysisDictionaryEntry::EnhancedAnalysisDictionaryEntry(
    const EnhancedAnalysisDictionaryEntry& eade) :
  AbstractDictionaryEntry(eade),
  m_startEntryData(eade.m_startEntryData),
  m_endEntryData(eade.m_endEntryData),
  m_dicoData(eade.m_dicoData),
  m_notMainKeysHandler(0)
{
  if (eade.m_notMainKeysHandler) {
    m_notMainKeysHandler=new NotMainKeysDictionaryEntryHandler(*(eade.m_notMainKeysHandler));
  }
}

EnhancedAnalysisDictionaryEntry::~EnhancedAnalysisDictionaryEntry()
{
  if (m_notMainKeysHandler) {
    delete m_notMainKeysHandler;
    m_notMainKeysHandler=0;
  }
}

void EnhancedAnalysisDictionaryEntry::parseLingInfos(
    AbstractDictionaryEntryHandler* targetHandler) const
{
  auto handler = targetHandler;
  if (m_notMainKeysHandler)
  {
    m_notMainKeysHandler->setDelegate(targetHandler);
    handler = m_notMainKeysHandler;
  }
  handler->startEntry(m_entryId);
  parseLingInfos(m_startEntryData, m_endEntryData, m_dicoData, handler);
  handler->endEntry();
}

void EnhancedAnalysisDictionaryEntry::parseConcatenated(
    AbstractDictionaryEntryHandler* targetHandler) const
{
  auto handler = targetHandler;
  if (m_notMainKeysHandler) {
    m_notMainKeysHandler->setDelegate(targetHandler);
    handler = m_notMainKeysHandler;
  }
  handler->startEntry(m_entryId);
  parseConcatenated(m_startEntryData,m_endEntryData,m_dicoData,handler);
  handler->endEntry();
}

void EnhancedAnalysisDictionaryEntry::parseAccentedForms(
    AbstractDictionaryEntryHandler* targetHandler) const
{
#ifdef DEBUG_LP
  ANALYSISDICTLOGINIT;
  LDEBUG << "EnhancedAnalysisDictionaryEntry::parseAccentedForms";
#endif
  auto handler = targetHandler;
  if (m_notMainKeysHandler) {
    m_notMainKeysHandler->setDelegate(targetHandler);
    handler = m_notMainKeysHandler;
  }
  handler->startEntry(m_entryId);
  unsigned char* p=m_startEntryData;
  assert(p != m_endEntryData);
  // skip ling infos
  auto read = static_cast<StringsPoolIndex>(DictionaryData::readCodedInt(p));
  p+=read;
  if (p != m_endEntryData)
  {
    // parse accented data
    read = static_cast<StringsPoolIndex>(DictionaryData::readCodedInt(p));
    unsigned char* end=p+read;
    while (p!=end)
    {
      read = static_cast<StringsPoolIndex>(DictionaryData::readCodedInt(p));
      if (read == 0)
      {
        read = static_cast<StringsPoolIndex>(DictionaryData::readCodedInt(p));
        handler->deleteAccentedForm(read);
      }
      else
      {
#ifdef DEBUG_LP
        LDEBUG << "  found accented form:" << (*m_stringsPool)[read];
#endif
        handler->foundAccentedForm(read);
        // parse accented form
        unsigned char* acc=m_dicoData->getEntryAddr(read);
        uint64_t tmp=DictionaryData::readCodedInt(acc);
        if (tmp == 1)
        {
          ANALYSISDICTLOGINIT;
          LWARN << "WARNING ! should never accentuate to a delete entry !";
          tmp=DictionaryData::readCodedInt(acc);
        }
        // tmp contains length
        if (tmp == 0)
        {
          ANALYSISDICTLOGINIT
          LWARN << "WARNING ! should never accentuate to an empty entry !";
        }
        parseLingInfos(acc,acc+tmp,m_dicoData,handler);
        parseConcatenated(acc,acc+tmp,m_dicoData,handler);
        handler->endAccentedForm();
      }
    }
  }
  handler->endEntry();
}

void EnhancedAnalysisDictionaryEntry::parseLingInfos(
    unsigned char* startEntry,
    unsigned char* endEntry,
    const DictionaryData* dicoData,
    AbstractDictionaryEntryHandler* handler)
{
  LIMA_UNUSED(endEntry);

#ifdef DEBUG_LP
 ANALYSISDICTLOGINIT;
 LDEBUG << "EnhancedAnalysisDictionaryEntry::parseLingInfos : "
        << (uint64_t)startEntry << " , " << (uint64_t)endEntry;
#endif

  unsigned char* p = startEntry;
  assert(p != endEntry);
  uint64_t read = DictionaryData::readCodedInt(p);
#ifdef DEBUG_LP
  LDEBUG << "read linginfo length = " << read;
#endif
  unsigned char* end = p + read;
#ifdef DEBUG_LP
 LDEBUG << "end = " << (uint64_t)(end);
#endif
  while ( p != end)
  {
#ifdef DEBUG_LP
    LDEBUG << "read linginfo p = " << (uint64_t)p;
#endif
    bool toDelete = false;
    auto lemma = static_cast<StringsPoolIndex>(DictionaryData::readCodedInt(p));

    if (lemma == static_cast<StringsPoolIndex>(0))
    {
#ifdef DEBUG_LP
     LDEBUG << "read delete flag (p =" << (uint64_t)p << ")";
#endif
      toDelete = true;
      lemma = static_cast<StringsPoolIndex>(DictionaryData::readCodedInt(p));
    }
#ifdef DEBUG_LP
   LDEBUG << "read lemma " << lemma << " (p =" << (uint64_t)p << ")";
#endif
    auto norm = static_cast<StringsPoolIndex>(DictionaryData::readCodedInt(p));
    if (norm == static_cast<StringsPoolIndex>(0))
    {
      norm = lemma;
    }
#ifdef DEBUG_LP
   LDEBUG << "read norm " << norm << " (p =" << (uint64_t)p << ")" ;
#endif
    if (toDelete)
    {
      handler->deleteLingInfos(lemma, norm);
    }
    uint64_t lingOffset = DictionaryData::readCodedInt(p);
#ifdef DEBUG_LP
   LDEBUG << "read lingOffset = " << lingOffset << " (p =" << (uint64_t)p << ")";
#endif
    // lingOffset=0 means there is no ling properties
    if (lingOffset != 0)
    {
      handler->foundLingInfos(lemma, norm);
      auto props = dicoData->getLingPropertiesAddr(lingOffset);
      read = DictionaryData::readCodedInt(props);
      auto propsEnd = props + read;
      while (props != propsEnd)
      {
        handler->foundProperties(
          static_cast<LinguisticCode>(DictionaryData::readCodedInt(props)));
      }
      handler->endLingInfos();
    }
  }
}

void EnhancedAnalysisDictionaryEntry::parseConcatenated(
    unsigned char* startEntry,
    unsigned char* endEntry,
    const DictionaryData* dicoData,
    AbstractDictionaryEntryHandler* handler)
{
//  ANALYSISDICTLOGINIT;
//  LDEBUG << "parse concatenated " << (uint64_t)startEntry << " , "
//          << (uint64_t)endEntry;

  unsigned char* p=startEntry;
  assert(p != endEntry);
  // skip linginfos
  uint64_t read=DictionaryData::readCodedInt(p);
  p+=read;
//  LDEBUG << "skip ling info of length " << read;
  if (p != endEntry)
  {
    // skip accented
    read=DictionaryData::readCodedInt(p);
    p+=read;
//    LDEBUG << "skip accented of length " << read;
    if (p != endEntry)
    {
      // read concat
      read=DictionaryData::readCodedInt(p);
      unsigned char* end=p+read;
//      LDEBUG << "read concat of length " << read;
      while (p!=end)
      {
        read=DictionaryData::readCodedInt(p);
        if (read == 0)
        {
          read=DictionaryData::readCodedInt(p);
//          LDEBUG << "has delete info";
          // parse concat to provide delete infos
          handler->deleteConcatenated();
          bool hasInfo=false;
          unsigned char* pp=p;
          uint64_t nb=read;
//          LDEBUG << "has " << nb << " components";
          while (nb-- > 0)
          {
            auto str = static_cast<StringsPoolIndex>(DictionaryData::readCodedInt(pp));
//            LDEBUG << "read str=" << str;
            uint64_t pos=DictionaryData::readCodedInt(pp);
//            LDEBUG << "read pos=" << pos;
            uint64_t len=DictionaryData::readCodedInt(pp);
//            LDEBUG << "read len=" << len;
            handler->foundComponent(pos,len,str);
            uint64_t lilength=DictionaryData::readCodedInt(pp);
//            LDEBUG << "lingInfo length = " << lilength;
            unsigned char* pp_end=pp+lilength;
            while (pp != pp_end)
            {
              hasInfo=true;
              DictionaryData::readCodedInt(pp); // read lemma
              DictionaryData::readCodedInt(pp); // read norm
              DictionaryData::readCodedInt(pp); // read props
            }
          }
          handler->endConcatenated();
          if (!hasInfo)
          {
            p=pp;
            continue;
          }
        }
        uint64_t nbComponents=read;
//        LDEBUG << "has " << nbComponents << " components";
        // parse concat infos
        handler->foundConcatenated();
        while (nbComponents-- > 0)
        {
          auto str = static_cast<StringsPoolIndex>(DictionaryData::readCodedInt(p));
//          LDEBUG << "read string " << str;
          uint64_t pos=DictionaryData::readCodedInt(p);
//          LDEBUG << "read pos=" << pos;
          uint64_t len=DictionaryData::readCodedInt(p);
//          LDEBUG << "read len=" << len;
          uint64_t lilength=DictionaryData::readCodedInt(p);
//          LDEBUG << "read LIlength=" << lilength;
          handler->foundComponent(pos,len,str);
          unsigned char* liend=p+lilength;
          while (p != liend)
          {
            auto lemma = static_cast<StringsPoolIndex>(DictionaryData::readCodedInt(p)); // read lemma
//            LDEBUG << "read lemma=" << lemma;
            auto norm =static_cast<StringsPoolIndex>(DictionaryData::readCodedInt(p)); // read norm
//            LDEBUG << "read norm=" << norm;
            if (norm==0)
            {
              norm=lemma;
            }
            handler->foundLingInfos(lemma,norm);
            uint64_t lingOffset=DictionaryData::readCodedInt(p); // read props
//            LDEBUG << "read ling offset = " << lingOffset;
            unsigned char* props = dicoData->getLingPropertiesAddr(lingOffset);
            read = DictionaryData::readCodedInt(props);
//            LDEBUG << "lingprops length = " << read;
            unsigned char* propsEnd = props + read;
            while (props!=propsEnd)
            {
              handler->foundProperties(
                static_cast<LinguisticCode>(DictionaryData::readCodedInt(props)));
            }
            handler->endLingInfos();
          }
          handler->endComponent();
        }
        handler->endConcatenated();
      }
    }
  }
}


void NotMainKeysDictionaryEntryHandler::foundLingInfos(StringsPoolIndex lemma,StringsPoolIndex norm)
{
  StringsPoolIndex splemma=(*m_sp)[m_access->getSpelling(lemma)];
  StringsPoolIndex spnorm=splemma;
  if (norm != lemma) {
    spnorm=(*m_sp)[m_access->getSpelling(norm)];
  }
  m_delegate->foundLingInfos(splemma,spnorm);
}

void NotMainKeysDictionaryEntryHandler::deleteLingInfos(StringsPoolIndex lemma,
                                                        StringsPoolIndex norm)
{
  auto splemma=(*m_sp)[m_access->getSpelling(lemma)];
  auto spnorm=splemma;
  if (norm != lemma)
  {
    spnorm=(*m_sp)[m_access->getSpelling(norm)];
  }
  m_delegate->deleteLingInfos(splemma,spnorm);
}

}

}

}
