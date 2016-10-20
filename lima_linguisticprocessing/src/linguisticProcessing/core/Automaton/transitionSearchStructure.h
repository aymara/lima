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
/************************************************************************
 *
 * @file       transitionSearchStructure.h
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Mon Oct 11 2004
 * copyright   Copyright (C) 2004 by CEA LIST
 * Project     Automaton
 * 
 * @brief      this class contains a structure designed to optimize 
 * the search for matching transition units with a vertex 
 * in analysis graph
 * 
 * 
 ***********************************************************************/

#ifndef TRANSITIONSEARCHSTRUCTURE_H
#define TRANSITIONSEARCHSTRUCTURE_H

#include "transitionUnit.h"
#include "automatonCommon.h"
#include <queue>
#ifdef ANTINNO_SPECIFIC
#include "searchGraph.h"
#endif

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

// TargetType will be Transition (in automaton) 
// or TriggerRule (in recognizer) : both have 
// access to matching TransitionUnit and to result
// type
template <typename TargetType>
class TransitionSearchStructure
{
 public:
  TransitionSearchStructure();
  ~TransitionSearchStructure();
  
  void init(const std::vector<TargetType>& l,
            const Common::PropertyCode::PropertyAccessor* macroAccessor,
            const Common::PropertyCode::PropertyAccessor* microAccessor);
  void clear();
  void clearMaps();
  bool empty() const;

  uint64_t 
    findMatchingTransitions(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                            const LinguisticGraphVertex& vertex,
                            AnalysisContent& analysis,
                            const LinguisticAnalysisStructure::Token* token,
                            const LinguisticAnalysisStructure::MorphoSyntacticData* data, 
                            std::vector<const TargetType*>& matchingSetOfRules) const;
  uint64_t 
    findMatchingTransitions2(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                            const LinguisticGraphVertex& vertex,
                            const LinguisticGraphVertex& limit,
                            SearchGraph* searchGraph,
                            AnalysisContent& analysis,
                            const LinguisticAnalysisStructure::Token* token,
                            const LinguisticAnalysisStructure::MorphoSyntacticData* data, 
                            std::vector<std::pair<std::deque<LinguisticGraphVertex>,const TargetType*> >& matchingSetOfRules) const;
  
  // for debug only
  void printStructure(std::ostream& os) const;

 private:
  typedef std::multimap<Tword,const TargetType*> WordMap;
  typedef std::multimap<Tpos,const TargetType*> PosMap;
  typedef std::multimap<std::pair<Tword,Tpos>,const TargetType*> LemmaMap;
  typedef std::multimap<LinguisticAnalysisStructure::TStatus,const TargetType*> TstatusMap;
  typedef std::vector<std::pair<TransitionUnit*,const TargetType*> > TransitionList;
  
  WordMap m_wordMap;
  PosMap m_posMap;
  LemmaMap m_lemmaMap;
  TstatusMap m_tstatusMap;
  TransitionList m_otherTransitions;

  // static members for access of macro and macro_micro
  const Common::PropertyCode::PropertyAccessor* m_macroAccessor;
  const Common::PropertyCode::PropertyAccessor* m_microAccessor;
};

} // end namespace
} // end namespace
} // end namespace

#include "transitionSearchStructure.tcc"

#endif
