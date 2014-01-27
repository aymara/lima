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
 * @file       constraintCheckList.h
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Thu Sep  4 2003
 * copyright   Copyright (C) 2003 by CEA LIST
 * Project     Automaton
 * 
 * @brief      this class contains the structure that allow
 * constraint checks
 * 
 * 
 ***********************************************************************/

#ifndef CONSTRAINTCHECKLIST_H
#define CONSTRAINTCHECKLIST_H

#include "AutomatonExport.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include <iostream>
#include <string>
#include <stack>

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

typedef enum {
  NONE,
  STORE,
  COMPARE,
  PUSH,
  COMPARE_STACK,
  TEST,
  COMPARE_REVERSE,
  COMPARE_STACK_REVERSE,
  PUSH_EXECUTE,
  EXECUTE_IF_SUCCESS,
  EXECUTE_IF_SUCCESS_REVERSE,
  EXECUTE_IF_FAILURE,
  EXECUTE_IF_FAILURE_REVERSE
} ConstraintAction;


class LIMA_AUTOMATON_EXPORT ConstraintCheckListElement
{
 public:
  ConstraintCheckListElement(); 
  ConstraintCheckListElement(const LinguisticAnalysisStructure::AnalysisGraph& graph);
  ConstraintCheckListElement(const ConstraintCheckListElement&);
  ~ConstraintCheckListElement();
  ConstraintCheckListElement& operator = (const ConstraintCheckListElement&);
  
  // comparison operators
  bool operator == (const ConstraintCheckListElement&) const;
  bool operator<(const ConstraintCheckListElement&) const;

  friend std::ostream& operator << (std::ostream&, const ConstraintCheckListElement&);

  LinguisticGraphVertex getValueStored() const;
  LinguisticGraphVertex getValueStack() const;

  void store(const LinguisticGraphVertex& v);
  void push(const LinguisticGraphVertex& v);
  void pop();
  bool empty() const;

  static const uint64_t novalue;

 private:
  LinguisticGraphVertex m_stored;
  std::stack<LinguisticGraphVertex,std::vector<LinguisticGraphVertex> > m_stack;
};
//**********************************************************************
// inline functions
//**********************************************************************
inline LinguisticGraphVertex ConstraintCheckListElement::getValueStored() const {
  return m_stored;
}
inline LinguisticGraphVertex ConstraintCheckListElement::getValueStack() const {
  return m_stack.top();
}
inline void ConstraintCheckListElement::pop() {
  return m_stack.pop();
}
inline bool ConstraintCheckListElement::empty() const {
  return m_stack.empty();
}

inline void ConstraintCheckListElement::store(const LinguisticGraphVertex& v) {
  m_stored=v;
}
inline void ConstraintCheckListElement::push(const LinguisticGraphVertex& v) {
  m_stack.push(v);
}

//**********************************************************************
// ConstraintCheckList
//**********************************************************************
typedef std::vector<ConstraintCheckListElement> ConstraintCheckList;

LIMA_AUTOMATON_EXPORT std::ostream& operator << (std::ostream&, const ConstraintCheckList&);

} // end namespace
} // end namespace
} // end namespace

#endif
