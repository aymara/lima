// Copyright 2002-2018 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       transition.h
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Fri Oct 04 2002
 * copyright   Copyright (C) 2002 by CEA LIST
 * Project     Automaton
 *
 * @brief      transition composed of a transition Unit and the next state
 * in the automaton
 *
 ***********************************************************************/

#ifndef TRANSITION_H
#define TRANSITION_H

#include "AutomatonExport.h"
#include <string>
#include "automatonCommon.h"
#include "transitionUnit.h"

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

class LIMA_AUTOMATON_EXPORT Transition
{
 public:
  Transition();
  Transition(TransitionUnit*, Tstate);
  Transition(const Transition& transition);
  virtual ~Transition();
  Transition& operator = (const Transition& transition);

  TransitionUnit* transitionUnit() const;
  void setTransitionUnit(TransitionUnit*);
  Tstate nextState() const;
  void setNextState(Tstate);

  friend LIMA_AUTOMATON_EXPORT std::ostream& operator << (std::ostream& os, const Transition& t);
  friend LIMA_AUTOMATON_EXPORT QDebug& operator << (QDebug& os, const Transition& t);

 private:
  TransitionUnit* m_unit;
  Tstate m_nextState;

  /***********************************************************************/
  // helper functions for constructors and destructors
  void freeMem();
  void copy(const Transition& t);

};

/***********************************************************************/
// inline access functions
/***********************************************************************/
inline TransitionUnit* Transition::transitionUnit() const { return m_unit; }
inline void Transition::setTransitionUnit(TransitionUnit *t) { m_unit = t; }

inline Tstate Transition::nextState() const { return m_nextState; }
inline void Transition::setNextState(Tstate state) { m_nextState = state; }

} // namespace end
} // namespace end
} // namespace end

#endif
