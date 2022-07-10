// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       andTransition.h
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Fri Sept 12 2003
 * copyright   Copyright (C) 2004 by CEA LIST
 * Project     Automaton
 * 
 * @brief      representation of AND transitions : the transitions are 
 * a combination of other transitions
 * 
 * 
 ***********************************************************************/

#ifndef ANDTRANSITION_H
#define ANDTRANSITION_H

#include "AutomatonExport.h"
#include "transitionUnit.h"
#include "automatonCommon.h"
#include "common/ProcessUnitFramework/AnalysisContent.h"

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

class LIMA_AUTOMATON_EXPORT AndTransition : public TransitionUnit
{
 public:
  AndTransition(); 

  /** 
   * constructor of andTransition based on a vector of TransitionUnit*
   * BE CAREFUL with this constructor : the pointers in the vectors 
   * are copied without reallocation : deleting pointers after 
   * calling this constructor will cause segmentation faults
   * 
   * @return 
   */
  AndTransition(const std::vector<TransitionUnit*>&, bool keep=true); 
  AndTransition(const Tpos&, bool keep=true); 
  AndTransition(const AndTransition&);
  virtual ~AndTransition();
  AndTransition& operator = (const AndTransition&);

  AndTransition* clone() const override;
  AndTransition* create() const override;

  std::string printValue() const override;
  bool operator== (const TransitionUnit&) const override;

  std::vector<TransitionUnit*>& getTransitions();

  bool compare(const LinguisticAnalysisStructure::AnalysisGraph& graph,
               const LinguisticGraphVertex& vertex,
               AnalysisContent& analysis,
               const LinguisticAnalysisStructure::Token* token,
               const LinguisticAnalysisStructure::MorphoSyntacticData* data) const override;
  
  TypeTransition type() const override;

  const std::vector<TransitionUnit*>& getTransitions() const
    { return m_transitions; }

 private:
  std::vector<TransitionUnit*> m_transitions;

  // helper functions for destructor/copy constructor/assignement operator
  void clean();
  void copy(const AndTransition& t);

};

/***********************************************************************/
// inline access functions
/***********************************************************************/
inline TypeTransition AndTransition::type() const { return T_AND; }
inline std::vector<TransitionUnit*>& AndTransition::getTransitions() {
  return m_transitions;
}

inline AndTransition* AndTransition::clone() const { 
  return new AndTransition(*this); }
inline AndTransition* AndTransition::create() const { 
  return new AndTransition(); }

} // namespace end
} // namespace end
} // namespace end

#endif
