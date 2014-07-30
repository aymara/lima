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
* File        : constraint.cpp
* Author      : Besancon Romaric (besanconr@zoe.cea.fr)
* Created on  : Tue Nov 26 2002
* Copyright   : (c) 2002 by CEA
* Version     : $Id$
*
************************************************************************/

#include "constraint.h"
#include "constraintFunctionManager.h"
#include "transitionUnit.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "common/Data/strwstrtools.h"
#include <iostream>

using namespace std;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

/***********************************************************************/
// defines
/***********************************************************************/
//internal text format of constraints
#define CHAR_SEP_CONSTRAINT_INTERNAL '/'
#define CHAR_BEGIN_CONSTRAINT_INTERNAL '+'
#define CHAR_NEGATIVE_CONSTRAINT_INTERNAL '!'

/***********************************************************************/
// constructors
/***********************************************************************/
Constraint::Constraint():
m_index(0),
m_functionAddr(0),
m_action(NONE),
m_negative(false)
{
}

Constraint::Constraint(const uint64_t i,
                       const std::string& functionName,
                       const ConstraintAction a,
                       MediaId language,
                       const LimaString& complement,
                       const bool negative):
m_index(i),
m_functionAddr(0),
m_action(a),
m_negative(negative)
{
  m_functionAddr=ConstraintFunctionManager::changeable().
    getConstraintFunction(functionName,language,complement);
}

Constraint::Constraint(const Constraint& c):
m_index(c.m_index),
m_functionAddr(c.m_functionAddr),
m_action(c.m_action),
m_negative(c.m_negative)
{
}

/***********************************************************************/
// destructor
/***********************************************************************/
Constraint::~Constraint() { }

void Constraint::reinit() {
  m_index=0;
  m_functionAddr=0;
  m_action=NONE;
  m_negative=false;
}

/***********************************************************************/
// copy
/***********************************************************************/
Constraint& Constraint::operator = (const Constraint& c) {
  if (this != &c) {
    m_index = c.index();
    m_functionAddr = c.m_functionAddr;
    m_action = c.m_action;
    m_negative = c.m_negative;
  }
  return (*this);
}

std::string Constraint::functionName() const {
  string functionName;
  LimaString complement;

  if (ConstraintFunctionManager::single().
      getFunctionName(m_functionAddr,
                      functionName,
                      complement)) {
    return functionName;
  }
  else {
    AULOGINIT;
    LERROR << "constraint function "
           << m_functionAddr << " not availale";
    return "";
  }
}

/***********************************************************************/
// check constraint according to a checklist
/***********************************************************************/
bool Constraint::
checkConstraint(const AnalysisGraph& graph,
                const LinguisticGraphVertex& vertex,
                AnalysisContent& analysis,
                ConstraintCheckList& constraintCheckList) const
{
/*
  Critical Function : commnet logging message
*/
//  AULOGINIT;

  // if the action is STORE, store the value in the checklist,
  // else compare the value with the value stored in the checklist
  // using the constraint function

//  LDEBUG << checkStringDebug(graph,vertex);

  bool success(false);
  bool compare(false);
  LinguisticGraphVertex
    firstVertex(0),
    secondVertex(vertex);

  // first get the value from the token
  switch(m_action) {
  case STORE: {
    constraintCheckList[m_index].store(vertex);
    return true;
  }
  case PUSH: {
    constraintCheckList[m_index].push(vertex);
    return true;
  }
  case COMPARE: {
    firstVertex=constraintCheckList[m_index].getValueStored();
    compare=true;
    break;
  }
  case COMPARE_REVERSE: {
    // same but reverse order of arguments
    firstVertex=vertex;
    secondVertex=constraintCheckList[m_index].getValueStored();
    compare=true;
    break;
  }
  case COMPARE_STACK: {
    firstVertex=constraintCheckList[m_index].getValueStack();
    compare=true;
    break;
  }
  case COMPARE_STACK_REVERSE: {
    // same but reverse order of arguments
    firstVertex=vertex;
    secondVertex=constraintCheckList[m_index].getValueStack();
    compare=true;
    break;
  }
  case TEST: {
    success=(*m_functionAddr)(graph,vertex,analysis);
    break;
  }
  case EXECUTE_IF_SUCCESS:
  case EXECUTE_IF_SUCCESS_REVERSE:
  case EXECUTE_IF_FAILURE:
  case EXECUTE_IF_FAILURE_REVERSE: {
    AULOGINIT;
    LERROR << "Constraint: cannot call checkConstraint with EXECUTE type"
           << "(function " << functionName() << ")"<< LENDL;
    return false;
  }
  default: {
    AULOGINIT;
    LERROR << "Constraint: no action specified "
           << "(function " << functionName() << ")"<< LENDL;
    return false;
  }
  }

  if (compare) {
    if (firstVertex == ConstraintCheckListElement::novalue ||
        secondVertex == ConstraintCheckListElement::novalue) {
      success=false;
    }
    else {
      success=(*m_functionAddr)(graph,firstVertex,secondVertex,analysis);
    }
  }

//  LDEBUG << " -> " << success << " => "
//         << (m_negative?(!success):success) <<  LENDL;
  return (m_negative?(!success):success);
}

