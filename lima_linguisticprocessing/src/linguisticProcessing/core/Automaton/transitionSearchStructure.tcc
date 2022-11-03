// Copyright 2002-2018 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       transitionSearchStructure.tcc
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Mon Oct 11 2004
 * copyright   Copyright (C) 2004 by CEA LIST
 * Project     Automaton
 *
 * @brief      definition of functions for template class
 *
 ***********************************************************************/

// transitions
#include "transitionUnit.h"
#include "posTransition.h"
#include "wordTransition.h"
#include "lemmaTransition.h"
#include "numericTransition.h"
#include "epsilonTransition.h"
#include "gazeteerTransition.h"
#include "starTransition.h"
#include "tstatusTransition.h"
#include "andTransition.h"
#include "setTransition.h"
#include "deaccentuatedTransition.h"
#include "common/MediaticData/mediaticData.h"
#include <deque>
#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

//************************************************************************
// constructor
//************************************************************************
template <typename TargetType>
TransitionSearchStructure<TargetType>::TransitionSearchStructure():
m_wordMap(),
m_posMap(),
m_lemmaMap(),
m_tstatusMap(),
m_otherTransitions(){
}

//************************************************************************
// destructor
//************************************************************************
template<typename MapType>
void clearTransitionSearchStructureMap(MapType& theMap) {
  typename MapType::iterator
    it=theMap.begin(),
    it_end=theMap.end();
  for (; it!=it_end; it++) {
    delete (*it).second;
    (*it).second=0;
  }
  theMap.clear();
}

template <typename TargetType>
TransitionSearchStructure<TargetType>::~TransitionSearchStructure() {
  clearMaps();
}

template <typename TargetType>
void TransitionSearchStructure<TargetType>::
clearMaps() {
  clearTransitionSearchStructureMap(m_wordMap);
  clearTransitionSearchStructureMap(m_posMap);
  clearTransitionSearchStructureMap(m_lemmaMap);
  clearTransitionSearchStructureMap(m_tstatusMap);
  typename TransitionList::iterator
    oit=m_otherTransitions.begin(),
    oit_end=m_otherTransitions.end();
  for (; oit!=oit_end; oit++) {
    delete (*oit).first;
    delete (*oit).second;
    (*oit).first=0;
    (*oit).second=0;
  }
  m_otherTransitions.clear();
}

template <typename TargetType>
void TransitionSearchStructure<TargetType>::
clear() {
  clearMaps();
}

template <typename TargetType>
bool TransitionSearchStructure<TargetType>::
empty() const {
  if (! m_wordMap.empty() ) { return false; }
  if (! m_posMap.empty() ) { return false; }
  if (! m_lemmaMap.empty() ) { return false; }
  if (! m_tstatusMap.empty() ) { return false; }
  if (! m_otherTransitions.empty() ) { return false; }
  return true;
}

