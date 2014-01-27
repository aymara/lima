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
/*************************************************************************
*
* File        : andTransition.cpp
* Author      : Romaric Besan�n (besanconr@zoe.cea.fr)
* Created on  : Fri Sept 12 2003
* Copyright   : (c) 2002 by CEA
* Version     : $Id$
*
*************************************************************************/


#include "andTransition.h"
#include "common/Data/strwstrtools.h"
#include <iostream>
#include <sstream>

using namespace std;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

/***********************************************************************/
// constructors
/***********************************************************************/
AndTransition::AndTransition():TransitionUnit(),m_transitions(0) {}

// CAREFUL: this constructor copies the vector directly, ie copies the
// pointers and not the object : handle with care
AndTransition::AndTransition(const std::vector<TransitionUnit*>& transitions,
                             bool keep):
  TransitionUnit(keep),
  m_transitions(transitions)
{
}


AndTransition::AndTransition(const AndTransition& t):
  TransitionUnit(t),
  m_transitions()
{
  copy(t);
}

/***********************************************************************/
// destructor
/***********************************************************************/
AndTransition::~AndTransition() {
  clean();
}

/***********************************************************************/
// copy
/***********************************************************************/
AndTransition& AndTransition::operator = (const AndTransition& t) {
  if (this != &t) {
    clean();
    copy(t);
    copyProperties(t);
  }
  return *this;
}

//helper functions for copy constructor and assignment operator
void AndTransition::clean() {
  // clean pointers
  for (uint64_t i(0); i<m_transitions.size(); i++) {
    delete m_transitions[i];
  }
  m_transitions.clear();
}

void AndTransition::copy(const AndTransition& t) {
  //copy values, not pointers
  for (uint64_t i(0); i<t.m_transitions.size(); i++) {
    m_transitions.push_back(t.m_transitions[i]->clone());
  }
}

std::string AndTransition::printValue() const {
  ostringstream oss;
  oss << "AND:";
  if (m_transitions.empty()) {
    return oss.str();
  }
  oss << m_transitions[0];
  for (uint64_t i(1); i<m_transitions.size(); i++) {
    oss << ";" << m_transitions[i];
  }
  return oss.str();
}


/***********************************************************************/
// comparison tests
/***********************************************************************/
bool AndTransition::operator== (const TransitionUnit& tright) const {
  if (tright.type() == type()) {
    const AndTransition& other=static_cast<const AndTransition&>(tright);
    if (m_transitions.size() != other.m_transitions.size()) {
      return false;
    }
    for (uint64_t i(0); i<m_transitions.size(); i++) {
      if (! (m_transitions[i] == other.m_transitions[i]) ) {
        return false;
      }
    }
    return compareProperties(tright);
  }
  else {
    return false;
  }
}

// for andTransition, test the AND on same vertex
// !!!!!!!!!! limitations : cannot prevent that matches on different parts
// !!!!!!!!!! of and transition is on different alternatives of the vertex
// !!!!!!!!!! (orthographic alternatives or different categories...)
bool AndTransition::
compare(const LinguisticAnalysisStructure::AnalysisGraph& graph,
        const LinguisticGraphVertex& vertex,
        AnalysisContent& analysis,
        const LinguisticAnalysisStructure::Token* token,
        const LinguisticAnalysisStructure::MorphoSyntacticData* data) const
{
  for (uint64_t i(0); i<m_transitions.size(); i++) {
    if (! m_transitions[i]->compare(graph,vertex,analysis,token,data)) {
      return false;
    }
  }

  return true;
}

} // namespace end
} // namespace end
} // namespace end