bool Constraint::apply(const AnalysisGraph& graph,
                       AnalysisContent& analysis,
                       ConstraintCheckList& constraintCheckList,
                       const bool success,
                       RecognizerMatch* result) const
{
/*
  Critical function : comment logging messages
*/
//   bool reverse(false);
  switch (m_action) {
  case EXECUTE_IF_FAILURE: {
    if (success) { return true; } // not applied but normal behaviour
    break;
  }
  case EXECUTE_IF_FAILURE_REVERSE: {
    if (success) { return true; }
//     reverse=true;
    break;
  }
  case EXECUTE_IF_SUCCESS: {
    if (! success) { return true; }
    break;
  }
  case EXECUTE_IF_SUCCESS_REVERSE: {
    if (! success) { return true; }
//     reverse=true;
    break;
  }
  default: {
    AULOGINIT;
    LERROR << "cannot apply an action if not type EXECUTE "
           << "(function " << functionName()
           << ")"<< LENDL;
    return false;
  }
  }

// AULOGINIT;
// LDEBUG << "case passed";
  bool res(false);
  if (m_index==Constraint::noindex) { // no argument
//    LDEBUG << applyStringDebug();

    if ((*m_functionAddr).actionNeedsRecognizedExpression()) {
      res=(*m_functionAddr)(*result,analysis);
    }
    else {
      res=(*m_functionAddr)(analysis);
    }
  }
  else {
    LinguisticGraphVertex firstArg=
      constraintCheckList[m_index].getValueStack();

    if (firstArg == ConstraintCheckListElement::novalue) {
//      LDEBUG << "Action: first argument is not set";
      res=false;
    }
    else {
      constraintCheckList[m_index].pop();
      if (constraintCheckList[m_index].empty()) { // only one argument
//        LDEBUG << applyStringDebug(firstArg);
        res=(*m_functionAddr)(graph,
                              firstArg,
                              analysis);
      }
      else {
        LinguisticGraphVertex secondArg=
          constraintCheckList[m_index].getValueStack();

        if (secondArg == ConstraintCheckListElement::novalue) {
//          LDEBUG << "Action: second argument is not set";
          res=false;
        }
        else {
//          LDEBUG << applyStringDebug(firstArg,secondArg);
          res=(*m_functionAddr)(graph,
                                firstArg,
                                secondArg,
                                analysis);
        }
      }
    }
  }
  //LDEBUG << " -> " << res << " => " << (m_negative?(!res):res) <<  LENDL;
  return (m_negative?(!res):res);
}

/***********************************************************************/
// equality test between constraints
/***********************************************************************/
bool operator == (const Constraint& c1,const Constraint& c2) {
  if (c1.m_index  != c2.m_index)  { return false; }
  if (c1.m_functionAddr != c2.m_functionAddr)   { return false; }
  if (c1.m_action != c2.m_action) { return false; }
  if (c1.m_negative != c2.m_negative) { return false; }
  return true;
}

/***********************************************************************/
// output
/***********************************************************************/
LimaString Constraint::str() const {
  string functionName;
  LimaString complement;
  if (! ConstraintFunctionManager::single().
      getFunctionName(m_functionAddr,
                      functionName,
                      complement)) {
    AULOGINIT;
    LERROR << "constraint function "
           << m_functionAddr << " not availale";
  }

  ostringstream oss;
  oss << (unsigned char)CHAR_BEGIN_CONSTRAINT_INTERNAL << m_index
      << (unsigned char)CHAR_SEP_CONSTRAINT_INTERNAL
      << static_cast<uint64_t>(m_action)
      << (unsigned char)CHAR_SEP_CONSTRAINT_INTERNAL;
  if (m_negative) {
    oss << CHAR_NEGATIVE_CONSTRAINT_INTERNAL;
  }
  oss << functionName;
  if (! complement.isEmpty()) {
    oss << (unsigned char)CHAR_SEP_CONSTRAINT_INTERNAL 
      << Common::Misc::limastring2utf8stdstring(complement);
  }
  return Common::Misc::utf8stdstring2limastring(oss.str());
}

