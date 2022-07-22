// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
*
* File        : constraintCheckList.cpp
* Author      : Besancon Romaric (besanconr@zoe.cea.fr)
* Created on  : Thu Sep  4 2003
* Copyright   : (c) 2003 by CEA
* Version     : $Id$
*
************************************************************************/

#include "constraintCheckList.h"

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

const uint64_t ConstraintCheckListElement::novalue=UINT_MAX;

//***********************************************************************
// constructors
//***********************************************************************
ConstraintCheckListElement::ConstraintCheckListElement():
m_stored(ConstraintCheckListElement::novalue),
m_stack()
{
  // push a default value
  m_stack.push(ConstraintCheckListElement::novalue);
}

ConstraintCheckListElement::
ConstraintCheckListElement(const LinguisticAnalysisStructure::AnalysisGraph& /*unused*/):
m_stored(ConstraintCheckListElement::novalue),
m_stack()
{
  // push a default value
  m_stack.push(ConstraintCheckListElement::novalue);
}

ConstraintCheckListElement::
ConstraintCheckListElement(const ConstraintCheckListElement& e):
m_stored(e.m_stored),
m_stack(e.m_stack)
{
}
  
//***********************************************************************
// destructor
//***********************************************************************
ConstraintCheckListElement::~ConstraintCheckListElement() {
}

//***********************************************************************
// assignment operator
//***********************************************************************
ConstraintCheckListElement& ConstraintCheckListElement::
operator = (const ConstraintCheckListElement& e) {
  m_stored=e.m_stored;
  m_stack=e.m_stack;
  return *this;
}

// comparison operators
bool ConstraintCheckListElement::
operator == (const ConstraintCheckListElement& e) const {
  if (m_stored!=ConstraintCheckListElement::novalue) {
    return (m_stored==e.m_stored);
  }
  return (m_stack == e.m_stack);
}

bool ConstraintCheckListElement::
operator<(const ConstraintCheckListElement& e) const {
  if (m_stored!=ConstraintCheckListElement::novalue) {
    return (m_stored<e.m_stored);
  }
  return (m_stack<e.m_stack);
}

//***********************************************************************
// output
//***********************************************************************
std::ostream& operator << (std::ostream& os, 
                           const ConstraintCheckListElement&e ) {
  os << "stored=" << e.getValueStored() 
     << "/" << "top=" << e.getValueStack();
  return os;
}

std::ostream& operator << (std::ostream& os, const ConstraintCheckList& l) {
  ConstraintCheckList::const_iterator
    it=l.begin(),
    it_end=l.end();
  os << "("; 
  uint64_t i=0;
  for (; it!=it_end;it++) {
    os << std::endl << i << " " << *it;
  }
  os << ")";
  return os;
}

} // end namespace
} // end namespace
} // end namespace
