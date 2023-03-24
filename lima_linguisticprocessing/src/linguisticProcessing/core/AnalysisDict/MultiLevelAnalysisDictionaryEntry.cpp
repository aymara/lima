// Copyright 2002-2020 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2020 by CEA LIST                                   *
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

namespace Lima {
namespace LinguisticProcessing {
namespace AnalysisDict {

class LingInfoLevelState
{
public:
  LingInfoLevelState();
  ~LingInfoLevelState() = default;
  LingInfoLevelState(const LingInfoLevelState&) = default;
  LingInfoLevelState& operator=(const LingInfoLevelState&) = default;

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
  std::shared_ptr<Lima::Common::AbstractAccessByString> keys;
  const DictionaryData* dicoData;

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

    Component() = default;
    ~Component() = default;
    Component(const Component&) = default;
    Component& operator=(const Component&) = default;

    bool operator<(const Component& c) const { return formStr<c.formStr;};
    bool operator==(const Component& c) const { return form==c.form;};
  };

  ConcatenatedLevelState() = default;
  ~ConcatenatedLevelState() = default;
  ConcatenatedLevelState(const ConcatenatedLevelState&) = default;
  ConcatenatedLevelState& operator=(const ConcatenatedLevelState&) = default;

  // position attributes
  unsigned char* pos;
  unsigned char* posEnd;

  // state attributes
  bool final;
  std::vector<Component> components;

  // data attributes
  bool mainKeys;
  std::shared_ptr<Lima::Common::AbstractAccessByString> keys;
  const DictionaryData* dicoData;


  bool operator<(const ConcatenatedLevelState& cls) const
  {
    return (cls.components.empty()
      || ( !components.empty() && (components < cls.components) ) );
  };
  bool operator==(const ConcatenatedLevelState& cls) const
  {
    return (components == cls.components);
  };

  void next(FsaStringsPool& sp);
  bool end() const;

};

class AccentedLevelState
{
public:
  AccentedLevelState();
  ~AccentedLevelState() = default;
  AccentedLevelState(const AccentedLevelState&) = default;
  AccentedLevelState& operator=(const AccentedLevelState&) = default;

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
  std::shared_ptr<Lima::Common::AbstractAccessByString> keys;
  const DictionaryData* dicoData;

  void next(FsaStringsPool& sp);
  bool end() const;
  bool operator<(const AccentedLevelState& lis) const;
  bool operator==(const AccentedLevelState& lis) const;
};

AccentedLevelState::AccentedLevelState() :
    pos(0),
    posEnd(0),
    accentedForm(STRINGS_POOL_INDEX_MAX_VALUE),
    accentedFormStr(),
    accentedEntry(STRINGS_POOL_INDEX_MAX_VALUE),
    final(false),
    mainKeys(false),
    keys(0),
    dicoData(0)
{}


void parseLingInfos(std::vector<LingInfoLevelState>& data,Lima::FsaStringsPool* sp,
                    AbstractDictionaryEntryHandler* handler);
void parseConcatenated(std::vector<ConcatenatedLevelState>& data,Lima::FsaStringsPool* sp,
                       AbstractDictionaryEntryHandler* handler);


class MultiLevelAnalysisDictionaryEntryPrivate
{
  friend class MultiLevelAnalysisDictionaryEntry;

  MultiLevelAnalysisDictionaryEntryPrivate(
    const std::vector<MultiLevelAnalysisDictionaryEntry::LevelData>& data,
    Lima::FsaStringsPool* sp) :
      m_data(data),
      m_sp(sp) {};


  virtual ~MultiLevelAnalysisDictionaryEntryPrivate() = default;

  MultiLevelAnalysisDictionaryEntryPrivate(const MultiLevelAnalysisDictionaryEntryPrivate& ) = default;
  MultiLevelAnalysisDictionaryEntryPrivate& operator=(const MultiLevelAnalysisDictionaryEntryPrivate& ) = default;

