/*
    Copyright 2002-2018 CEA LIST

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
/******************************************************************************
*
* File        : transition.cpp
* Author      : Romaric Besan�on (besanconr@zoe.cea.fr)
* Created on  : Fri Oct 04 2002
* Copyright   : (c) 2002 by CEA
* Version     : $Id$
*
******************************************************************************/


#include "transition.h"
#include <iostream>

using namespace std;

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

/***********************************************************************/
// constructors
/***********************************************************************/
Transition::Transition():
  m_unit(0),
  m_nextState(0)
{}

Transition::Transition(TransitionUnit* unit, Tstate state):
  m_unit(unit),
  m_nextState(state)
{}

Transition::Transition(const Transition& t)
{
  copy(t);
}

/***********************************************************************/
// desctructor
/***********************************************************************/
Transition::~Transition() {
  freeMem();
}

/***********************************************************************/
// assignment operator
/***********************************************************************/
Transition& Transition::operator = (const Transition& t) {
  if (this != &t) {
    freeMem();
    copy(t);
  }
  return *this;
}

/***********************************************************************/
// helper functions for constructors and assignment operator
/***********************************************************************/
void Transition::freeMem() {
  if (m_unit != 0) {
    delete m_unit;
  }
  m_unit = 0;
}

void Transition::copy(const Transition& t) {
  m_unit = t.transitionUnit()->clone();
  m_nextState = t.nextState();
}

/***********************************************************************/
// output
/***********************************************************************/

ostream& operator << (ostream& os, const Transition& t) {
  return os << "[" << *(t.transitionUnit()) << "] -> " << t.nextState();
}

QDebug& operator << (QDebug& os, const Transition& t) {
  return os << "[" << *(t.transitionUnit()) << "] -> " << t.nextState();
}

} // namespace end
} // namespace end
} // namespace end