//************************************************************************
// initialize the search structure from a list of objects
//************************************************************************
template <typename TargetType>
void TransitionSearchStructure<TargetType>::init(const std::vector<TargetType>& l,
     const Common::PropertyCode::PropertyAccessor* macroAccessor,
     const Common::PropertyCode::PropertyAccessor* microAccessor)
{
#ifdef LDEBUG
  AULOGINIT;
  LDEBUG << "TransitionSearchStructure::init" << (void*)macroAccessor << (void*)microAccessor;
#endif

  for (auto it=l.begin(), it_end=l.end(); it!=it_end;it++) {
    auto transition = (*it).transitionUnit();

    auto newTarget = new TargetType(*it);
    // transition type is not used in target (use only transition properties,
    // suchas constraints, keep, negative etc): to optimize space, it
    // can be replaced by empty star transition (no info)
    // => transition type can be used for debug
    //newTarget->setTransitionUnit(new StarTransition(*transition));

    // negative transitions must be in linear search
    if (transition->negative()) {
      m_otherTransitions.push_back(std::make_pair(transition->clone(),newTarget));
      continue;
    }

    switch(transition->type()) {
    case T_WORD: {
      WordTransition* t=static_cast<WordTransition*>(transition);
//       LDEBUG << "TransitionSearchStructure: insert WordTransition "
//              << t->printValue();
      m_wordMap.insert(std::make_pair(t->word(),newTarget));
      break;
    }
    case T_POS: {
      PosTransition* t=static_cast<PosTransition*>(transition);
//       LDEBUG << "TransitionSearchStructure: insert PosTransition "
//              << t->printValue();
      m_posMap.insert(std::make_pair(t->pos(),newTarget));
      break;
    }
    case T_LEMMA: {
      LemmaTransition* t=static_cast<LemmaTransition*>(transition);
//       LDEBUG << "TransitionSearchStructure: insert LemmaTransition "
//              << t->printValue();
      m_lemmaMap.insert(std::make_pair(std::make_pair(t->lemma(),t->partOfSpeech()),newTarget));
      break;
    }
    case T_TSTATUS: {
      TStatusTransition* t=static_cast<TStatusTransition*>(transition);
//       LDEBUG << "TransitionSearchStructure: insert TstatusTransition "
//              << t->printValue();
      m_tstatusMap.insert(std::make_pair(t->status(),newTarget));
      break;
    }
    case T_GAZETEER:
    case T_STAR:
    case T_NUM:
    case T_AND:
    case T_SET:
    case T_ENTITY:
    case T_ENTITY_GROUP:
    case T_DEACCENTUATED: {
//       LDEBUG << "TransitionSearchStructure: insert other Transition "
//              << transition->printValue();
      m_otherTransitions.push_back(std::make_pair(transition->clone(),newTarget));
      break;
    }
    default: {
      AULOGINIT;
      LERROR << "transition type " << transition->type()
             << " not handled in transitionSearchStructure";
    }
    }
  }

//   LDEBUG << m_posMap.size() << " pos stored in posMap";

  // initialization of helper members (to avoid doing it for each comparison)
  // m_macroAccessor=macroAccessor;
  // m_microAccessor=microAccessor;
}