  std::vector<MultiLevelAnalysisDictionaryEntry::LevelData> m_data;
  Lima::FsaStringsPool* m_sp;

};


MultiLevelAnalysisDictionaryEntry::MultiLevelAnalysisDictionaryEntry(
  StringsPoolIndex formId,
  bool isFinal,
  bool isEmpty,
  bool hasLingInfos,
  bool hasConcatenated,
  bool hasAccentedForm,
  const std::vector<LevelData>& data,
  Lima::FsaStringsPool* sp) :
    AbstractDictionaryEntry(formId,
                            isFinal,
                            isEmpty,
                            hasLingInfos,
                            hasConcatenated,
                            hasAccentedForm),
    m_d(new MultiLevelAnalysisDictionaryEntryPrivate(data, sp))
{
}

MultiLevelAnalysisDictionaryEntry::MultiLevelAnalysisDictionaryEntry(
  const MultiLevelAnalysisDictionaryEntry& made) :
    AbstractDictionaryEntry(made),
    m_d(new MultiLevelAnalysisDictionaryEntryPrivate(*made.m_d))
{
}

MultiLevelAnalysisDictionaryEntry& MultiLevelAnalysisDictionaryEntry::operator=(
  const MultiLevelAnalysisDictionaryEntry& made)
{
  *m_d = *made.m_d;
  return *this;
}


AbstractDictionaryEntry* MultiLevelAnalysisDictionaryEntry::clone()
{
  return new MultiLevelAnalysisDictionaryEntry(*this);
}

void MultiLevelAnalysisDictionaryEntry::parseAccentedForms(
  AbstractDictionaryEntryHandler* handler) const
{
#ifdef DEBUG_LP
  ANALYSISDICTLOGINIT;
  LDEBUG << "MultiLevelAnalysisDictionaryEntry::parseAccentedForms";
#endif
  handler->startEntry(m_entryId);

  // initialize states
  std::vector<AccentedLevelState> state;
  for (const auto& ld : m_d->m_data)
  {
    AccentedLevelState curState;

    // set position attributes
    curState.pos = ld.startEntryData;
    assert(curState.pos != ld.endEntryData);
    // skip linginfos
    auto read = DictionaryData::readCodedInt(curState.pos);
    curState.pos += read;
    if (curState.pos != ld.endEntryData)
    {
      // read accented
      read = DictionaryData::readCodedInt(curState.pos);
      curState.posEnd= curState.pos + read;

      // set data attributes
      curState.mainKeys = ld.mainKeys;
      curState.keys = ld.keys;
      curState.dicoData = ld.dicoData;

      // set state attributes
      curState.next(*m_d->m_sp);

      state.push_back(curState);
    }
  }

  // parse accented forms
  while (true)
  {
    // find lower state
    auto* lowerState = &state.front();
    auto final = lowerState->final;
    auto hasInfo = !lowerState->final;
    for (auto& als : state)
    {
      if (als < *lowerState)
      {
        lowerState = &als;
        final=als.final;
      }
      else if (als == *lowerState)
      {
        if (als.final)
        {
          final=true;
        }
      }
    }

    if (lowerState->end())
    {
      break;
    }

    if (final)
    {
      handler->deleteAccentedForm(lowerState->accentedForm);
    }
    if (hasInfo)
    {
      handler->foundAccentedForm(lowerState->accentedForm);
      std::vector<LingInfoLevelState> liStates;
      std::vector<ConcatenatedLevelState> concatStates;
      auto finalReached = false;
      for (const auto& als : state)
      {
        if (als == *lowerState)
        {
          finalReached = finalReached || als.final;
          if (!finalReached)
          {

            auto acc = als.dicoData->getEntryAddr(als.accentedEntry);
            auto tmp = DictionaryData::readCodedInt(acc);
            if (tmp == 1)
            {
              ANALYSISDICTLOGINIT;
              LWARN << "WARNING ! should never accentuate to a delete entry !";
              tmp = DictionaryData::readCodedInt(acc);
            }
            // tmp contains length
            if (tmp == 0)
            {
              ANALYSISDICTLOGINIT
              LWARN << "WARNING ! should never accentuate to a empty entry !";
            }
            auto accEnd = acc+tmp;
            // read linginfo
            tmp = DictionaryData::readCodedInt(acc);
            if (tmp > 0)
            {
              LingInfoLevelState curState;
              curState.pos = acc;
              curState.posEnd = acc + tmp;

              // set data attributes
              curState.mainKeys = als.mainKeys;
              curState.keys = als.keys;
              curState.dicoData = als.dicoData;

              // set state attributes
              curState.next(*m_d->m_sp);

              liStates.push_back(curState);

              acc += tmp;
            }
            if (acc != accEnd)
            {
              // skip accented
              tmp = DictionaryData::readCodedInt(acc);
              acc += tmp;
              if (acc != accEnd)
              {
                // read concat
                tmp = DictionaryData::readCodedInt(acc);

                ConcatenatedLevelState curState;
                curState.pos = acc;
                curState.posEnd = acc+tmp;

                // set data attributes
                curState.mainKeys = als.mainKeys;
                curState.keys = als.keys;
                curState.dicoData = als.dicoData;

                // set state attributes
                curState.next(*m_d->m_sp);

                concatStates.push_back(curState);

                acc += tmp;
                assert(acc == accEnd);
              }
            }
          }

          AnalysisDict::parseLingInfos(liStates, m_d->m_sp, handler);
          AnalysisDict::parseConcatenated(concatStates, m_d->m_sp, handler);

        }
      }
      handler->endAccentedForm();
    }
    // advance states
    for (auto& als : state)
    {
      if (als == *lowerState)
      {
        als.next(*m_d->m_sp);
      }
    }
  }
  handler->endEntry();
}

void MultiLevelAnalysisDictionaryEntry::parseConcatenated(
  AbstractDictionaryEntryHandler* handler) const
{

  handler->startEntry(m_entryId);

  // initialize states
  std::vector<ConcatenatedLevelState> state;

  for (const auto& ld : m_d->m_data)
  {
    ConcatenatedLevelState curState;

    // set position attributes
    curState.pos = ld.startEntryData;
    assert(curState.pos != ld.endEntryData);

    // skip linginfos
    uint64_t read = DictionaryData::readCodedInt(curState.pos);
    curState.pos += read;
    if (curState.pos != ld.endEntryData)
    {
      // skip accented
      read = DictionaryData::readCodedInt(curState.pos);
      curState.pos += read;
      if (curState.pos != ld.endEntryData)
      {
        // read concat
        read = DictionaryData::readCodedInt(curState.pos);
        curState.posEnd = curState.pos+read;

        // set data attributes
        curState.mainKeys = ld.mainKeys;
        curState.keys = ld.keys;
        curState.dicoData = ld.dicoData;

        // set state attributes
        curState.next(*m_d->m_sp);

        state.push_back(curState);
      }
    }
  }

  AnalysisDict::parseConcatenated(state, m_d->m_sp, handler);

  handler->endEntry();

}

void MultiLevelAnalysisDictionaryEntry::parseLingInfos(
  AbstractDictionaryEntryHandler* handler) const
{
#ifdef DEBUG_LP
  ANALYSISDICTLOGINIT;
  LDEBUG << "MultiLevelAnalysisDictionaryEntry::parseLingInfos initialize entry reading";
#endif

  handler->startEntry(m_entryId);

  // initialize states
  std::vector<LingInfoLevelState> states;
  for (const auto& ld : m_d->m_data)
  {
    LingInfoLevelState curState;

    // set position attributes
    curState.pos = ld.startEntryData;
    auto read = DictionaryData::readCodedInt(curState.pos);
    curState.posEnd = curState.pos + read;

    // set data attributes
    curState.mainKeys = ld.mainKeys;
    curState.keys = ld.keys;
    curState.dicoData = ld.dicoData;

    // read current level first state;
    curState.next(*m_d->m_sp);

    states.push_back(curState);
  }
  AnalysisDict::parseLingInfos(states, m_d->m_sp, handler);

  handler->endEntry();
}

LingInfoLevelState::LingInfoLevelState() :
    pos(0),
    posEnd(0),
    currentLemma(STRINGS_POOL_INDEX_MAX_VALUE),
    lemmaStr(),
    currentNorm(STRINGS_POOL_INDEX_MAX_VALUE),
    normStr(),
    lingInfoOffset(0),
    final(false),
    mainKeys(false),
    keys(0),
    dicoData(0)
{}

void LingInfoLevelState::next(FsaStringsPool& sp)
{
#ifdef DEBUG_LP
  ANALYSISDICTLOGINIT;
  LDEBUG << "LingInfoLevelState::next" << (void*)pos << (void*)posEnd
          << "lemma : " << lemmaStr << ", norm : " << normStr;
#endif
  if (pos != posEnd)
  {
    currentLemma = DictionaryData::readCodedInt(pos);
    if (currentLemma == 0)
    {
      final = true;
      currentLemma = DictionaryData::readCodedInt(pos);
    }
    else
    {
      final = false;
    }
    lemmaStr = keys->getSpelling(currentLemma);
    if (!mainKeys)
    {
      currentLemma = sp[lemmaStr];
    }
    currentNorm = DictionaryData::readCodedInt(pos);
    if (currentNorm == 0)
    {
      currentNorm = currentLemma;
      normStr = lemmaStr;
    }
    else
    {
      normStr = keys->getSpelling(currentNorm);
      if (!mainKeys)
      {
        currentNorm = sp[normStr];
      }
    }
    lingInfoOffset = DictionaryData::readCodedInt(pos);
  }
  else
  {
    currentLemma = STRINGS_POOL_INDEX_MAX_VALUE;
    lemmaStr.clear();
    currentNorm = STRINGS_POOL_INDEX_MAX_VALUE;
    normStr.clear();
    final = false;
    lingInfoOffset = 0;
  }
#ifdef DEBUG_LP
  LDEBUG << "LingInfoLevelState::next on OUT:" << (void*)pos << (void*)posEnd
          << "lemma : " << lemmaStr << ", norm : " << normStr;
#endif
}

bool LingInfoLevelState::end() const
{
#ifdef DEBUG_LP
  ANALYSISDICTLOGINIT;
  LDEBUG << "LingInfoLevelState::end" << pos << posEnd << currentLemma
          << STRINGS_POOL_INDEX_MAX_VALUE;
#endif
  return ((pos==posEnd) && (currentLemma == STRINGS_POOL_INDEX_MAX_VALUE));
}

bool LingInfoLevelState::operator<(const LingInfoLevelState& lis) const
{
  if (lis.end()) return true;
  if (end()) return false;
  if (currentLemma == lis.currentLemma) return (normStr < lis.normStr);
  return lemmaStr < lis.lemmaStr;
}

bool LingInfoLevelState::operator==(const LingInfoLevelState& lis) const
{
  return ((currentLemma == lis.currentLemma) && (currentNorm == lis.currentNorm));
}

void parseLingInfos(
  std::vector<LingInfoLevelState>& states,
  Lima::FsaStringsPool* sp,
  AbstractDictionaryEntryHandler* handler)
{
  if (states.empty()) return;

#ifdef DEBUG_LP
  ANALYSISDICTLOGINIT;
  LDEBUG << "MultiLevelAnalysisDictionaryEntry::parseLingInfos IN";
#endif
  // read entry
  while (true)
  {
    // find lower lemma,norm
    auto* lowerState = &states.front();
    auto final = false;
    auto hasInfos = false;
#ifdef DEBUG_LP
    LDEBUG << "MultiLevelAnalysisDictionaryEntry::parseLingInfos find lower state. state size="
            << states.size();
#endif
    for (auto& state : states)
    {
#ifdef DEBUG_LP
      LDEBUG << "MultiLevelAnalysisDictionaryEntry::parseLingInfos next level is at state lemma="
              << state.lemmaStr;
#endif
      if (state < *lowerState)
      {
#ifdef DEBUG_LP
        LDEBUG << "MultiLevelAnalysisDictionaryEntry::parseLingInfos is lower !";
#endif
        lowerState = &state;
        hasInfos = (lowerState->lingInfoOffset !=0);
        final = lowerState->final;
      }
      else if (state == *lowerState)
      {
#ifdef DEBUG_LP
        LDEBUG << "MultiLevelAnalysisDictionaryEntry::parseLingInfos stateItr == lowerState";
#endif
        if (!final && state.lingInfoOffset != 0)
        {
          hasInfos = true;
        }
        final = state.final;
      }
    }
    if (lowerState->end())
    {
      // nothing more to read, exit the while(true)
#ifdef DEBUG_LP
     LDEBUG << "MultiLevelAnalysisDictionaryEntry::parseLingInfos nothing more to read, exit";
#endif
      break;
    }
#ifdef DEBUG_LP
    LDEBUG << "MultiLevelAnalysisDictionaryEntry::parseLingInfos lowerState found lemma="
            << lowerState->lemmaStr << ", norm=" << lowerState->normStr;
#endif
    // if final then call delete
    if (final)
    {
      handler->deleteLingInfos(lowerState->currentLemma, lowerState->currentNorm);
    }
    // if has info, should have some properties
    if (hasInfos)
    {
      handler->foundLingInfos(lowerState->currentLemma, lowerState->currentNorm);
    }
    bool finalReached = false;
    std::set<LinguisticCode> propsRead;
    for (auto& state : states)
    {
#ifdef DEBUG_LP
      LDEBUG << "MultiLevelAnalysisDictionaryEntry::parseLingInfos in second states loop. lemma="
              << state.lemmaStr << ", norm=" << state.normStr;
#endif
      if (state == *lowerState)
      {
#ifdef DEBUG_LP
        LDEBUG << "MultiLevelAnalysisDictionaryEntry::parseLingInfos stateItr == lowerState !";
#endif
        // read this level
        if (!finalReached)
        {
          finalReached = state.final;
          if (state.lingInfoOffset != 0)
          {
#ifdef DEBUG_LP
            LDEBUG << "MultiLevelAnalysisDictionaryEntry::parseLingInfos read level info";
#endif
            if (state.lingInfoOffset != 0)
            {
              auto props = state.dicoData->getLingPropertiesAddr(state.lingInfoOffset);
              auto read = DictionaryData::readCodedInt(props);
              auto propsEnd = props + read;
              while (props!=propsEnd)
              {
                auto l = LinguisticCode::decodeFromBinary(props);
#ifdef DEBUG_LP
                LDEBUG << "MultiLevelAnalysisDictionaryEntry::parseLingInfos got linguistic code" << l.toString();
#endif
                if (propsRead.find(l) == propsRead.end())
                {
                  handler->foundProperties(l);
                  propsRead.insert(l);
                }
                else
                {
#ifdef DEBUG_LP
                  LDEBUG << "MultiLevelAnalysisDictionaryEntry::parseLingInfos ling properties already read in previous dictionary !";
#endif
                }
              }
            }
          }
        }
        // next state
        state.next(*sp);
      }
    }
    if (hasInfos)
    {
      handler->endLingInfos();
    }
  }
}

void ConcatenatedLevelState::next(FsaStringsPool& sp)
{
#ifdef DEBUG_LP
  ANALYSISDICTLOGINIT;
  LDEBUG << "ConcatenatedLevelState::next" << (void*)pos << (void*)posEnd;
#endif

  components.clear();
  if (pos != posEnd)
  {
    auto read = DictionaryData::readCodedInt(pos);
    if (read == 0)
    {
      final = true;
      read = DictionaryData::readCodedInt(pos);
    }
    else
    {
      final = false;
    }
    // read the components
    for (uint64_t nb = read; nb > 0; nb--)
    {
      components.push_back(Component());
      auto& c = components.back();

      // set component attributes
      c.form = DictionaryData::readCodedInt(pos);
      c.formStr = keys->getSpelling(c.form);
      if (!mainKeys)
      {
        c.form = sp[c.formStr];
      }
      c.pos = DictionaryData::readCodedInt(pos);
      c.len = DictionaryData::readCodedInt(pos);

      // set ling info state position attribute
      read = DictionaryData::readCodedInt(pos);
      c.liState.pos = pos;
      pos += read;
      c.liState.posEnd = pos;

      // set ling info state data attributes
      c.liState.mainKeys = mainKeys;
      c.liState.keys = keys;
      c.liState.dicoData = dicoData;

      // set ling info state attributes
      c.liState.next(sp);
    }
  }
}

bool ConcatenatedLevelState::end() const
{
  return (pos==posEnd) && components.empty();
}


void parseConcatenated(
  std::vector<ConcatenatedLevelState>& state,
  Lima::FsaStringsPool* sp,
  AbstractDictionaryEntryHandler* handler)
{
#ifdef DEBUG_LP
  ANALYSISDICTLOGINIT;
  LDEBUG << "parseConcatenated";
#endif
  if (state.empty()) return;

  // read entry
  while (true)
  {
    // find lower state
#ifdef DEBUG_LP
    LDEBUG << "MultiLevelAnalysisDictionaryEntry::parseConcatenated find lower concat state";
#endif
    auto* lowerState = &state.front();
    auto final = false;
    auto hasInfos = false;
    for (auto& cls : state)
    {
#ifdef DEBUG_LP
      LDEBUG << "state has " << cls.components.size() << " components";
#endif
      if (cls < *lowerState)
      {
#ifdef DEBUG_LP
        LDEBUG << "is lower !";
#endif
        lowerState = &cls;
        final = cls.final;
        hasInfos = (cls.components.front().liState.lingInfoOffset != 0);
      }
      else if (cls == *lowerState)
      {
#ifdef DEBUG_LP
        LDEBUG << "MultiLevelAnalysisDictionaryEntry::parseConcatenated stateItr == lowerState";
#endif
        if (!final)
        {
          if (!cls.components.empty()
            && cls.components.front().liState.lingInfoOffset != 0)
          {
            hasInfos = true;
          }
          if (cls.final)
          {
            final = true;
          }
        }
      }
    }
    // check if info to read
    if (lowerState->end())
    {
      break;
    }
#ifdef DEBUG_LP
    LDEBUG << "MultiLevelAnalysisDictionaryEntry::parseConcatenated read infos";
#endif
    // read info
    if (final)
    {
      handler->deleteConcatenated();
      for (const auto& component : lowerState->components)
      {
        handler->foundComponent(component.pos, component.len, component.form);
      }
      handler->endConcatenated();
    }
    auto finalReached = false;
    if (hasInfos)
    {
      // retrieve all components iterators
      std::vector<std::pair<std::vector<ConcatenatedLevelState::Component>::iterator,
                            std::vector<ConcatenatedLevelState::Component>::iterator> > componentsIt;
      for (auto& cls : state)
      {
        if (cls == *lowerState)
        {
#ifdef DEBUG_LP
          LDEBUG << "MultiLevelAnalysisDictionaryEntry::parseConcatenated stateItr == lowerState";
#endif
          if (!finalReached)
          {
            componentsIt.push_back(std::make_pair(cls.components.begin(),
                                                  cls.components.end()));
            if (cls.final)
            {
              finalReached = true;
            }
          }
        }
      }
      // read all components
#ifdef DEBUG_LP
      LDEBUG << "MultiLevelAnalysisDictionaryEntry::parseConcatenated read the "
              << componentsIt.size() << " level iterators";
#endif
      handler->foundConcatenated();
      while (componentsIt.front().first != componentsIt.front().second)
      {
        handler->foundComponent(componentsIt.front().first->pos,
                                componentsIt.front().first->len,
                                componentsIt.front().first->form);
        std::vector<LingInfoLevelState> lingInfoState;
        for (auto& component : componentsIt)
        {
          assert(component.first != component.second);
          lingInfoState.push_back(component.first->liState);
          component.first.operator++();
        }
        parseLingInfos(lingInfoState, sp, handler);
        handler->endComponent();
      }
      handler->endConcatenated();
    }
    // next states
    for (auto& cls : state)
    {
      if (cls == *lowerState)
      {
        cls.next(*sp);
      }
    }

  }
}

void AccentedLevelState::next(FsaStringsPool& sp)
{
#ifdef DEBUG_LP
  ANALYSISDICTLOGINIT;
  LDEBUG << "AccentedLevelState::next" << (void*)pos << (void*)posEnd;
#endif

  if (pos != posEnd)
  {
    accentedEntry = DictionaryData::readCodedInt(pos);
    if (accentedEntry == 0)
    {
      final = true;
      accentedEntry = DictionaryData::readCodedInt(pos);
    }
    else
    {
      final = false;
    }
    accentedFormStr = keys->getSpelling(accentedEntry);
    if (!mainKeys)
    {
      accentedForm = sp[accentedFormStr];
    }
    else
    {
      accentedForm = accentedEntry;
    }
  }
  else
  {
    final = false;
    accentedEntry = STRINGS_POOL_INDEX_MAX_VALUE;
    accentedForm = STRINGS_POOL_INDEX_MAX_VALUE;
  }

}

bool AccentedLevelState::end() const
{
  return accentedForm == STRINGS_POOL_INDEX_MAX_VALUE;
}

bool AccentedLevelState::operator<(const AccentedLevelState& as) const
{
  if (as.end()) return true;
  if (end()) return false;
  return accentedFormStr < as.accentedFormStr;
}

bool AccentedLevelState::operator==(const AccentedLevelState& as) const
{
  return accentedForm == as.accentedForm;
}


} // namespace
} // namespace
} // namespace
