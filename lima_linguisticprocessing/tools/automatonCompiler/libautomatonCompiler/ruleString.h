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
* File        : ruleString.h
* Project     : NamedEntities
* Author      : Besancon Romaric (besanconr@zoe.cea.fr)
* Created on  : Mon Sep  8 2003
* Copyright   : (c) 2003 by CEA
* Version     : $Id: ruleString.h 8079 2007-04-13 17:06:31Z mesnardo $
*
* Description : this class contains the decomposed string representing the rule, useful for the treatment of constraints
*
*************************************************************************/

#ifndef RULESTRING_H
#define RULESTRING_H

#include "automatonString.h"
#include "gazeteer.h"
#include "subAutomaton.h"
#include "subPartIndex.h"
#include "linguisticProcessing/core/Automaton/rule.h"
#include "common/LimaCommon.h"
#include <iostream>

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

class RuleString
{
 public:
  RuleString(); 
  RuleString(const RuleString& r);
  RuleString(const LimaString& str,
             MediaId language,
             const std::vector<Gazeteer>& gazeteers,
             const std::vector<SubAutomaton>& subAutomatons);
  ~RuleString();
  RuleString& operator = (const RuleString& r);
  
  AutomatonString& getTrigger() { return m_trigger; }
  AutomatonString& getLeft()    { return m_left; }
  AutomatonString& getRight()   { return m_right; }
  LimaString& getType()        { return m_type; }
  LimaString& getNorm()        { return m_norm; }
  const std::string& getRuleId() { return m_ruleId; }
  int getNbConstraints() { return m_nbConstraints; }
  int hasLeftRightConstraint() {return m_hasLeftRightConstraint;}
  const std::vector<Constraint>& getActions() const { return m_actions; }

  void addAction(const Constraint& a) { m_actions.push_back(a); }
  
  LimaString getString() const;

  // add a constraint to the rule (i.e. to the string describing the rule)
  void addConstraint(const LimaString& constraint, 
                     MediaId language,
                     const std::vector<SubAutomaton>& subAutomatons,
                     const bool isAction=false);

  friend std::ostream& operator << (std::ostream&, const RuleString&);

 private:
  AutomatonString m_trigger;    /**< trigger : isUnit() must be true */
  AutomatonString m_left;       /**< left context */
  AutomatonString m_right;      /**< right context */
  LimaString m_type;               /**< type of the rule  */
  LimaString m_norm;               /**< normalized form */
  int m_nbConstraints; /**< number of constraints in the rule */
  bool m_hasLeftRightConstraint; /**< indicates if at least one constraint is on left and right parts */
  std::string m_ruleId; /**< identifier of the rule (file + line number, for debug) */

  // possible actions attached to the rule (not to transitions)
  std::vector<Constraint> m_actions;

  // enum types for internal use only
  /**
   * enum type for identifying the part of the rule concerned by a constraint
   */
  typedef enum { TRIGGER=0, LEFT=1, RIGHT=2 } PartOfRule;

  // private utility functions
  // set some RuleElementIdentifier to each transition
  void identifyTransition();
  
  void initPart(const LimaString& str, AutomatonString& part,
                const std::vector<Gazeteer>& gazeteers,
                const std::vector<SubAutomaton>& subAutomatons);

  void treatConstraints(const LimaString& str,
                        MediaId language,
                        const std::vector<SubAutomaton>& subAutomatons);

  int findNextConstraint(const LimaString& s,
                                  const int pos,
                                  bool& isAction) const;

  bool addConstraint(const PartOfRule part,
                     const SubPartIndex& index,
                     Constraint& c);
  bool existsConstraint(const PartOfRule part,
                        const SubPartIndex& index,
                        const std::string& constraintName,
                        const ConstraintAction& constraintAction,
                        int& c);

  bool readConstrainedTerm(const LimaString& str, 
                           PartOfRule& part, 
                           int& index,
                           LimaString& subindex);

  void parseIndex(const LimaString& str, 
                  int& index,
                  LimaString& subindex);


  bool orderArguments(PartOfRule& partFirstArg,
                      SubPartIndex& indexFirstArg,
                      PartOfRule& partSecondArg,
                      SubPartIndex&  indexSecondArg);

  void 
    addUnaryConstraint(const std::string& constraintName,
                       const LimaString& complement,
                       MediaId language,
                       const PartOfRule& part,
                       const SubPartIndex& index,
                       const bool negative,
                       const bool isAction,
                       const bool actionIfSuccess);
  
  void 
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
                        const bool reverseArguments);

  bool 
    IsOnSameRepetitiveStructure(const SubPartIndex& indexFirstArg,
                                const SubPartIndex& indexSecondArg);

  LimaString 
    readConstraintName(const LimaString& str,
                       const bool& isAction,
                       std::string& constraintName,
                       bool& negative,
                       bool& actionIfSuccess);

  void readConstraintComplement(LimaString& str,
                                LimaString& complement);

  LimaString 
    readConstraintArgument(const LimaString& arguments, 
                           PartOfRule& part, 
                           SubPartIndex& index,
                           const std::vector<SubAutomaton>& subAutomatons);
};

} // end namespace
} // end namespace
} // end namespace

#endif
