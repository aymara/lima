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
* File        : rule.cpp
* Author      : Romaric Besan�n (besanconr@zoe.cea.fr)
* Created on  : Tue Oct 15 2002
* Copyright   : (c) 2002 by CEA
* Version     : $Id$
*
*************************************************************************/

#include "rule.h"
#include "automaton.h"
#include "common/MediaticData/mediaticData.h"
#include<iostream>
#include<string>

using namespace std;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

/***********************************************************************/
// constructors
/***********************************************************************/
Rule::Rule():
  m_trigger(0),
  m_left(),
  m_right(),
  m_type(),
  m_lingProp(0),
  m_normalizedForm(),
  m_numberOfConstraints(0),
  m_contextual(true),
  m_negative(false),
  m_hasLeftRightConstraint(false),
  m_actions(),
  m_weight(0.0),
  m_ruleId("")
{
}

Rule::Rule(const Rule& r):
  m_trigger(r.m_trigger->clone()),
  m_left(r.m_left),
  m_right(r.m_right),
  m_type(r.m_type),
  m_lingProp(r.m_lingProp),
  m_normalizedForm(r.m_normalizedForm),
  m_numberOfConstraints(r.m_numberOfConstraints),
  m_contextual(r.m_contextual),
  m_negative(r.m_negative),
  m_hasLeftRightConstraint(r.m_hasLeftRightConstraint),
  m_actions(r.m_actions),
  m_weight(r.m_weight),
  m_ruleId(r.m_ruleId)
{
}

// read a rule from a text entry
// Rule::Rule(const LimaString& s, Tchar sep) {
// //   std::cerr << "building rule from string " << s << endl;
//   *this=RuleCompiler::buildRule(s,sep);
// }

/***********************************************************************/
// destructor
/***********************************************************************/
Rule::~Rule() {
  freeMem();
}

void Rule::reinit() {
  m_trigger=0;
  m_left.reinit();
  m_right.reinit();
  m_type=Common::MediaticData::EntityType();
  m_lingProp=0;
  m_normalizedForm.clear();
  m_numberOfConstraints=0;
  m_contextual=true;
  m_negative=false;
  m_hasLeftRightConstraint=false;
  m_actions.clear();
  m_weight=0.0;
  m_ruleId="";
}

/***********************************************************************/
// copy
/***********************************************************************/
Rule& Rule::operator = (const Rule& r) {
  if (this != &r) {
    freeMem();
    init();
    copy(r);
  }
  return (*this);
}

bool Rule::operator<(const Rule& r) {
  return (m_weight<r.m_weight);
}

//**********************************************************************
// helper functions for constructors and destructors
void Rule::init()
{
  m_trigger=0;
  m_left=Automaton();
  m_right=Automaton();
  m_type=Common::MediaticData::EntityType();
  m_lingProp=0;
  m_normalizedForm.clear();
  m_numberOfConstraints=0;
  m_contextual=true;
  m_negative=false;
  m_hasLeftRightConstraint=false;
  m_actions=std::vector<Constraint>();
  m_weight=0.0;
  m_ruleId="";
}

void Rule::copy(const Rule& r)
{
  m_trigger = r.getTrigger()->clone();
  m_left = r.leftAutomaton();
  m_right = r.rightAutomaton();
  m_type = r.getType();
  m_lingProp = r.getLinguisticProperties();
  m_normalizedForm = r.getNormalizedForm();
  m_numberOfConstraints = r.numberOfConstraints();
  m_contextual = r.contextual();
  m_negative = r.negative();
  m_hasLeftRightConstraint = r.m_hasLeftRightConstraint;
  m_actions = r.getActions();
  m_weight = r.m_weight;
  m_ruleId = r.m_ruleId;
}

void Rule::freeMem()
{
  if (m_trigger != 0) {
    delete m_trigger;
  }
  m_trigger=0;
}