std::string Constraint::
checkStringDebug(const AnalysisGraph& graph,
                 const LinguisticGraphVertex vertex) const {
  string functionName;
  LimaString complement;

  if (! ConstraintFunctionManager::single().
      getFunctionName(m_functionAddr,
                      functionName,
                      complement)) {
    AULOGINIT;
    LERROR << "constraint function "
           << m_functionAddr << " not availale";
  }

  ostringstream oss;
  oss << "Constraint:" << actionString()
      << " vertex " << vertex
      << " (" << Common::Misc::limastring2utf8stdstring((get(vertex_token,*(graph.getGraph()),vertex))->stringForm()) << ")"
      << " in constraint " << m_index
      << ",compl=" 
      << Common::Misc::limastring2utf8stdstring(complement)
      << "\", using function " << functionName;
  return oss.str();
}

std::string Constraint::
applyStringDebug(const LinguisticGraphVertex firstArg,
                 const LinguisticGraphVertex secondArg) const {
  string functionName;
  LimaString complement;

  if (! ConstraintFunctionManager::single().
      getFunctionName(m_functionAddr,
                      functionName,
                      complement)) {
    AULOGINIT;
    LERROR << "constraint function "
           << m_functionAddr << " not availale";
  }

  ostringstream oss;
  oss << "Constraint: executing action "
      << functionName ;
  if (firstArg!=0 && secondArg!=0) {
    oss << " on vertices "<< firstArg
        << "," << secondArg;
  }
  else if (firstArg!=0) {
    oss << " on vertex " << firstArg;
  }
  else {
    oss << " (no args)";
  }
  oss << " with complement=" << Common::Misc::limastring2utf8stdstring(complement);
  return oss.str();
}

std::string Constraint::
actionString() const {
  switch(m_action) {
  case NONE: return "NONE";
  case STORE: return "STORE";
  case COMPARE: return "COMPARE";
  case PUSH: return "PUSH";
  case COMPARE_STACK: return "COMPARE_STACK";
  case TEST: return "TEST";
  case COMPARE_REVERSE: return "COMPARE_REVERSE";
  case COMPARE_STACK_REVERSE: return "COMPARE_STACK_REVERSE";
  case PUSH_EXECUTE: return "PUSH_EXECUTE";
  case EXECUTE_IF_SUCCESS: return "EXECUTE_IF_SUCCESS";
  case EXECUTE_IF_SUCCESS_REVERSE: return "EXECUTE_IF_SUCCESS_REVERSE";
  case EXECUTE_IF_FAILURE: return "EXECUTE_IF_FAILURE";
  case EXECUTE_IF_FAILURE_REVERSE: return "EXECUTE_IF_FAILURE_REVERSE";
  }
  return "UNKNOWN_ACTION";
}


ostream& operator << (ostream& os, const Constraint& c) {
  string functionName;
  LimaString complement;
  if (! ConstraintFunctionManager::single().
    getFunctionName(c.m_functionAddr,
                    functionName,
                    complement)) {
    AULOGINIT;
  LERROR << "constraint function "
  << c.m_functionAddr << " not availale";
                    }
                    switch (c.action()) {
                      case EXECUTE_IF_SUCCESS:
                      case EXECUTE_IF_SUCCESS_REVERSE: os << "=>"; break;
                      case EXECUTE_IF_FAILURE:
                      case EXECUTE_IF_FAILURE_REVERSE: os << "=<"; break;
                      default: os << "+"; break;
                    }
                    if (c.index() == Constraint::noindex) {
                      os << "[";
                    }
                    else {
                      os << "[" << c.index()<< ",";
                    }
                    if (c.m_negative) { os << "!"; }
                    os << functionName << ","
                    << c.actionString();
                    if (! complement.isEmpty()) {
                      os << "," << Common::Misc::limastring2utf8stdstring(complement);
                    }
                    os << "]";
                    return os;
}

QDebug& operator << (QDebug& os, const Constraint& c) {
  string functionName;
  LimaString complement;
  if (! ConstraintFunctionManager::single().
    getFunctionName(c.m_functionAddr,
                    functionName,
                    complement)) {
    AULOGINIT;
  LERROR << "constraint function "
  << c.m_functionAddr << " not availale";
                    }
                    switch (c.action()) {
                      case EXECUTE_IF_SUCCESS:
                      case EXECUTE_IF_SUCCESS_REVERSE: os << "=>"; break;
                      case EXECUTE_IF_FAILURE:
                      case EXECUTE_IF_FAILURE_REVERSE: os << "=<"; break;
                      default: os << "+"; break;
                    }
                    if (c.index() == Constraint::noindex) {
                      os << "[";
                    }
                    else {
                      os << "[" << c.index()<< ",";
                    }
                    if (c.m_negative) { os << "!"; }
                    os << functionName << ","
                    << c.actionString();
                    if (! complement.isEmpty()) {
                      os << "," << Common::Misc::limastring2utf8stdstring(complement);
                    }
                    os << "]";
                    return os;
}

} // end namespace
} // end namespace
} // end namespace
