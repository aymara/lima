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
 *   Copyright (C) 2004-2012 by CEA LIST                              *
 *                                                                         *
 ***************************************************************************/
#include "MultiLevelAnalysisDictionaryEntry.h"

#include "linguisticProcessing/core/FlatTokenizer/SpiritCharChartParser.hpp"
#include "linguisticProcessing/LinguisticProcessingCommon.h"

#include "common/Data/strwstrtools.h"

#include <vector>
#include <limits>
#include <iostream>
#include <cassert>
#include <set>

using namespace std;

namespace Lima
{

namespace LinguisticProcessing
{

namespace AnalysisDict
{

MultiLevelAnalysisDictionaryEntry::MultiLevelAnalysisDictionaryEntry(
  StringsPoolIndex formId,
  bool isFinal,
  bool isEmpty,
  bool hasLingInfos,
  bool hasConcatenated,
  bool hasAccentedForm,
  const std::vector<LevelData>& data,
  Lima::FsaStringsPool* sp
) :
    AbstractDictionaryEntry(formId,isFinal,isEmpty,hasLingInfos,hasConcatenated,hasAccentedForm),
    m_data(data),
    m_sp(sp)
{}

MultiLevelAnalysisDictionaryEntry::~MultiLevelAnalysisDictionaryEntry()
{}


AbstractDictionaryEntry* MultiLevelAnalysisDictionaryEntry::clone()
{
  return new MultiLevelAnalysisDictionaryEntry(*this);
}

void MultiLevelAnalysisDictionaryEntry::parseAccentedForms(AbstractDictionaryEntryHandler* handler) const
{
  ANALYSISDICTLOGINIT;
  LDEBUG << "MultiLevelAnalysisDictionaryEntry::parseAccentedForms";
  handler->startEntry(m_entryId);

  // initialize states
  vector<AccentedLevelState> state;
  for (vector<LevelData>::const_iterator it=m_data.begin();
       it!=m_data.end();
       it++)
  {
    AccentedLevelState curState;

    // set position attributes
    curState.pos=it->startEntryData;
    assert(curState.pos != it->endEntryData);
    // skip linginfos
    uint64_t read=DictionaryData::readCodedInt(curState.pos);
    curState.pos+=read;
    if (curState.pos != it->endEntryData)
    {
      // read accented
      read=DictionaryData::readCodedInt(curState.pos);
      curState.posEnd= curState.pos + read;

      // set data attributes
      curState.mainKeys=it->mainKeys;
      curState.keys=it->keys;
      curState.dicoData=it->dicoData;

      // set state attributes
      curState.next(*m_sp);

      state.push_back(curState);
    }
  }

  // parse accented forms
  while (true)
  {
    // find lower state
    AccentedLevelState lowerState = state.front();
    bool final=lowerState.final;
    bool hasInfo= !lowerState.final;
    for (vector<AccentedLevelState>::iterator stateItr=state.begin();
         stateItr!=state.end();
         stateItr++)
    {
      if (*stateItr < lowerState)
      {
        lowerState = *stateItr;
        final=stateItr->final;
      }
      else if (*stateItr == lowerState)
      {
        if (stateItr->final)
        {
          final=true;
        }
      }
    }

    if (lowerState.end())
    {
      break;
    }

    if (final)
    {
      handler->deleteAccentedForm(lowerState.accentedForm);
    }
    if (hasInfo)
    {
      handler->foundAccentedForm(lowerState.accentedForm);
      vector<LingInfoLevelState> liStates;
      vector<ConcatenatedLevelState> concatStates;
      bool finalReached=false;
      for (vector<AccentedLevelState>::iterator stateItr=state.begin();
           stateItr!=state.end();
           stateItr++)
      {
        if (*stateItr == lowerState)
        {
          finalReached = finalReached || stateItr->final;
          if (!finalReached)
          {

            unsigned char* acc=stateItr->dicoData->getEntryAddr(stateItr->accentedEntry);
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
              LWARN << "WARNING ! should never accentuate to a empty entry !";
            }
            unsigned char* accEnd=acc+tmp;
            // read linginfo
            tmp=DictionaryData::readCodedInt(acc);
            if (tmp>0)
            {
              LingInfoLevelState curState;
              curState.pos=acc;
              curState.posEnd=acc+tmp;

              // set data attributes
              curState.mainKeys=stateItr->mainKeys;
              curState.keys=stateItr->keys;
              curState.dicoData=stateItr->dicoData;

              // set state attributes
              curState.next(*m_sp);

              liStates.push_back(curState);

              acc+=tmp;
            }
            if (acc != accEnd)
            {
              // skip accented
              tmp=DictionaryData::readCodedInt(acc);
              acc+=tmp;
              if (acc != accEnd)
              {
                // read concat
                tmp=DictionaryData::readCodedInt(acc);

                ConcatenatedLevelState curState;
                curState.pos=acc;
                curState.posEnd=acc+tmp;

                // set data attributes
                curState.mainKeys=stateItr->mainKeys;
                curState.keys=stateItr->keys;
                curState.dicoData=stateItr->dicoData;

                // set state attributes
                curState.next(*m_sp);

                concatStates.push_back(curState);

                acc+=tmp;
                assert(acc == accEnd);

              }
            }

          }

          parseLingInfos(liStates,m_sp,handler);
          parseConcatenated(concatStates,m_sp,handler);

        }
      }
      handler->endAccentedForm();
    }
    // advance states
    for (vector<AccentedLevelState>::iterator stateItr=state.begin();
         stateItr!=state.end();
         stateItr++)
    {
      if (*stateItr == lowerState)
      {
        stateItr->next(*m_sp);
      }
    }
  }
  handler->endEntry();
}

void MultiLevelAnalysisDictionaryEntry::parseConcatenated(AbstractDictionaryEntryHandler* handler) const
{

  handler->startEntry(m_entryId);

  // initialize states
  vector<ConcatenatedLevelState> state;

  for (vector<LevelData>::const_iterator it=m_data.begin();
       it!=m_data.end();
       it++)
  {
    ConcatenatedLevelState curState;

    // set position attributes
    curState.pos=it->startEntryData;
    assert(curState.pos != it->endEntryData);
    // skip linginfos
    uint64_t read=DictionaryData::readCodedInt(curState.pos);
    curState.pos+=read;
    if (curState.pos != it->endEntryData)
    {
      // skip accented
      read=DictionaryData::readCodedInt(curState.pos);
      curState.pos+=read;
      if (curState.pos != it->endEntryData)
      {
        // read concat
        read=DictionaryData::readCodedInt(curState.pos);
        curState.posEnd=curState.pos+read;

        // set data attributes
        curState.mainKeys=it->mainKeys;
        curState.keys=it->keys;
        curState.dicoData=it->dicoData;

        // set state attributes
        curState.next(*m_sp);

        state.push_back(curState);

      }
    }

  }

  parseConcatenated(state,m_sp,handler);

  handler->endEntry();

}

void MultiLevelAnalysisDictionaryEntry::parseLingInfos(AbstractDictionaryEntryHandler* handler) const
{
  ANALYSISDICTLOGINIT;
  LDEBUG << "MultiLevelAnalysisDictionaryEntry::parseLingInfos initialize entry reading";
	
  handler->startEntry(m_entryId);

  // initialize states
  vector<LingInfoLevelState> state;
  for (vector<LevelData>::const_iterator it=m_data.begin();
       it!=m_data.end();
       it++)
  {
    LingInfoLevelState curState;

    // set position attributes
    curState.pos=it->startEntryData;
    uint64_t read=DictionaryData::readCodedInt(curState.pos);
    curState.posEnd=curState.pos + read;

    // set data attributes
    curState.mainKeys=it->mainKeys;
    curState.keys=it->keys;
    curState.dicoData=it->dicoData;

    // read state;
    curState.next(*m_sp);

    state.push_back(curState);
  }
  parseLingInfos(state,m_sp,handler);

  handler->endEntry();
}

MultiLevelAnalysisDictionaryEntry::LingInfoLevelState::LingInfoLevelState() :
    pos(0),
    posEnd(0),
    currentLemma(0),
    lemmaStr(),
    currentNorm(0),
    normStr(),
    lingInfoOffset(0),
    final(false),
    mainKeys(false),
    keys(0),
    dicoData(0)
{}

MultiLevelAnalysisDictionaryEntry::LingInfoLevelState::~LingInfoLevelState() {}


void MultiLevelAnalysisDictionaryEntry::LingInfoLevelState::next(FsaStringsPool& sp)
{
  ANALYSISDICTLOGINIT;
  LDEBUG << "LingInfoLevelState::next" << (void*)pos << (void*)posEnd;
  if (pos != posEnd)
  {
    currentLemma=DictionaryData::readCodedInt(pos);
    if (currentLemma==0)
    {
      final=true;
      currentLemma=DictionaryData::readCodedInt(pos);
    }
    else
    {
      final=false;
    }
    lemmaStr=keys->getSpelling(currentLemma);
    if (!mainKeys)
    {
      currentLemma=sp[lemmaStr];
    }
    currentNorm=DictionaryData::readCodedInt(pos);
    if (currentNorm==0)
    {
      currentNorm=currentLemma;
      normStr=lemmaStr;
    }
    else
    {
      normStr=keys->getSpelling(currentNorm);
      if (!mainKeys)
      {
        currentNorm=sp[normStr];
      }
    }
    LDEBUG << "LingInfoLevelState::next lemma : " << lemmaStr << ", norm : " << normStr;
    lingInfoOffset=DictionaryData::readCodedInt(pos);
  }
  else
  {
    currentLemma = std::numeric_limits<StringsPoolIndex>::max();
    lemmaStr.clear();
    currentNorm = std::numeric_limits<StringsPoolIndex>::max();
    normStr.clear();
    final = false;
    lingInfoOffset = 0;
  }
}

bool MultiLevelAnalysisDictionaryEntry::LingInfoLevelState::end() const
{
  ANALYSISDICTLOGINIT;
  LDEBUG << "LingInfoLevelState::end" << currentLemma << std::numeric_limits<StringsPoolIndex>::max();
  return ((pos==posEnd) && (currentLemma == std::numeric_limits<StringsPoolIndex>::max()));
}

bool MultiLevelAnalysisDictionaryEntry::LingInfoLevelState::operator<(const LingInfoLevelState& lis) const
{
  if (lis.end()) return true;
  if (end()) return false;
  if (currentLemma == lis.currentLemma) return (normStr < lis.normStr);
  return lemmaStr < lis.lemmaStr;
}

bool MultiLevelAnalysisDictionaryEntry::LingInfoLevelState::operator==(const LingInfoLevelState& lis) const
{
  return ((currentLemma == lis.currentLemma) && (currentNorm == lis.currentNorm));
}

void MultiLevelAnalysisDictionaryEntry::parseLingInfos(
  std::vector<LingInfoLevelState>& state,
  Lima::FsaStringsPool* sp,
  AbstractDictionaryEntryHandler* handler)
{
  if (state.empty()) return;

  ANALYSISDICTLOGINIT;
  LDEBUG << "MultiLevelAnalysisDictionaryEntry::parseLingInfos IN";

  // read entry
  while (true)
  {
    // find lower lemma,norm
    LingInfoLevelState& lowerState=state.front();
    bool final=false;
    bool hasInfos=false;
    LDEBUG << "MultiLevelAnalysisDictionaryEntry::parseLingInfos find lower state. state size=" << state.size();
    for (vector<LingInfoLevelState>::iterator stateItr=state.begin();
         stateItr!=state.end();
         stateItr++)
    {
      LDEBUG << "MultiLevelAnalysisDictionaryEntry::parseLingInfos next level is at state lemma=" << stateItr->lemmaStr;
      if (*stateItr < lowerState)
      {
        LDEBUG << "MultiLevelAnalysisDictionaryEntry::parseLingInfos is lower !";
        lowerState = *stateItr;
        hasInfos=(lowerState.lingInfoOffset !=0);
        final = lowerState.final;
      }
      else if (*stateItr == lowerState)
      {
        LDEBUG << "MultiLevelAnalysisDictionaryEntry::parseLingInfos stateItr == lowerState";
        if (!final && stateItr->lingInfoOffset !=0)
        {
          hasInfos=true;
        }
        final = stateItr->final;
      }
    }
    if (lowerState.end())
    {
      // nothing more to read, exit
      LDEBUG << "MultiLevelAnalysisDictionaryEntry::parseLingInfos nothing more to read, exit";
      break;
    }
    LDEBUG << "MultiLevelAnalysisDictionaryEntry::parseLingInfos lowerState found lemma=" << lowerState.lemmaStr << ", norm=" << lowerState.normStr;
    // if final then call delete
    if (final)
    {
      handler->deleteLingInfos(lowerState.currentLemma,lowerState.currentNorm);
    }
    // if has info, should have some properties
    if (hasInfos)
    {
      handler->foundLingInfos(lowerState.currentLemma,lowerState.currentNorm);
    }
    bool finalReached=false;
    set<LinguisticCode> propsRead;
    for (vector<LingInfoLevelState>::iterator stateItr=state.begin();
         stateItr!=state.end();
         stateItr++)
    {
      LDEBUG << "MultiLevelAnalysisDictionaryEntry::parseLingInfos in second states loop. lemma="  << stateItr->lemmaStr << ", norm=" << stateItr->normStr;
      if (*stateItr == lowerState)
      {
        LDEBUG << "MultiLevelAnalysisDictionaryEntry::parseLingInfos stateItr == lowerState !";
        // read this level
        if (!finalReached)
        {
          finalReached=stateItr->final;
          if (stateItr->lingInfoOffset!=0)
          {
            LDEBUG << "read level info";
            if (stateItr->lingInfoOffset != 0)
            {
              unsigned char* props = stateItr->dicoData->getLingPropertiesAddr(stateItr->lingInfoOffset);
              uint64_t read = DictionaryData::readCodedInt(props);
              unsigned char* propsEnd = props + read;
              while (props!=propsEnd)
              {
                LinguisticCode l=static_cast<LinguisticCode>(DictionaryData::readCodedInt(props));
                if (propsRead.find(l)==propsRead.end())
                {
                  handler->foundProperties(l);
                  propsRead.insert(l);
                }
                else
                {
                  LDEBUG << "ling properties already read in previous dictionary !";
                }
              }
            }
          }
        }
        // next state
        (*stateItr).next(*sp);
      }
    }
    if (hasInfos)
    {
      handler->endLingInfos();
    }
  }
}

MultiLevelAnalysisDictionaryEntry::ConcatenatedLevelState::Component::Component() :
    form(0),
    formStr(),
    pos(0),
    len(0),
    liState()
{}

MultiLevelAnalysisDictionaryEntry::ConcatenatedLevelState::Component::~Component()
{}

MultiLevelAnalysisDictionaryEntry::ConcatenatedLevelState::ConcatenatedLevelState() :
    pos(0),
    posEnd(0),
    final(false),
    components(),
    mainKeys(false),
    keys(0),
    dicoData(0)
{}

MultiLevelAnalysisDictionaryEntry::ConcatenatedLevelState::~ConcatenatedLevelState() {}

void MultiLevelAnalysisDictionaryEntry::ConcatenatedLevelState::next(FsaStringsPool& sp)
{
  ANALYSISDICTLOGINIT;
  LDEBUG << "ConcatenatedLevelState::next" << (void*)pos << (void*)posEnd;
  
  components.clear();
  if (pos != posEnd)
  {
    uint64_t read=DictionaryData::readCodedInt(pos);
    if (read==0)
    {
      final=true;
      read=DictionaryData::readCodedInt(pos);
    }
    else
    {
      final=false;
    }
    // read the components
    for (uint64_t nb=read;nb>0;nb--)
    {
      components.push_back(Component());
      Component& c=components.back();

      // set component attributes
      c.form=DictionaryData::readCodedInt(pos);
      c.formStr=keys->getSpelling(c.form);
      if (!mainKeys)
      {
        c.form=sp[c.formStr];
      }
      c.pos=DictionaryData::readCodedInt(pos);
      c.len=DictionaryData::readCodedInt(pos);

      // set ling info state position attribute
      read=DictionaryData::readCodedInt(pos);
      c.liState.pos=pos;
      pos+=read;
      c.liState.posEnd=pos;

      // set ling info state data attributes
      c.liState.mainKeys=mainKeys;
      c.liState.keys=keys;
      c.liState.dicoData=dicoData;

      // set ling info state attributes
      c.liState.next(sp);
    }
  }
}

bool MultiLevelAnalysisDictionaryEntry::ConcatenatedLevelState::end() const
{
  return (pos==posEnd) && components.empty();
}


void MultiLevelAnalysisDictionaryEntry::parseConcatenated(
  std::vector<ConcatenatedLevelState>& state,
  Lima::FsaStringsPool* sp,
  AbstractDictionaryEntryHandler* handler)
{
  ANALYSISDICTLOGINIT;
  LDEBUG << "MultiLevelAnalysisDictionaryEntry::parseConcatenated";
  if (state.empty()) return;
  // read entry



  while (true)
  {
    // find lower state
    LDEBUG << "MultiLevelAnalysisDictionaryEntry::parseConcatenated find lower concat state";
    ConcatenatedLevelState lowerState=state.front();
    bool final=false;
    bool hasInfos=false;
    for (vector<ConcatenatedLevelState>::iterator stateItr=state.begin();
         stateItr!=state.end();
         stateItr++)
    {
      LDEBUG << "state has " << stateItr->components.size() << " components";
      if (*stateItr < lowerState)
      {
        LDEBUG << "is lower !";
        lowerState = *stateItr;
        final=stateItr->final;
        hasInfos= (stateItr->components.front().liState.lingInfoOffset != 0);
      }
      else if (*stateItr == lowerState)
      {
        LDEBUG << "MultiLevelAnalysisDictionaryEntry::parseConcatenated stateItr == lowerState";
        if (!final)
        {
          if (!stateItr->components.empty() && stateItr->components.front().liState.lingInfoOffset != 0)
          {
            hasInfos=true;
          }
          if (stateItr->final)
          {
            final=true;
          }
        }
      }
    }
    // check if info to read
    if (lowerState.end())
    {
      break;
    }
    LDEBUG << "MultiLevelAnalysisDictionaryEntry::parseConcatenated read infos";
    // read info
    if (final)
    {
      handler->deleteConcatenated();
      for (vector<ConcatenatedLevelState::Component>::const_iterator cItr=lowerState.components.begin();
           cItr!=lowerState.components.end();
           cItr++)
      {
        handler->foundComponent(cItr->pos,cItr->len,cItr->form);
      }
      handler->endConcatenated();
    }
    bool finalReached=false;
    if (hasInfos)
    {
      // retrive all components iterators
      vector<pair<vector<ConcatenatedLevelState::Component>::iterator,vector<ConcatenatedLevelState::Component>::iterator> > componentsIt;
      for (vector<ConcatenatedLevelState>::iterator stateItr=state.begin();
           stateItr!=state.end();
           stateItr++)
      {
        if (*stateItr == lowerState)
        {
          LDEBUG << "MultiLevelAnalysisDictionaryEntry::parseConcatenated stateItr == lowerState";
          if (!finalReached)
          {
            componentsIt.push_back(make_pair(stateItr->components.begin(),stateItr->components.end()));
            if (stateItr->final)
            {
              finalReached=true;
            }
          }
        }
      }
      // read all components
      LDEBUG << "MultiLevelAnalysisDictionaryEntry::parseConcatenated read the " << componentsIt.size() << " level iterators";
      handler->foundConcatenated();
      while (componentsIt.front().first != componentsIt.front().second)
      {
        handler->foundComponent(componentsIt.front().first->pos,componentsIt.front().first->len,componentsIt.front().first->form);
        vector<LingInfoLevelState> lingInfoState;
        for (vector<pair<vector<ConcatenatedLevelState::Component>::iterator,vector<ConcatenatedLevelState::Component>::iterator> >::iterator stateItr=componentsIt.begin();
             stateItr!=componentsIt.end();
             stateItr++)
        {
          assert(stateItr->first != stateItr->second);
          lingInfoState.push_back(stateItr->first->liState);
          stateItr->first.operator++();
        }
        parseLingInfos(lingInfoState,sp,handler);
        handler->endComponent();
      }
      handler->endConcatenated();
    }
    // next states
    for (vector<ConcatenatedLevelState>::iterator stateItr=state.begin();
         stateItr!=state.end();
         stateItr++)
    {
      if (*stateItr == lowerState)
      {
        stateItr->next(*sp);
      }
    }

  }
}

void MultiLevelAnalysisDictionaryEntry::AccentedLevelState::next(FsaStringsPool& sp)
{
  ANALYSISDICTLOGINIT;
  LDEBUG << "AccentedLevelState::next" << (void*)pos << (void*)posEnd;

  if (pos != posEnd)
  {
    accentedEntry=DictionaryData::readCodedInt(pos);
    if (accentedEntry == 0)
    {
      final=true;
      accentedEntry=DictionaryData::readCodedInt(pos);
    }
    else
    {
      final=false;
    }
    accentedFormStr=keys->getSpelling(accentedEntry);
    if (!mainKeys)
    {
      accentedForm=sp[accentedFormStr];
    }
    else
    {
      accentedForm = accentedEntry;
    }
  }
  else
  {
    final=false;
    accentedEntry=numeric_limits<StringsPoolIndex>::max();
    accentedForm=numeric_limits<StringsPoolIndex>::max();
  }

}

bool MultiLevelAnalysisDictionaryEntry::AccentedLevelState::end() const
{
  return accentedForm == numeric_limits<StringsPoolIndex>::max();
}

bool MultiLevelAnalysisDictionaryEntry::AccentedLevelState::operator<(const AccentedLevelState& as) const
{
  if (as.end()) return true;
  if (end()) return false;
  return accentedFormStr < as.accentedFormStr;
}

bool MultiLevelAnalysisDictionaryEntry::AccentedLevelState::operator==(const AccentedLevelState& as) const
{
  return accentedForm == as.accentedForm;
}




}

}

}
