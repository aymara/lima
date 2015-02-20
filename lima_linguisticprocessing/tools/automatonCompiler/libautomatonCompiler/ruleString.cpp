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
* File        : ruleString.cpp
* Author      : Besancon Romaric (besanconr@zoe.cea.fr)
* Created on  : Mon Sep  8 2003
* Copyright   : (c) 2003 by CEA
* Version     : $Id: ruleString.cpp 8075 2007-04-13 17:02:29Z mesnardo $
*
*************************************************************************/

#include "ruleString.h"
#include "ruleFormat.h"
#include "compilerExceptions.h"
#include "tstring.h"

using namespace std;

namespace Lima {
using namespace Common;
namespace LinguisticProcessing {
namespace Automaton {

//***********************************************************************
// constructors
//***********************************************************************
RuleString::RuleString():
m_trigger(),
m_left(),
m_right(),
m_type(),
m_norm(),
m_nbConstraints(0),
m_hasLeftRightConstraint(false),
m_actions(),
m_actionsWithOneArgument()
{
}

RuleString::RuleString(const RuleString& r):
m_trigger(r.m_trigger),
m_left(r.m_left),
m_right(r.m_right),
m_type(r.m_type),
m_norm(r.m_norm),
m_nbConstraints(r.m_nbConstraints),
m_hasLeftRightConstraint(r.m_hasLeftRightConstraint),
m_actions(r.m_actions),
m_actionsWithOneArgument(r.m_actionsWithOneArgument)
{
}

RuleString::RuleString(const LimaString& str,
                       MediaId language,
                       const std::vector<Gazeteer>& gazeteers,
                       const std::vector<SubAutomaton>& subAutomatons):
m_trigger(),
m_left(),
m_right(),
m_type(),
m_norm(),
m_nbConstraints(0),
m_hasLeftRightConstraint(false),
m_actions(),
m_actionsWithOneArgument()
{
  AUCLOGINIT;
  int position = findSpecialCharacter(str,CHAR_SEP_RULE,0);
  // trigger
  initPart(str.left(position),m_trigger,gazeteers,subAutomatons);

  // left part
  int next = findSpecialCharacter(str,CHAR_SEP_RULE,position+1);
  initPart(str.mid(position+1,next-position-1),m_left,gazeteers,subAutomatons);

  // right part
  position=next;
  next = findSpecialCharacter(str,CHAR_SEP_RULE,position+1);
  initPart(str.mid(position+1,next-position-1),m_right,gazeteers,subAutomatons);

  // type
  position=next;
  next = findSpecialCharacter(str,CHAR_SEP_RULE,position+1);
  m_type=str.mid(position+1,next-position-1);

  // normalized form (do not keep possible constraints at end of string)
  position=next;
  next = findSpecialCharacter(str,CHAR_BEGIN_CONSTRAINT,position+1);
  LDEBUG << "str='" << str
    <<"' position='"<<position<<"' next='"<<next<<"'";
  if (next == -1) {
    next = findSpecialCharacter(str,CHAR_BEGIN_ACTION,position+1);
  }
  if (next == -1) {
    m_norm=str.mid(position+1);
  }
  else {
    m_norm=str.mid(position+1,next-position-1);
  }
  LDEBUG << "norm=" << Common::Misc::limastring2utf8stdstring(m_norm);

  // set some identifier for each element of the rule
  identifyTransition();
  //constraints
  if (next != -1) {
    // need subAutomatons to deal with named sub-indexes
    treatConstraints(str.mid(next),language,subAutomatons);
  }
  
  //simplify automatonStrings (help building minimal automata)
  m_left.removeUnitSequences();
  m_right.removeUnitSequences();

  LDEBUG << "left=" << Common::Misc::limastring2utf8stdstring(m_left.getStringDebug());
  LDEBUG << "right=" << Common::Misc::limastring2utf8stdstring(m_right.getStringDebug());

//   LDEBUG << "RuleString:init:rule=" << getString() << endl;
}

//***********************************************************************
// destructor
//***********************************************************************
RuleString::~RuleString() {
}

//***********************************************************************
// assignment operator
//***********************************************************************
RuleString& RuleString::operator = (const RuleString& r) {
  m_trigger=r.m_trigger;
  m_left=r.m_left;
  m_right=r.m_right;
  m_type=r.m_type;
  m_norm=r.m_norm;
  m_nbConstraints=r.m_nbConstraints;
  m_hasLeftRightConstraint=r.m_hasLeftRightConstraint;
  m_actions=r.m_actions;
  m_actionsWithOneArgument=r.m_actionsWithOneArgument;
  return *this;
}

//***********************************************************************
// initialize the parts of the rule
//***********************************************************************
void
RuleString::initPart(const LimaString& str, AutomatonString& part,
                     const std::vector<Gazeteer>& gazeteers,
                     const std::vector<SubAutomaton>& subAutomatons) {

  part=AutomatonString(str,gazeteers,subAutomatons);
  part.removeArtificialSequences();
  part.propagateProperties();

//   int position(0);
//   int end(0);

//   while (end != -1) {
//     switch (str[position]) {
//     case CHAR_GROUP_OPEN_RE:
//       end = findSpecialCharacter(str,CHAR_GROUP_CLOSE_RE,position+1);
//     case CHAR_NOKEEP_CLOSE_RE:
//       end = findSpecialCharacter(str,CHAR_NOKEEP_CLOSE_RE,position+1);
//     }
//     end = findSpecialCharacter(str,CHAR_SEP_RE,position+1);
//     part.push_back(AutomatonString(LimaString(str,position,end)));
//     position=end+1;
//   }
}

//***********************************************************************
// get the string from the rule
//***********************************************************************
LimaString RuleString::getString() const {
  return ( m_trigger.getString() + CHAR_SEP_RULE +
           m_left.getString() + CHAR_SEP_RULE +
           m_right.getString() + CHAR_SEP_RULE +
           m_type + CHAR_SEP_RULE +
           m_norm );
}


//***********************************************************************
// treat the constraints of the rule
//***********************************************************************
/***********************************************************************/
// treat the possible constraints on a rule
/***********************************************************************/
int RuleString::findNextConstraint(const LimaString& s,
                                            const int pos,
                                            bool& isAction) const {
  isAction=false;
  int nextConstraint(findSpecialCharacter(s,CHAR_BEGIN_CONSTRAINT,pos));

  if (nextConstraint == -1) {
    int nextAction(findSpecialCharacter(s,CHAR_BEGIN_ACTION,pos));
    if (nextAction != -1) {
      isAction=true;
    }
    return nextAction;
  }
  else {
    // check is there is an action before
    // (shouldn't but can't control how people write the rules)
    int nextAction(findSpecialCharacter(s,CHAR_BEGIN_ACTION,pos));
    if (nextAction == -1 || nextConstraint < nextAction) {
      return nextConstraint;
    }
    else {
      isAction=true;
      return nextAction;
    }
  }
}

void RuleString::
treatConstraints(const LimaString& s,
                 MediaId language,
                 const std::vector<SubAutomaton>& subAutomatons)
{
  AUCLOGINIT;
  bool isAction(false);

  int currentConstraint=findNextConstraint(s,0,isAction);

  if (currentConstraint == -1) { // no constraints
    return;
  }

  int nextConstraint;
  while (currentConstraint != -1) {

    bool nextIsAction(false);
    nextConstraint=findNextConstraint(s,currentConstraint+1,nextIsAction);
    LimaString constraint;
    if (nextConstraint == -1) {
      constraint=s.mid(currentConstraint+1);
    }
    else {
      constraint=s.mid(currentConstraint+1,nextConstraint-currentConstraint-1);
    }

    // treats the constraint
    addConstraint(constraint,language,subAutomatons,isAction);

    LDEBUG << "After adding constraint";
    // to the next constraint
    currentConstraint=nextConstraint;
    isAction=nextIsAction;
  }

  LDEBUG << "Before propagate";
  // propagate the constraints to the units
  m_left.propagateConstraints();
  m_right.propagateConstraints();
}

/**
 * ??? return true if the action has been added, false if it has not
 * (not an error, additions could have been made to an old action) ???
 */
void RuleString::addAction(const Constraint& a, const LimaString& argument) {
  AUCLOGINIT;
  LDEBUG << "adding action indexed with" << argument;

  m_actionsWithOneArgument.insert(std::pair<LimaString,Constraint>(argument,a));
}


/**
 * return true if the constraint has been added, false if it has not
 * (not an error, additions could have been made to an old constraint)
 */
bool RuleString::addConstraint(const PartOfRule part,
                               const SubPartIndex& index,
                               Constraint& c) {
  AUCLOGINIT;
  LDEBUG << "adding constraint in " << part << ":" << index;

  switch (part) {
  case TRIGGER: {
    m_trigger.insertConstraintInUnit(c);
    break;
  }
  case LEFT: {
    m_left.insertConstraint(&index,c);
    break;
  }
  case RIGHT: {
    m_right.insertConstraint(&index,c);
    break;
  }
  } // end switch

  return true;
}

bool RuleString::existsConstraint(const PartOfRule part,
                                  const SubPartIndex& index,
                                  const std::string& constraintName,
                                  const ConstraintAction& constraintAction,
                                  int& c) {
  switch (part) {
  case TRIGGER: {
    ostringstream oss;
    oss << "Error on constraint "
        << ": cannot have subindex on trigger";
    throw ConstraintSyntaxException(oss.str());
  }
  case LEFT: {
    return m_left.existsConstraint(&index,constraintName,
                                   constraintAction,c);
  }
  case RIGHT: {
    return m_right.existsConstraint(&index,constraintName,
                                    constraintAction,c);
  }
  } // end switch

  return true;

}

// to manage the agreement constraint, we modify the string of the rule
// in order to attach the constraint information to the transition
// this information will be decoded by the function dealing with
// the transition (createTransition) : this way, we do not have to trace
// the transition in all the buildind process of the automaton (including
// determinization and minimization).
// the format of modified transition is
// transition+index1/action1/type1+index2/action2/type2...

void RuleString::
addConstraint(const LimaString& constraint,
              MediaId language,
              const std::vector<SubAutomaton>& subAutomatons,
              const bool isAction) {
  AUCLOGINIT;
  if (isAction) {
    LDEBUG << "adding action " << constraint;
  }
  else {
    LDEBUG << "adding constraint " << constraint;
  }

  bool negative(false);
  bool actionIfSuccess(false);
  string constraintName;
  LimaString complement;

  LimaString arguments
    =readConstraintName(constraint,isAction,
                        constraintName,
                        negative,
                        actionIfSuccess);
  LDEBUG << "RuleString::addConstraint constraintName: " << constraintName << "; arguments: " << arguments;
    
  if (! arguments.isEmpty()) {
    readConstraintComplement(arguments,
                             complement);
  }
  // constraint has no arguments: is action
  // do not increment the number of constraints on the rule
  // (necessary for the constraintCheckList, that is not used for actions)
  if (isAction) {
    if (! arguments.isEmpty()) {
      LWARN << "Actions with arguments...";
      LWARN << "Hypothesis is only 1 argument...";
      // read (first) argument
      LimaString firstArg;
      arguments= readActionArgument(arguments,firstArg);
      const bool negative(false);
      // build Constraint object (to be registered in Recognizer)
      addUnaryAction(constraintName, complement, language, firstArg, negative, isAction, actionIfSuccess);
    }
    else {
      ConstraintAction executeAction(EXECUTE_IF_SUCCESS);
      if (! actionIfSuccess) {
        executeAction=EXECUTE_IF_FAILURE;
      }
      Constraint a(Constraint::noindex,constraintName,executeAction,language,complement);
      addAction(a);
      LDEBUG << "RuleString::addConstraint returns";
    }
    return;
  }
  else if (arguments.isEmpty()) {
    ostringstream oss;
    oss << "Error on constraint " << Misc::limastring2utf8stdstring(constraint)
        << ": no arguments found (maybe should be an action)";
    throw ConstraintSyntaxException(oss.str());
  }

  // read first argument
  PartOfRule partFirstArg;
  SubPartIndex indexFirstArg;

  arguments=
    readConstraintArgument(arguments,
                           partFirstArg,
                           indexFirstArg,
                           subAutomatons);

  if (arguments.isEmpty()) {
    // constraint is unary
    addUnaryConstraint(constraintName,complement,language,
                       partFirstArg,indexFirstArg,
                       negative,isAction,actionIfSuccess);
  }
  else {
    //binary constraint, read second argument
    PartOfRule partSecondArg;
    SubPartIndex indexSecondArg;
    readConstraintArgument(arguments,
                           partSecondArg,
                           indexSecondArg,
                           subAutomatons);

    bool reverseArguments=
      orderArguments(partFirstArg,indexFirstArg,
                     partSecondArg,indexSecondArg);

    if (partFirstArg==LEFT && partSecondArg==RIGHT) {
      m_hasLeftRightConstraint=true;
    }

    addBinaryConstraint(constraintName,complement,language,
                        partFirstArg,indexFirstArg,
                        partSecondArg,indexSecondArg,
                        negative,isAction,actionIfSuccess,
                        reverseArguments);
  }

  LDEBUG << "RuleString:Rule=" << getString();
}

// reorder arguments relatively to the order in which they will be
// tested during search in the automaton
// return false if nothing was done (same order)
// return true if arguments have been swaped (reverse order)
bool RuleString::
orderArguments(PartOfRule& partFirstArg,
               SubPartIndex& indexFirstArg,
               PartOfRule& partSecondArg,
               SubPartIndex&  indexSecondArg)
{
  if (partFirstArg < partSecondArg) {
    return false;
  }
  else if (partFirstArg == partSecondArg) {
    if (partFirstArg == LEFT) { // for left part, larger indices first
      if (indexSecondArg.isBefore(indexFirstArg)) {
        return false;
      }
    }
    else { // right part
      if (indexFirstArg.isBefore(indexSecondArg)) {
        return false;
      }
    }
  }

  // reverse arguments: swap
  PartOfRule tmpPart=partFirstArg;
  SubPartIndex tmpIndex=indexFirstArg;

  partFirstArg=partSecondArg;
  indexFirstArg=indexSecondArg;

  partSecondArg=tmpPart;
  indexSecondArg=tmpIndex;

  return true;
}

void RuleString::
addUnaryAction(const std::string& constraintName,
                   const LimaString& complement,
                   MediaId language,
                   const LimaString& argument,
                   const bool negative,
                   const bool isAction,
                   const bool actionIfSuccess)
{
  if (isAction) {
    /*
    ???  unary action not implemented version
    ConstraintAction executeAction(EXECUTE_IF_SUCCESS);
    if (! actionIfSuccess) {
      executeAction=EXECUTE_IF_FAILURE;
    }
    Constraint c(m_nbConstraints,constraintName,STORE,complement,negative);
    Constraint a(m_nbConstraints,constraintName,executeAction,complement,negative);
    addConstraint(part,index,subindex,c);
    addAction(a);
    m_nbConstraints++;
    ??? unary constraint version
    Constraint c(Constraint::noindex,constraintName,TEST,language,complement,negative);
    addConstraint(part,index,c);
    */
    // ??? synthesis
    ConstraintAction executeAction(EXECUTE_IF_SUCCESS);
    if (! actionIfSuccess) {
      executeAction=EXECUTE_IF_FAILURE;
    }
    Constraint a(Constraint::noindex,constraintName,executeAction,language,complement,negative);
    addAction(a,argument);
  }
  else {
    //unary actions are not supported yet
    AUCLOGINIT;
    LERROR << "addUnaryAction: isAction = false!";
    throw ConstraintSyntaxException("Action or Constraint with bad arguments...");
  }
}

void RuleString::
addUnaryConstraint(const std::string& constraintName,
                   const LimaString& complement,
                   MediaId language,
                   const PartOfRule& part,
                   const SubPartIndex& index,
                   const bool negative,
                   const bool isAction,
                   const bool actionIfSuccess)
{
  if (isAction) {
    //unary actions are not supported yet
    AUCLOGINIT;
    LERROR << "addUnaryConstraint: Actions with arguments are not yet supported";
    throw ConstraintSyntaxException("Actions with arguments are not yet supported");

    /*
    ConstraintAction executeAction(EXECUTE_IF_SUCCESS);
    if (! actionIfSuccess) {
      executeAction=EXECUTE_IF_FAILURE;
    }
    Constraint c(m_nbConstraints,constraintName,STORE,complement,negative);
    Constraint a(m_nbConstraints,constraintName,executeAction,complement,negative);
    addConstraint(part,index,subindex,c);
    addAction(a);
    m_nbConstraints++;
    */
  }
  else {
    Constraint c(Constraint::noindex,constraintName,TEST,language,complement,negative);
    addConstraint(part,index,c);
  }
}

void RuleString::
addBinaryConstraint(const std::string& constraintName,
                    const LimaString& complement,
                    MediaId language,
                    const PartOfRule& partFirstArg,
                    const SubPartIndex& indexFirstArg,
                    const PartOfRule& partSecondArg,
                    const SubPartIndex& indexSecondArg,
                    const bool negative,
                    const bool isAction,
                    const bool actionIfSuccess,
                    const bool reverseArguments)
{
  LIMA_UNUSED(actionIfSuccess);
  AUCLOGINIT;
  LDEBUG << "RuleString::addBinaryConstraint " << constraintName << " " << Common::Misc::limastring2utf8stdstring(complement)
  << " " << partFirstArg << " " << indexFirstArg << " " << partSecondArg << " " << indexSecondArg
  << " neg: " << negative << " isAction: " << isAction << " reverseArguments: " << reverseArguments;
  if (isAction) {
    // binary actions are not supported
    LERROR << "addBinaryConstraint: Actions with arguments are not yet supported";
    throw ConstraintSyntaxException("Actions with arguments are not yet supported");

    /*
    // push both arguments, action is EXECUTE
    Constraint cpush(m_nbConstraints,constraintName,PUSH,complement);
    addConstraint(partFirstArg,indexFirstArg,cpush);
    addConstraint(partSecondArg,indexSecondArg,cpush);

    ConstraintAction executeAction(EXECUTE_IF_SUCCESS);
    if (reverseArguments) {
      if (actionIfSuccess) {
        executeAction = EXECUTE_IF_SUCCESS_REVERSE;
      }
      else {
        executeAction = EXECUTE_IF_FAILURE_REVERSE;
      }
    }
    else if (! actionIfSuccess) {
      executeAction = EXECUTE_IF_FAILURE;
    }

    Constraint a(m_nbConstraints,constraintName,executeAction,complement);
    addAction(a);
    */
  }
  else {
    ConstraintAction storeAction(STORE);
    ConstraintAction compareAction(COMPARE);
    if (reverseArguments) {
      compareAction=COMPARE_REVERSE;
    }

    int constraintIndex(m_nbConstraints);
    int incrementConstraint(true);
    int addFirstConstraint(true);
    int addSecondConstraint(true);

    // special case if constraint on repetitive structure
    // use stack
    if (partFirstArg == partSecondArg &&
        IsOnSameRepetitiveStructure(indexFirstArg,indexSecondArg)) {
      storeAction=PUSH;
      compareAction=COMPARE_STACK;
      if (reverseArguments) {
        compareAction=COMPARE_STACK_REVERSE;
      }

      LDEBUG << "testing exists constraint";
      if (existsConstraint(partFirstArg,indexFirstArg,constraintName,
                           storeAction,constraintIndex)) {
        LDEBUG << "=> true: constraint " << constraintIndex;
        addFirstConstraint=false;
        incrementConstraint=false;
      }
      LDEBUG << "testing exists second constraint";
      int constraintIndex2(0);
      if (existsConstraint(partSecondArg,indexSecondArg,constraintName,
                           compareAction,constraintIndex2)) {
        LDEBUG << "=> true: constraint " << constraintIndex;
        addSecondConstraint=false;
        incrementConstraint=false;
        constraintIndex=constraintIndex2;
      }
    }
    if (addFirstConstraint) {
      Constraint cfirst(constraintIndex,constraintName,
                        storeAction,language,complement);
      LDEBUG << "addConstraint(constraintIndex:" << constraintIndex
             << "constraintName:" << constraintName
             << "storeAction:" << storeAction << ")";
      addConstraint(partFirstArg,indexFirstArg,cfirst);
    }
    if (addSecondConstraint) {
      Constraint csecond(constraintIndex,constraintName,
                         compareAction,language,complement,negative);
      LDEBUG << "addConstraint(constraintIndex:" << constraintIndex
             << "constraintName:" << constraintName
             << "compareAction:" << compareAction << ")";
      addConstraint(partSecondArg,indexSecondArg,csecond);
    }
    if (incrementConstraint) {
      m_nbConstraints++;
    }
  }
  LDEBUG << "====== END RuleString::addBinaryConstraint ";
}

bool RuleString::
IsOnSameRepetitiveStructure(const SubPartIndex& index1,
                            const SubPartIndex& index2)
{

  if (index1.getPartIndex() ==
      index2.getPartIndex()) {
    if (index1.hasSubPart()
        && index2.hasSubPart()) {
      return
        IsOnSameRepetitiveStructure(*(index1.getSubPartIndex()),
                                    *(index2.getSubPartIndex()));
    }
    else {
      return false;
    }
  }

  // check all cases
  // constraint between first and last in not repetitive
  // only between
  // first/last and current/next
  // current and first/next/last
  // next and first/current/last

  switch (index1.getPartIndex().first) {
  case SUB_NONE: return false;
  case SUB_FIRST:
    switch (index2.getPartIndex().first) {
    case SUB_CURRENT:
    case SUB_NEXT:    return true;
    default:          return false;
    }
  case SUB_CURRENT:
    switch (index2.getPartIndex().first) {
    case SUB_FIRST:
    case SUB_NEXT:
    case SUB_LAST:    return true;
    default:          return false;
    }
  case SUB_NEXT:
    switch (index2.getPartIndex().first) {
    case SUB_CURRENT:
    case SUB_LAST:    return true;
    default:          return false;
    }
  case SUB_LAST:
    switch (index2.getPartIndex().first) {
    case SUB_CURRENT:
    case SUB_NEXT:    return true;
    default:          return false;
    }
  }
  return false;
}

LimaString RuleString::
readConstraintName(const LimaString& str,
                   const bool& isAction,
                   std::string& constraintName,
                   bool& negative,
                   bool& actionIfSuccess) {

  int begin=str.indexOf(CHAR_CONSTRAINT_BEGIN_ARG);

  if (begin == -1 ||
      str[str.length()-1] != CHAR_CONSTRAINT_END_ARG) {
    ostringstream oss;
    oss << "Error on constraint " << Misc::limastring2utf8stdstring(str)
        << ": cannot find argument delimiters";
    throw ConstraintSyntaxException(oss.str());
  }

  // read the constraint name
  negative=false;
  actionIfSuccess=false;
  constraintName=Misc::limastring2utf8stdstring(str.left(begin));
  if (isAction) {
    if (constraintName[0] == CHAR_BEGIN_ACTION_IF_SUCCESS) {
      actionIfSuccess=true;
      constraintName=string(constraintName,1);
    }
    else if (constraintName[0] == CHAR_BEGIN_ACTION_IF_FAILURE) {
      actionIfSuccess=false;
      constraintName=string(constraintName,1);
    }
    else { // default is true (compatible with previous notation)
      actionIfSuccess=true;
    }
  }
  if (constraintName[0] == CHAR_NEGATIVE_CONSTRAINT) {
    negative=true;
    constraintName=string(constraintName,1);
  }

  // last character before ending of args ')'
  int end=str.length()-2;
//   AUCLOGINIT;
//   LDEBUG << "readConstraintName got constraint name:" << constraintName << ", actionIsSuccess:" << actionIfSuccess << ", negative:" << negative << "and args: " << str.mid(begin+1,end-begin);
  return str.mid(begin+1,end-begin);
}

void RuleString::
readConstraintComplement(LimaString& str,
                         LimaString& complement)
{
  // testing if the constraint has a complement argument
  // (to be given as argument of the constraint function)
  // if given, is always last argument of the constraint

  AUCLOGINIT;
  LINFO << "readConstraintComplement: [" << str << "]";

  // last character before ending of args ')'
  int end=str.length()-1;
  //LINFO << "  end is " << end;
  if (rfindSpecialCharacter(str,CHAR_CONSTRAINT_COMPLEMENT_ARG,end)
      == end) {
    // complement argument exists
    int beginCompl=rfindSpecialCharacter(str,
                                       CHAR_CONSTRAINT_COMPLEMENT_ARG,
                                       end-1);
    //LINFO << "  beginCompl is " << beginCompl;
    if (beginCompl == -1
        || (beginCompl>=1 &&
            str[beginCompl-1] != CHAR_CONSTRAINT_SEP_ARG)
        ) {
      ostringstream oss;
      oss << "Error on constraint " << Misc::limastring2utf8stdstring(str)
          << ": got confused by complement argument";
      throw ConstraintSyntaxException(oss.str());
    }
    complement=str.mid(beginCompl+1,end-beginCompl-1);
    if (beginCompl>=1) {
      end=beginCompl-1; // skip '"' and "," before it
    }
    else {
      end = 0;
    }
    //LINFO << "  erasing from " << end;
    str.truncate(end);
  }
  //LINFO << "  complement is " << complement;
  //LINFO << "  str is now " << str;
}

LimaString RuleString::
readActionArgument(const LimaString& arguments,
                       LimaString& argument) {
  LimaString nextArgument;

  // check if more than 1 argument
  int sep=
    findSpecialCharacter(arguments,
                         CHAR_CONSTRAINT_SEP_ARG,
                         0);
  if (sep != -1) {
    argument=arguments.left(sep);
    nextArgument=arguments.mid(sep+1);
  }
  else
    argument=arguments;
  // check if argument begin with "trigger", "left" or "right"
  if ( (argument.indexOf(STRING_CONSTRAINT_TRIGGER) != 0)
    && (argument.indexOf(STRING_CONSTRAINT_LEFT) != 0)
    && (argument.indexOf(STRING_CONSTRAINT_RIGHT) != 0) )
  {
    AUCLOGINIT;
    LWARN << "Warning! readActionArgument: [" << argument
        << "/" << nextArgument << "], bad value for argument";
  }
  else
  {
    AUCLOGINIT;
    LDEBUG << "readActionArgument: [" << argument
          << "/" << nextArgument << "]";
  }
  return nextArgument;
}


LimaString RuleString::
readConstraintArgument(const LimaString& arguments,
                       PartOfRule& part,
                       SubPartIndex& index,
                       const std::vector<SubAutomaton>& subAutomatons) {

  LimaString argument(arguments);
  LimaString nextArgument;

  int sep=
    findSpecialCharacter(arguments,
                         CHAR_CONSTRAINT_SEP_ARG,
                         0);

  if (sep != -1) {
    argument=arguments.left(sep);
    nextArgument=arguments.mid(sep+1);
  }

  AUCLOGINIT;
  LINFO << "readConstraintArgument: [" << argument
        << "/" << nextArgument << "]";

  if (argument.indexOf(STRING_CONSTRAINT_TRIGGER) == 0) {
    part=TRIGGER;
    index.init(Common::Misc::utf8stdstring2limastring("1"),
               subAutomatons); // always 1 for the trigger
  }
  else if (argument.indexOf(STRING_CONSTRAINT_LEFT) == 0) {
    part=LEFT;
    index.init(argument.mid(LENGTH_CONSTRAINT_LEFT+1),
               subAutomatons);
  }
  else if (argument.indexOf(STRING_CONSTRAINT_RIGHT) == 0) {
    part=RIGHT;
    index.init(argument.mid(LENGTH_CONSTRAINT_RIGHT+1),
               subAutomatons);
  }
//   else {
//     ostringstream oss;
//     oss << "Error on constraint "
//         << ": cannot recognize part name in \""
//         << Common::Misc::limastring2utf8stdstring(argument)
//         << "\" (should be "
//         << Common::Misc::limastring2utf8stdstring(STRING_CONSTRAINT_TRIGGER) << ","
//         << Common::Misc::limastring2utf8stdstring(STRING_CONSTRAINT_LEFT) << " or "
//         << Common::Misc::limastring2utf8stdstring(STRING_CONSTRAINT_RIGHT) << ")";
//     throw ConstraintSyntaxException(oss.str());
//   }

  return nextArgument;
}

// set some RuleElementIdentifier to each transition
void RuleString::identifyTransition() {
  
  m_trigger.identifyTransition("trigger");
  m_left.identifyTransition("left");
  m_right.identifyTransition("right");
}

//------------------------------
// parse the index specifying the word in the rule part
// (can be complicated with sub-indexes)
void RuleString::parseIndex(const LimaString& str,
                            int& index,
                            LimaString& subindex)
{
  int i=str.indexOf(CHAR_CONSTRAINT_INDEX);
  if (i != -1) { // group subindex indicated
    index=str.left(i).toInt();
    subindex=str.mid(i+1);
  }
  else {
    index=str.toInt();
    subindex=LimaString();
  }
}

bool RuleString::readConstrainedTerm(const LimaString& term,
                                     PartOfRule& part,
                                     int& index,
                                     LimaString& subindex) {
  subindex=LimaString();

  if (term.indexOf(STRING_CONSTRAINT_TRIGGER) == 0) {
    part=TRIGGER;
    index=1; // always 1 for the trigger
    return true;
  }
  else if (term.indexOf(STRING_CONSTRAINT_LEFT) == 0) {
    part=LEFT;
    if (term[LENGTH_CONSTRAINT_LEFT] != CHAR_CONSTRAINT_INDEX) {
      return false;
    }
    else {
      parseIndex(term.mid(LENGTH_CONSTRAINT_LEFT+1),
                 index,subindex);
      return true;
    }
  }
  else if (term.indexOf(STRING_CONSTRAINT_RIGHT) == 0) {
    part=RIGHT;
    if (term[LENGTH_CONSTRAINT_RIGHT] != CHAR_CONSTRAINT_INDEX) {
      return false;
    }
    else {
      parseIndex(term.mid(LENGTH_CONSTRAINT_RIGHT+1),
                 index,subindex);
      return true;
    }
  }
  return false;
}


//***********************************************************************
// output
//***********************************************************************
std::ostream& operator << (std::ostream& os, const RuleString&) {
  return os;
}

} // end namespace
} // end namespace
} // end namespace