//************************************************************************
// search function
//************************************************************************
template <typename TargetType>
uint64_t TransitionSearchStructure<TargetType>::
findMatchingTransitions(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                        const LinguisticGraphVertex& vertex,
                        AnalysisContent& analysis,
                        const LinguisticAnalysisStructure::Token* token,
                        const LinguisticAnalysisStructure::MorphoSyntacticData* data,
                        std::vector<const TargetType*>& matchingTransitions) const
{
#ifdef DEBUG_LP
   AULOGINIT;
   LDEBUG << "TransitionSearchStructure::findMatchingTransitions from vertex " << vertex;
#endif
  auto metadata = std::dynamic_pointer_cast<LinguisticMetaData>(analysis.getData("LinguisticMetaData"));
  if (metadata == nullptr) {
    LERROR << "TransitionSearchStructure::findMatchingTransitions no LinguisticMetaData ! abort";
      return 0;
  }
  auto& propertyCodeManager = static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(metadata->getMetaData("Lang"))).getPropertyCodeManager();
  auto& macroAccessor = propertyCodeManager.getPropertyAccessor("MACRO");
  auto& microAccessor = propertyCodeManager.getPropertyAccessor("MICRO");

  matchingTransitions.clear();

  if (! m_wordMap.empty()) {
    // get words in token
    std::pair<typename WordMap::const_iterator,typename WordMap::const_iterator>
      wordRange=m_wordMap.equal_range(token->form());
    for (; wordRange.first!=wordRange.second; wordRange.first++) {
      matchingTransitions.push_back(wordRange.first->second);
    }
    LinguisticAnalysisStructure::MorphoSyntacticData::const_iterator
      it=data->begin(),
      it_end=data->end();
    StringsPoolIndex current=static_cast<StringsPoolIndex>(0);
    for (; it!=it_end; it++) {
      if (it->inflectedForm == current) continue;
      current=it->inflectedForm;
      wordRange=m_wordMap.equal_range(current);
      for (; wordRange.first!=wordRange.second; wordRange.first++) {
        matchingTransitions.push_back(wordRange.first->second);
      }
    }
  }

  if (! m_posMap.empty()) {
    // get pos in data
    LinguisticAnalysisStructure::MorphoSyntacticData::const_iterator
      it=data->begin(),
      it_end=data->end();
    for (; it!=it_end; it++) {

      // match on macro+micro
      Tpos pos(microAccessor.readValue((*it).properties));

      std::pair<typename PosMap::const_iterator,typename PosMap::const_iterator>
        posRange=m_posMap.equal_range(pos);
      for (; posRange.first!=posRange.second; posRange.first++) {
        matchingTransitions.push_back(posRange.first->second);
      }

      // match on macro only
      Tpos posMacro(macroAccessor.readValue((*it).properties));
      posRange=m_posMap.equal_range(posMacro);
      for (; posRange.first!=posRange.second; posRange.first++) {
        matchingTransitions.push_back(posRange.first->second);
      }
    }
  }

  if (! m_lemmaMap.empty()) {
    LinguisticAnalysisStructure::MorphoSyntacticData::const_iterator
      it=data->begin(),
      it_end=data->end();
    for (; it!=it_end; it++) {

      // match on macro+micro
      Tpos pos(microAccessor.readValue((*it).properties));

      std::pair<typename LemmaMap::const_iterator,typename LemmaMap::const_iterator>
        lemmaRange=m_lemmaMap.equal_range(std::make_pair((*it).lemma,pos));
      for (; lemmaRange.first!=lemmaRange.second; lemmaRange.first++) {
        matchingTransitions.push_back(lemmaRange.first->second);
      }

      // match on macro only
      Tpos posMacro(macroAccessor.readValue((*it).properties));
      lemmaRange=m_lemmaMap.equal_range(std::make_pair((*it).lemma,posMacro));
      for (; lemmaRange.first!=lemmaRange.second; lemmaRange.first++) {
        matchingTransitions.push_back(lemmaRange.first->second);
      }
    }
  }

  if (! m_tstatusMap.empty()) {
    const LinguisticAnalysisStructure::TStatus& tstatus=token->status();
/*    if (tstatus==0) {
      AULOGINIT;
      LWARN << "no tstatus found for fulltoken "
        << token->stringForm();
    }
    else {*/
      std::pair<typename TstatusMap::const_iterator,typename TstatusMap::const_iterator>
        tstatusRange=m_tstatusMap.equal_range(tstatus);

      for (; tstatusRange.first!=tstatusRange.second; tstatusRange.first++) {
        matchingTransitions.push_back(tstatusRange.first->second);
      }
//     }
  }

  if (! m_otherTransitions.empty()) {
    typename TransitionList::const_iterator
      otherTransition=m_otherTransitions.begin(),
      otherTransition_end=m_otherTransitions.end();
    for (; otherTransition!=otherTransition_end; otherTransition++) {
      bool match=(*otherTransition).first->compare(graph,vertex,analysis,token,data);
      if ((*otherTransition).first->negative()) {
        match = (!match);
      }
      if (match) {
        matchingTransitions.push_back((*otherTransition).second);
      }
    }
  }

  return matchingTransitions.size();
}

