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
* File        : ruleCompiler.cpp
* Author      : Besancon Romaric (besanconr@zoe.cea.fr)
* Created on  : Mon Apr  7 2003
* Copyright   : (c) 2003 by CEA
* Version     : $Id: ruleCompiler.cpp 9081 2008-02-25 18:34:51Z de-chalendarg $
*
************************************************************************/

#include "ruleCompiler.h"
#include "ruleString.h"
#include "automatonCompiler.h"
#include "transitionCompiler.h"
#include "compilerExceptions.h"
#include "ruleFormat.h"
#include "tstring.h"
#include "linguisticProcessing/core/Automaton/rule.h"
#include "linguisticProcessing/core/Automaton/constraint.h"
#include "common/MediaticData/mediaticData.h"
#include "common/Data/strwstrtools.h"
#include <iostream>

using namespace std;

namespace Lima {
using namespace Common;
namespace LinguisticProcessing {
namespace Automaton {
using namespace AutomatonCompiler;
namespace RuleCompiler {

/***********************************************************************/
// building the rule from a string
/***********************************************************************/
LimaString initRule(Rule& r,
                     const LimaString& str, 
                     MediaId language,
                     const std::vector<Gazeteer>& gazeteers,
                     const std::vector<SubAutomaton>& subAutomatons,
                     const std::vector<LimaString>& activeEntityGroups,
                     const std::string& filename,
                     uint64_t lineNumber) {

  AUCLOGINIT;
  LDEBUG << "RuleCompiler:initializing rule " << str;

  //LimaString ruleString;
  RuleString s(str,language,gazeteers,subAutomatons);

  try {
    // copy constraint informations to the rule
    // set the number of constraints
    r.setNumberOfConstraints(s.getNbConstraints());
    r.setHasLeftRightConstraint(s.hasLeftRightConstraint());
    
    // add the actions to the rule
    for (std::vector<Constraint>::const_iterator a=s.getActions().begin(); 
         a!=s.getActions().end(); a++) {
      r.addAction(*a);
    }
    for (std::vector<std::pair<LimaString,Constraint> >::const_iterator a=s.getActionsWithOneArgument().begin(); 
         a!=s.getActionsWithOneArgument().end(); a++) {
      r.addAction(a->second,a->first);
    }
    
    //ruleString=s.getString();
  }
  catch (AutomatonCompilerException& exception) {
  LERROR << "Error on rule: " << str;
    throw;
  }
  
  //LDEBUG << "RuleCompiler:string=" << ruleString;
  

  r.setTrigger(createTransition(s.getTrigger(),language,"trigger",activeEntityGroups));

  try {
    r.setLeftAutomaton(AutomatonCompiler::buildAutomaton(s.getLeft(), 
                                                         language,
                                                         BACKWARDSEARCH,
                                                         activeEntityGroups));
  }
  catch (AutomatonCompilerException& e) {
    LERROR << "Error: "<< e.what()
      << " on left part of rule: " << str;
    throw;
  }

  try {
    r.setRightAutomaton(AutomatonCompiler::buildAutomaton(s.getRight(),
                                                          language,
                                                          FORWARDSEARCH,
                                                          activeEntityGroups));
  }
  catch (AutomatonCompilerException& e) {
    LERROR << "Error: "<< e.what()
      << " on left part of rule: " << str;
    throw;
  }
  
  LimaString& stringType=s.getType();
  try {
    if (stringType.indexOf(STRING_NEGATIVE_TYPE_RULE)==0) {
      setType(r,
              stringType.mid(
                          LENGTH_NEGATIVE_TYPE_RULE),
              activeEntityGroups);
      r.setNegative(true);
    }
    else if (stringType.indexOf(STRING_ABSOLUTE_TYPE_RULE)==0) {
      setType(r,
              stringType.mid(
                          LENGTH_ABSOLUTE_TYPE_RULE),
              activeEntityGroups);
      r.setContextual(false);
    }
    else {
      setType(r,stringType,activeEntityGroups);
    }
  }
  catch (UnknownTypeException) {
    LERROR << "Error on rule [" << str << "]";
    throw;
  }

  r.setNormalizedForm(s.getNorm());
  ostringstream oss;
  oss << filename << ":" << lineNumber;
  LDEBUG << "rule id is '" << oss.str() << "' / filename="<< filename << ",lineNumber=" << lineNumber;
  r.setRuleId(oss.str());

  return str;
}

//**********************************************************************
// find the numeric form of type of expression from a text form
//**********************************************************************
void setType(Rule& r,
             const LimaString& s,
             const std::vector<LimaString>& activeEntityGroups) 
{
  LimaString str;
  //std::string::size_type i(findSpecialCharacter(s,CHAR_POS_TR,0));
  int i(findSpecialCharacter(s,CHAR_POS_TR,0));
  if (i != -1) { // there are linguistic properties
    r.setLinguisticProperties(static_cast<LinguisticCode>(s.mid(i+1).toInt()));
    str=s.left(i);
  }
  else {
    r.setLinguisticProperties(static_cast<LinguisticCode>(0));
    str=s;
  }

  Common::MediaticData::EntityType type=
    resolveEntityName(str,activeEntityGroups);
  if (type.isNull()) {
    std::ostringstream oss;
    oss << "type [" << str.toUtf8().data() << "] not recognized";
    throw UnknownTypeException(oss.str());
  }
  r.setType(type);
}


} // end namespace
} // end namespace
} // end namespace
} // end namespace
