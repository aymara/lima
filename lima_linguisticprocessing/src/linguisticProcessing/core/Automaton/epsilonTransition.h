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
 * @file       epsilonTransition.h
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Tue Oct 15 2002
 * copyright   Copyright (C) 2002 by CEA LIST
 * Project     Automaton
 * 
 * @brief      representation of epsilon-transitions in the automaton
 * 
 * 
 ***********************************************************************/

#ifndef EPSILONTRANSITION_H
#define EPSILONTRANSITION_H

#include "AutomatonExport.h"
#include "transitionUnit.h"
#include "common/Data/strwstrtools.h"

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

class LIMA_AUTOMATON_EXPORT EpsilonTransition : public TransitionUnit
{
 public:
  EpsilonTransition();
  EpsilonTransition(const EpsilonTransition&);
  virtual ~EpsilonTransition();
  EpsilonTransition& operator = (const EpsilonTransition&);
  
  EpsilonTransition* clone() const;
  EpsilonTransition* create() const;

  std::string printValue() const;
  bool operator== (const TransitionUnit&) const;

  bool compare(const LinguisticAnalysisStructure::AnalysisGraph& graph,
               const LinguisticGraphVertex& vertex,
               AnalysisContent& analysis,
               const LinguisticAnalysisStructure::Token* token,
               const LinguisticAnalysisStructure::MorphoSyntacticData* data) const;

  bool isEpsilonTransition() const { return true; }
  TypeTransition type() const;

 private:
  
};

/***********************************************************************/
// inline functions
/***********************************************************************/
inline EpsilonTransition* EpsilonTransition::clone() const { 
  return new EpsilonTransition(*this); }
inline EpsilonTransition* EpsilonTransition::create() const { 
  return new EpsilonTransition(); }

inline TypeTransition EpsilonTransition::type() const { return T_EPSILON; }


inline std::string EpsilonTransition::printValue() const { 
  return "epsilonT";
} 


} // namespace end
} // namespace end
} // namespace end

#endif