template <typename TargetType>
uint64_t TransitionSearchStructure<TargetType>::
    findMatchingTransitions2(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                            const LinguisticGraphVertex& vertex,
                            const LinguisticGraphVertex& limit,
                            const SearchGraph* searchGraph,
                            AnalysisContent& analysis,
                            const LinguisticAnalysisStructure::Token* token,
                            const LinguisticAnalysisStructure::MorphoSyntacticData* data,
                            std::vector<std::pair<std::deque<LinguisticGraphVertex>,const TargetType*> >& matchingTransitions) const
{
#ifdef DEBUG_LP
   AULOGINIT;
   LDEBUG << "findMatchingTransitions2 from vertex " << vertex;
#endif
  auto metadata = std::dynamic_pointer_cast<LinguisticMetaData>(analysis.getData("LinguisticMetaData"));
  if (metadata == nullptr) {
    LERROR << "TransitionSearchStructure::findMatchingTransitions no LinguisticMetaData ! abort";
      return 0;
  }
  auto& propertyCodeManager = static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(metadata->getMetaData("Lang"))).getPropertyCodeManager();
  auto& macroAccessor = propertyCodeManager.getPropertyAccessor("MACRO");
  auto& microAccessor = propertyCodeManager.getPropertyAccessor("MICRO");

  if (! m_wordMap.empty()) {
    // get words in token
    std::pair<typename WordMap::const_iterator,typename WordMap::const_iterator>
      wordRange=m_wordMap.equal_range(token->form());
    std::deque<LinguisticGraphVertex> singleton(1,vertex);
    for (; wordRange.first!=wordRange.second; wordRange.first++) {
      matchingTransitions.push_back(std::pair<std::deque<LinguisticGraphVertex>,const TargetType* >(singleton,wordRange.first->second));
    }
    LinguisticAnalysisStructure::MorphoSyntacticData::const_iterator
      it=data->begin(),
      it_end=data->end();
    StringsPoolIndex current=static_cast<StringsPoolIndex>(0);
    for (; it!=it_end; it++) {
      if (it->inflectedForm == current) continue;
      current=it->inflectedForm;
      wordRange=m_wordMap.equal_range(current);
      std::deque<LinguisticGraphVertex> singleton(1,vertex);

      for (; wordRange.first!=wordRange.second; wordRange.first++) {
        matchingTransitions.push_back(std::pair<std::deque<LinguisticGraphVertex>,const TargetType* >(singleton,wordRange.first->second));
      }
    }
  }

  if (! m_posMap.empty()) {
    // get pos in data
    LinguisticAnalysisStructure::MorphoSyntacticData::const_iterator
      it=data->begin(),
      it_end=data->end();
    std::deque<LinguisticGraphVertex> singleton(1,vertex);
    for (; it!=it_end; it++) {

      // match on macro+micro
      Tpos pos(microAccessor.readValue((*it).properties));

      std::pair<typename PosMap::const_iterator,typename PosMap::const_iterator>
        posRange=m_posMap.equal_range(pos);
      for (; posRange.first!=posRange.second; posRange.first++) {
        matchingTransitions.push_back(std::pair<std::deque<LinguisticGraphVertex>,const TargetType* >(singleton,posRange.first->second));
      }

      // match on macro only
      Tpos posMacro(macroAccessor.readValue((*it).properties));
      posRange=m_posMap.equal_range(posMacro);
      for (; posRange.first!=posRange.second; posRange.first++) {
        matchingTransitions.push_back(std::pair<std::deque<LinguisticGraphVertex>,const TargetType* >(singleton,posRange.first->second));
      }
    }
  }

  if (! m_lemmaMap.empty()) {
    LinguisticAnalysisStructure::MorphoSyntacticData::const_iterator
      it=data->begin(),
      it_end=data->end();
    std::deque<LinguisticGraphVertex> singleton(1,vertex);
    for (; it!=it_end; it++) {

      // match on macro+micro
      Tpos pos(microAccessor.readValue((*it).properties));

      std::pair<typename LemmaMap::const_iterator,typename LemmaMap::const_iterator>
        lemmaRange=m_lemmaMap.equal_range(std::make_pair((*it).lemma,pos));
      for (; lemmaRange.first!=lemmaRange.second; lemmaRange.first++) {
        matchingTransitions.push_back(std::pair<std::deque<LinguisticGraphVertex>,const TargetType* >(singleton,lemmaRange.first->second));
      }

      // match on macro only
      Tpos posMacro(macroAccessor.readValue((*it).properties));
      lemmaRange=m_lemmaMap.equal_range(std::make_pair((*it).lemma,posMacro));
      for (; lemmaRange.first!=lemmaRange.second; lemmaRange.first++) {
        matchingTransitions.push_back(std::pair<std::deque<LinguisticGraphVertex>,const TargetType* >(singleton,lemmaRange.first->second));
      }
    }
  }

  if (! m_tstatusMap.empty()) {
    std::deque<LinguisticGraphVertex> singleton(1,vertex);
    const LinguisticAnalysisStructure::TStatus& tstatus=token->status();
      std::pair<typename TstatusMap::const_iterator,typename TstatusMap::const_iterator>
        tstatusRange=m_tstatusMap.equal_range(tstatus);

      for (; tstatusRange.first!=tstatusRange.second; tstatusRange.first++) {
        matchingTransitions.push_back(std::pair<std::deque<LinguisticGraphVertex>,const TargetType* >(singleton,tstatusRange.first->second));
      }
  }

  if (! m_otherTransitions.empty()) {
    typename TransitionList::const_iterator
      otherTransition=m_otherTransitions.begin(),
      otherTransition_end=m_otherTransitions.end();
    std::deque<LinguisticGraphVertex> noVertices;
    std::pair<std::deque<LinguisticGraphVertex>,const Transition*>  newPair(noVertices,nullptr);
    for (; otherTransition!=otherTransition_end; otherTransition++) {
      bool match=(*otherTransition).first->compare(graph,vertex,analysis,token,data);
      const GazeteerTransition* gtrans = dynamic_cast<const GazeteerTransition*>((*otherTransition).second);
      if( gtrans != 0 ) {
        std::deque<LinguisticGraphVertex> vertices;
        match = gtrans->matchPath(graph, vertex, limit, searchGraph, analysis, token, vertices, data);
        if( match ) {
          newPair = std::pair<std::deque<LinguisticGraphVertex>,const Transition*>(vertices,(*otherTransition).second);
        }
      }
      else {
        std::deque<LinguisticGraphVertex> singleton(1,vertex);
        newPair = std::pair<std::deque<LinguisticGraphVertex>,const Transition*>(singleton,(*otherTransition).second);
      }
      if ((*otherTransition).first->negative()) {
        match = (!match);
      }
      if (match) {
        matchingTransitions.push_back(newPair);
      }
    }
  }

  return matchingTransitions.size();
}

