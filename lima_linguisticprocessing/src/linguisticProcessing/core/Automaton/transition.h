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
