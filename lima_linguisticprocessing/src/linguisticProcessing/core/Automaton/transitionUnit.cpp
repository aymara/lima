// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***********************************************************************
*
* File        : transitionUnit.cpp
* Author      : Romaric Besan�on (besanconr@zoe.cea.fr)
* Created on  : Fri Oct 04 2002
* Copyright   : (c) 2002 by CEA
* Version     : $Id$
*
************************************************************************/


#include "transitionUnit.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "common/Data/strwstrtools.h"
#include <iostream>

using namespace std;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

/***********************************************************************/
// constructors
/***********************************************************************/
TransitionUnit::TransitionUnit():
  m_keep(true),
  m_negative(false),
  m_head(false),
  m_actionHash(""),
  m_constraints(vector<Constraint>(0))
{
}

TransitionUnit::TransitionUnit(bool keep, bool negative):
  m_keep(keep),
  m_negative(negative),
  m_head(false),
  m_actionHash(""),
  m_constraints(vector<Constraint>(0)) { 
}

TransitionUnit::TransitionUnit(const TransitionUnit& t):
  m_keep(t.m_keep),
  m_negative(t.m_negative),
  m_head(t.m_head),
  m_id(t.m_id),
  m_actionHash(t.m_actionHash),
  m_constraints(t.m_constraints) {
}

/***********************************************************************/
// destructor
/***********************************************************************/
TransitionUnit::~TransitionUnit() { 
  m_constraints.clear();
}

/***********************************************************************/
// copy
/***********************************************************************/
TransitionUnit& TransitionUnit::operator = (const TransitionUnit& t) {
  if (this != &t) {
    copyProperties(t);
  }
  return *this;
}
  
/***********************************************************************/
// functions to treat common properties to all kind of transitions
/***********************************************************************/
void TransitionUnit::copyProperties(const TransitionUnit& t) {
  setKeep(t.keep());
  setNegative(t.negative());
  setHead(t.head());
  setId(t.getId()),
  setActionHash(t.getActionHash()),
  m_constraints.clear();
  for (uint64_t i(0); i<t.numberOfConstraints(); i++) {
    addConstraint(t.constraint(i));
  }
}

bool TransitionUnit::compareProperties(const TransitionUnit& t) const {
  if (m_id != t.getId()) { return false; }
  if (m_keep != t.keep()) { return false; }
  if (m_negative != t.negative()) { return false; }
  if (m_head != t.head()) { return false; }
  if (numberOfConstraints() != t.numberOfConstraints()) { return false; }
  if( getActionHash() != t.getActionHash()) { return false; }
  for (uint64_t i(0); i<numberOfConstraints(); i++) {
    if (!(constraint(i) == t.constraint(i)) ) { 
      return false; 
    }
  }
  return true;
}

/***********************************************************************/
// check constraints 
/***********************************************************************/
bool TransitionUnit::
checkConstraints(const AnalysisGraph& graph,
                 const LinguisticGraphVertex& vertex, 
                 AnalysisContent& analysis,
                 ConstraintCheckList& checklist) const {
  
  std::vector<Constraint>::const_iterator
    c=m_constraints.begin(),
    c_end=m_constraints.end();
  for (; c!=c_end; c++) {
    if (!(*c).checkConstraint(graph,vertex,analysis,checklist)) {
      return false;
    }
  }
  return true;
}

// if stored values resulting from the constraints are to be
// removed
void TransitionUnit::popConstraints(ConstraintCheckList& checklist) const {
  for (uint64_t i(0); i<numberOfConstraints(); i++) {
    if (m_constraints[i].action() == PUSH) {
      checklist[i].pop();
    }
  }
}

/***********************************************************************/
// output
/***********************************************************************/

ostream& operator << (ostream& os, const TransitionUnit& t)
{
//   AULOGINIT;
//   LTRACE << "TransitionUnit::operator<< on transition of type " << t.type();
  
  if (t.negative()) { os << '^'; }
  if (t.head()) { os << 'H'; }
  os << "'" << t.getId() << "'#" << t.getActionHash();
  if (! t.keep()) {
    os << '_' << t.printValue() << '_';
  }
  else {
    os << t.printValue();
  }
  if (t.numberOfConstraints()) {
    for (uint64_t i(0); i<t.numberOfConstraints(); i++) {
      os << t.constraint(i);
    }
  }
  
  return os;
}

QDebug& operator << (QDebug& os, const TransitionUnit& t)
{
//   AULOGINIT;
//   LTRACE << "TransitionUnit::operator<< on transition of type " << t.type();
  
  if (t.negative()) { os << '^'; }
  if (t.head()) { os << 'H'; }
  os << "'" << t.getId() << "'#" << t.getActionHash();
  if (! t.keep()) {
    os << '_' << t.printValue() << '_';
  }
  else {
    os << t.printValue();
  }
  if (t.numberOfConstraints()) {
    os << "withConstraints:";
    for (uint64_t i(0); i<t.numberOfConstraints(); i++) {
      os << t.constraint(i);
    }
  }
  
  return os;
}

} // namespace end
} // namespace end
} // namespace end