//**********************************************************************
// output function (for debug puposes)
//**********************************************************************
template <typename TargetType>
void TransitionSearchStructure<TargetType>::
printStructure(std::ostream& os) const
{
  os << "WordMap=(" << std::endl;
  for (typename WordMap::const_iterator elt=m_wordMap.begin();
       elt!=m_wordMap.end(); elt++) {
    os << (*elt).first << "=>" << (*elt).second << std::endl;
  }
  os << ")" << std::endl;
  os << "PosMap=(" << std::endl;
  for (typename PosMap::const_iterator elt=m_posMap.begin();
       elt!=m_posMap.end(); elt++) {
    os << (*elt).first << "=>" << (*elt).second << std::endl;
  }
  os << ")" << std::endl;
  os << "LemmaMap=(" << std::endl;
  for (typename LemmaMap::const_iterator elt=m_lemmaMap.begin();
       elt!=m_lemmaMap.end(); elt++) {
    os << (*elt).first << "=>" << (*elt).second << std::endl;
  }
  os << ")" << std::endl;
  os << "TstatusMap=" << std::endl;
  for (typename TstatusMap::const_iterator elt=m_tstatusMap.begin();
       elt!=m_tstatusMap.end(); elt++) {
    os << (*elt).first << "=>" << (*elt).second << std::endl;
  }
  os << ")" << std::endl;
  os << "Other=" << std::endl;
  for (typename TransitionList::const_iterator
         elt=m_otherTransitions.begin();
       elt!=m_otherTransitions.end(); elt++) {
    os << (*elt).first << "=>" << (*elt).second << std::endl;
  }
}

} // end namespace
} // end namespace
} // end namespace
