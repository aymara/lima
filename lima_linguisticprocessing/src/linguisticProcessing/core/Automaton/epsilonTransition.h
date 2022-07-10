// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
  
  EpsilonTransition* clone() const override;
  EpsilonTransition* create() const override;

  std::string printValue() const override;
  bool operator== (const TransitionUnit&) const override;

  bool compare(const LinguisticAnalysisStructure::AnalysisGraph& graph,
               const LinguisticGraphVertex& vertex,
               AnalysisContent& analysis,
               const LinguisticAnalysisStructure::Token* token,
               const LinguisticAnalysisStructure::MorphoSyntacticData* data) const override;

  bool isEpsilonTransition() const override { return true; }
  TypeTransition type() const override;

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
