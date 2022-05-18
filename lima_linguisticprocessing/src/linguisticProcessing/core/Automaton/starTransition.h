// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       starTransition.h
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Mon Nov 18 2002
 * copyright   Copyright (C) 2003 by CEA LIST
 * Project     Automaton
 * 
 * @brief      representation of *-transition in the automaton (matches 
 *             any token)
 * 
 ***********************************************************************/

#ifndef STARTRANSITION_H
#define STARTRANSITION_H

#include "AutomatonExport.h"
#include "transitionUnit.h"

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

class LIMA_AUTOMATON_EXPORT StarTransition : public TransitionUnit
{
 public:
  StarTransition();
  StarTransition(const bool keep);
  StarTransition(const StarTransition&);
  StarTransition(const TransitionUnit&); // reduce any transition to star transition
  virtual ~StarTransition();
  StarTransition& operator = (const StarTransition&);
  
  StarTransition* clone() const override;
  StarTransition* create() const override;

  std::string printValue() const override;
  bool operator== (const TransitionUnit&) const override;

  bool compare(const LinguisticAnalysisStructure::AnalysisGraph& graph,
               const LinguisticGraphVertex& vertex,
               AnalysisContent& analysis,
               const LinguisticAnalysisStructure::Token* token,
               const LinguisticAnalysisStructure::MorphoSyntacticData* data) const override;

  TypeTransition type() const override;

 private:
  
};

/***********************************************************************/
// inline functions
/***********************************************************************/
inline StarTransition* StarTransition::clone() const { 
  return new StarTransition(*this); }
inline StarTransition* StarTransition::create() const { 
  return new StarTransition(); }
inline TypeTransition StarTransition::type() const { return T_STAR; }


} // namespace end
} // namespace end
} // namespace end

#endif
