// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       setTransition.h
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Fri Sept 19 2003
 * copyright   Copyright (C) 2003 by CEA LIST
 * Project     Automaton
 * 
 * @brief      representation of set transitions : the transitions are 
 * a set of simple words (for better search in gazeteers)
 * 
 ***********************************************************************/

#ifndef SETTRANSITION_H
#define SETTRANSITION_H

#include "AutomatonExport.h"
#include "transitionUnit.h"
#include "automatonCommon.h"
#include <set>

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

class LIMA_AUTOMATON_EXPORT SetTransition : public TransitionUnit
{
 public:
  SetTransition(); 
  SetTransition(const std::set<Tword>& words, bool keep=true); 
  SetTransition(const SetTransition&);
  virtual ~SetTransition();
  SetTransition& operator = (const SetTransition&);

  SetTransition* clone() const override;
  SetTransition* create() const override;

  std::string printValue() const override;
  bool operator== (const TransitionUnit&) const override;
  
  const std::set<Tword>& getWords() const { return m_words; }
  std::set<Tword>& getWords() { return m_words; }
  
  bool compare(const LinguisticAnalysisStructure::AnalysisGraph& graph,
               const LinguisticGraphVertex& vertex,
               AnalysisContent& analysis,
               const LinguisticAnalysisStructure::Token* token,
               const LinguisticAnalysisStructure::MorphoSyntacticData* data) const override;

  TypeTransition type() const override;


 private:
  std::set<Tword> m_words;

  // helper functions for destructor/copy constructor/assignement operator
  void clean();
  void copy(const SetTransition& t);

};

/***********************************************************************/
// inline functions
/***********************************************************************/
inline TypeTransition SetTransition::type() const { return T_SET; }
inline SetTransition* SetTransition::clone() const { 
  return new SetTransition(*this); }
inline SetTransition* SetTransition::create() const { 
  return new SetTransition(); }

} // namespace end
} // namespace end
} // namespace end

#endif