//**********************************************************************
// test the rule on a graph
//**********************************************************************
bool Rule::test(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                const LinguisticGraphVertex& leftContext,
                const LinguisticGraphVertex& rightContext,
                const LinguisticGraphVertex& leftLimit,
                const LinguisticGraphVertex& rightLimit,
                AnalysisContent& analysis,
                RecognizerMatch& leftmatch,
                RecognizerMatch& rightmatch,
                ConstraintCheckList& constraintCheckList,
                ForwardSearch& forward,
                BackwardSearch& backward,
                const AutomatonControlParams& controlParams) const {

  if (m_hasLeftRightConstraint) {

    // do not need to get all matches from left part:
    // matches on left and right parts are independant
    if (! m_left.getBestMatch(graph, leftContext, leftLimit,
                              analysis,leftmatch,
                              constraintCheckList,
                              BACKWARDSEARCH, controlParams)) {
/*      AULOGINIT;
      LDEBUG << "no match found on the left part" << LENDL;*/
      return false;
    }
    if (! m_right.getBestMatch(graph, rightContext, rightLimit,
                               analysis,rightmatch,
                               constraintCheckList,
                               FORWARDSEARCH, controlParams)) {
/*      AULOGINIT;
      LDEBUG << "no match found on the right part" << LENDL;*/
      return false;
    }

    return true;
  }

  // else has to store all possible left matches to
  // get best right match wrt each left match

  Automaton::AutomatonMatchSet leftMatches;
  if (! m_left.getAllMatches(graph, leftContext, leftLimit,
                             analysis,leftMatches,
                             constraintCheckList,forward,backward,
                             BACKWARDSEARCH, controlParams)) {
/*    AULOGINIT;
    LDEBUG << "no match found on the left part" << LENDL;*/
    return false;
  }
//   AULOGINIT;
//   LDEBUG << "Rule: found " << leftMatches.size()
//          << " matches for left part" << LENDL;

  Automaton::AutomatonMatchSet::const_iterator
    currentLeftMatch=leftMatches.begin(),
    endLeftMatch=leftMatches.end();

  // store checklists (to avoid testing right part with same
  // checklists)
  std::set<ConstraintCheckList> leftCheckLists;

  for (; currentLeftMatch!=endLeftMatch; currentLeftMatch++) {

//     LDEBUG << "Rule: looking at right part with left="
//            << (*currentLeftMatch).first << ",checklist="
//            << (*currentLeftMatch).second << LENDL;

    if (leftCheckLists.find((*currentLeftMatch).second)!=
        leftCheckLists.end()) {
      continue;
    }
    // must copy constraintCheckList to modify it
    // because iterator in set are not mutable
    constraintCheckList=(*currentLeftMatch).second;
    if (m_right.getBestMatch(graph, rightContext, rightLimit,
                             analysis,rightmatch,constraintCheckList,
                             FORWARDSEARCH, controlParams)) {
      leftmatch=(*currentLeftMatch).first;
      // has to reverse left match (to be in natural sense of the graph)
      std::reverse(leftmatch.begin(),leftmatch.end());

      return true;
    }
    leftCheckLists.insert((*currentLeftMatch).second);
  }
  return false;
}

bool Rule::executeActions(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                          AnalysisContent& analysis,
                          ConstraintCheckList& constraintCheckList,
                          const bool success,
                          RecognizerMatch* result) const {

  if (m_actions.empty()) {
    return true;
  }

//  AULOGINIT;
  
  // execute actions associated to the rule
  // even if rule failed (actions are internally conditionned by success)
  for (std::vector<Constraint>::const_iterator action=m_actions.begin();
       action!=m_actions.end(); action++) {
//    LDEBUG << "Applying action " << action->functionName() << LENDL;
    if (! (*action).apply(graph,analysis,
                          constraintCheckList,
                          success,
                          result)) {
//       LDEBUG << "a match was found but an action has not succeeded"
//              << " -> returned false" << LENDL;
      return false;
    }
  }
  return true;
}

//**********************************************************************
// output
ostream& operator << (ostream& os, const Rule& r) {
  os << r.getRuleId() << ":" << endl;
  os << *(r.getTrigger()) << "(w=" << (r.getWeight())<< "):" << endl;
  os << "left=" << endl << r.leftAutomaton();
  os << "right=" << endl << r.rightAutomaton();
  os << r.getType()<< ";" << r.getLinguisticProperties() << endl;
  for (std::vector<Constraint>::const_iterator action=r.m_actions.begin();
       action!=r.m_actions.end(); action++) {
    os << *action << endl;
       }
       return os;
}
QDebug& operator << (QDebug& os, const Rule& r) {
  os << r.getRuleId() << ":" << endl;
  os << *(r.getTrigger()) << "(w=" << (r.getWeight())<< "):" << endl;
  os << "left=" << endl << r.leftAutomaton();
  os << "right=" << endl << r.rightAutomaton();
  os << r.getType()<< ";" << r.getLinguisticProperties() << endl;
  for (std::vector<Constraint>::const_iterator action=r.m_actions.begin();
       action!=r.m_actions.end(); action++) {
    os << *action << endl;
       }
       return os;
}

} // namespace end
} // namespace end
} // namespace end
