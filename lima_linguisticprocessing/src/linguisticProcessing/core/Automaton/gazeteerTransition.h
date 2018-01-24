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
 * @file       gazeteerTransition.h
 * @author     Olivier Mesnard (olivier.mesnard@cea.fr)
 * @date       Thu August 04 2015
 * copyright   Copyright (C) 2002-2015 by CEA LIST
 * Project     Automaton
 * 
 * @brief      transitions that are surface form : belongs to a gazeteer
 * 
 ***********************************************************************/

#ifndef GAZETEERTRANSITION_H
#define GAZETEERTRANSITION_H

#include "AutomatonExport.h"
#include "automatonCommon.h"
#include "transitionUnit.h"
#include <deque>
#include "searchGraph.h"

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

class LIMA_AUTOMATON_EXPORT GazeteerTransition : public TransitionUnit
{
 public:
  GazeteerTransition(); 
  GazeteerTransition(const std::vector<LimaString>& wordSet, const LimaString& alias, bool keep=true); 
  GazeteerTransition(const GazeteerTransition&);
  virtual ~GazeteerTransition();
  GazeteerTransition& operator = (const GazeteerTransition&);
  
  GazeteerTransition* clone() const override;
  GazeteerTransition* create() const override;

  std::string printValue() const override;
  bool operator== (const TransitionUnit&) const override;

  bool compare(const LinguisticAnalysisStructure::AnalysisGraph& graph,
               const LinguisticGraphVertex& vertex,
               AnalysisContent& analysis,
               const LinguisticAnalysisStructure::Token* token,
               const LinguisticAnalysisStructure::MorphoSyntacticData* data) const override;
  
  TypeTransition type() const override;
  LimaString alias() const;
  const std::set<LimaString>& wordSet() const;

  bool matchPath(const LinguisticAnalysisStructure::AnalysisGraph& graph,
        const LinguisticGraphVertex& vertex,
        const LinguisticGraphVertex& limit,
        SearchGraph* searchGraph,
        AnalysisContent& analysis,
        const LinguisticAnalysisStructure::Token* token,
        std::deque<LinguisticGraphVertex>& vertices,
        const LinguisticAnalysisStructure::MorphoSyntacticData* ) const;
  
 private:
   bool checkMultiTerms( const LinguisticAnalysisStructure::AnalysisGraph& graph,
             const LinguisticGraphVertex& position,
             const LinguisticGraphVertex& limit,
             SearchGraph* searchGraph,
             AnalysisContent& analysis,
             const std::vector<std::vector<LimaString> >& additionalMultiTermList,
             std::stack<std::deque<LinguisticGraphVertex>,std::vector<std::deque<LinguisticGraphVertex> > >& matches
                           ) const;
                           
   bool buildNextTermsList( const LimaString& firstSimpleTerm, std::vector<std::vector<LimaString> >& multiTermList ) const;
   std::set<LimaString> m_wordSet;
   LimaString m_alias;

};


/***********************************************************************/
// inline access functions
/***********************************************************************/
inline const std::set<LimaString>& GazeteerTransition::wordSet() const { return m_wordSet; }
inline TypeTransition GazeteerTransition::type() const { return T_GAZETEER; }

inline GazeteerTransition* GazeteerTransition::clone() const { 
  return new GazeteerTransition(*this); }
inline GazeteerTransition* GazeteerTransition::create() const {
  return new GazeteerTransition(); }
inline  LimaString GazeteerTransition::alias() const { return m_alias; }


} // namespace end
} // namespace end
} // namespace end

#endif
