// Copyright 2002-2018 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
