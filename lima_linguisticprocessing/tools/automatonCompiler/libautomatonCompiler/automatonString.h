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
* File        : AutomatonString.h
* Project     : NamedEntities
* Author      : Besancon Romaric (besanconr@zoe.cea.fr)
* Created on  : Mon Sep  8 2003
* Copyright   : (c) 2003 by CEA
* Version     : $Id: automatonString.h 9081 2008-02-25 18:34:51Z de-chalendarg $
*
* Description : this class contains a structured string representation of a rule part (automaton)
*
*************************************************************************/

#ifndef AUTOMATONSTRING_H
#define AUTOMATONSTRING_H

#include "ruleFormat.h"
#include "subPartIndex.h"
#include "common/LimaCommon.h"
#include "linguisticProcessing/core/Automaton/automatonCommon.h"
#include "linguisticProcessing/core/Automaton/constraint.h"
#include <limits.h> /* for UINT_MAX */
#include <vector>
#include <iostream>

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

class Gazeteer;
class SubAutomaton;

#define AUCLOGINIT LOGINIT("Automaton::Compiler")

typedef enum {
  UNKNOWN_TYPE,
  UNIT,
  SEQUENCE,
  SIMPLE_GAZETEER,
  ALTERNATIVE
} ElementType;

class AutomatonString
{
 public:

  // representing infinity (loop on a same state)
  static const int INFINITE_OCC=UINT_MAX;

  AutomatonString(); 
  AutomatonString(const AutomatonString&);
  AutomatonString(const LimaString& str);
  AutomatonString(const LimaString& str,
                  const std::vector<Gazeteer>& gazeteers,
                  const std::vector<SubAutomaton>& subAutomatons);
  AutomatonString(const LimaString& str,
                  const std::vector<Gazeteer>& gazeteers,
                  const std::vector<SubAutomaton>& subAutomatons,
                  const int begin,
                  const int size=-1);
  AutomatonString(const LimaString& string, const LimaString& modifier);
  ~AutomatonString();
  AutomatonString& operator = (const AutomatonString&);
  void reinit();

  void init(const LimaString& str,
            const std::vector<Gazeteer>& gazeteers,
            const std::vector<SubAutomaton>& subAutomatons,
            const int begin,
            const int size);

  void insertConstraint(const SubPartIndex* index,
                        Constraint& constraint,
                        const std::string& depth="");

  void insertConstraint(std::vector <Lima::LinguisticProcessing::Automaton::AutomatonString >::size_type index,
                        const LimaString& subindex,
                        const Constraint& constraint);
  void insertConstraint(const LimaString& subindex,
                        const Constraint& constraint);
  void split();
  void split(const bool first);
  void insertConstraintInUnit(const Constraint& c);
  void insertConstraintInPart(std::vector <Lima::LinguisticProcessing::Automaton::AutomatonString >::size_type index,
                              const Constraint& c);
  
  // Prepare work to build an identifier of type RuleElementIdentifier for each transition
  // The idea is to go through the structure of the ruleString and set a property for each node
  // of the rule, down to the leaves (AutomatonString of type UNIT). The property is a pair (partId, index) 
  // This property will be used to create RuleElementIdentifier for each TransitionUnit.
  void identifyTransition(const std::string & id);

  /** 
   * propagate the constraints from groups to elements
   * 
   */
  void propagateConstraints();

  /**
   * propagate some distributive properties (keep) 
   * from groups to elements 
   */
  void propagateProperties();

  bool empty() const;

  bool isOptional() const;
  bool isUnit() const;
  bool isSequence() const;
  bool isAlternative() const;
  bool isSimpleGazeteer() const;

  bool isArtificialSequence() const; //only for construction
  bool isSplittedFirst() const { return m_isSplittedFirst; }
  bool isSplittedLast() const { return m_isSplittedLast; }

  bool existsConstraint(int& constraintNumber) const;
  bool existsConstraint(const LimaString& subindex,
                        int& constraintNumber) const;
  bool existsConstraint(std::vector <Lima::LinguisticProcessing::Automaton::AutomatonString >::size_type index,
                        const LimaString& subindex,
                        int& constraintNumber) const;
  bool existsConstraint(const SubPartIndex* index,
                        const std::string& constraintName,
                        const ConstraintAction& constraintAction,
                        int& constraintNumber) const;
  bool existsConstraint(const std::string& constraintName,
                        const ConstraintAction& constraintAction,
                        int& constraintNumber) const;

  bool hasConstraint() const;
  bool hasParts() const;
  bool hasModifiers() const;

  bool isKept() const;
  bool isNegative() const;

  int getMinOccurrences() const;
  int getMaxOccurrences() const;
  const LimaString& getUnitString() const { return m_unit; }
  const std::vector<Constraint>& getConstraints() const { return m_constraints; }
  ElementType getType() const { return m_type; }
  void setRuleElementId(const std::string& partId, const SubPartIndex index);

  void setType(const ElementType type);
  void setKeep(const bool keep);
  void setNegative(const bool negative);
  void setArtificialSequence(const bool isArtificial);
  void setOccurrences(const int min, const int max);
  void addConstraint(const Constraint& c);
  void addConstraints(const std::vector<Constraint>& c);

  void copyParts(const AutomatonString& r);
  void copyModifiers(const AutomatonString& a);
  void copyConstraints(const AutomatonString& a);

  void removeArtificialSequences(const bool inSubPart=false);
  void removeUnitSequences();

  bool parse(const LimaString& str,
             const std::vector<Gazeteer>& gazeteers,
             const std::vector<SubAutomaton>& subAutomatons,
             const int begin=0,
             const int end=-1);
  void parseUnit(const LimaString& str,
                 const std::vector<Gazeteer>& gazeteers,
                 const std::vector<SubAutomaton>& subAutomatons,
                 const int begin=0,
                 const int end=-1);

  int parseGroup(const LimaString& str,
                          const std::vector<Gazeteer>& gazeteers,
                          const std::vector<SubAutomaton>& subAutomatons, 
                          const int begin);

  int parseGroupSequence(const LimaString& str,
                                  const std::vector<Gazeteer>& gazeteers,
                                  const std::vector<SubAutomaton>& subAutomatons,
                                  const int offset,
                                  const LimaChar endChar);
  int parseGroupAlternative(const LimaString& str,
                                     const std::vector<Gazeteer>& gazeteers,
                                     const std::vector<SubAutomaton>& subAutomatons,
                                     const int offset);

  int addGroup(const LimaString& str,
                        const std::vector<Gazeteer>& gazeteers,
                        const std::vector<SubAutomaton>& subAutomatons,
                        const int offset);

  
  int parseModifiersPre(const LimaString& s,
                                 const int begin);
  int parseModifiersPost(const LimaString& s,
                                  const int begin);

  bool parseModifiers(const LimaString& s, 
                      int& begin,
                      int& size);

  // for the getString function : do not put parentheses if 
  // the sequence contains at least one nokeep: AutomatonRegexp 
  // cannot handle ([a] [b])
  // suppose it is not called if is a unit
  // (test (!isUnit() && hasNoKeepInParts()) )
  bool hasNoKeepInParts() const;

  LimaString getString() const;
  LimaString applyModifiers(const LimaString& s) const;
  // just for debug
  LimaString getModifier() const;
  LimaString getStringDebug() const;
  const std::string& getId() const; /** id  with embeded numbering */
  
  std::vector<AutomatonString>& getParts();
  const std::vector<AutomatonString>& getParts() const;
  
  friend std::ostream& operator << (std::ostream&, const AutomatonString&);
  friend QDebug& operator << (QDebug&, const AutomatonString&);
  
 private:
  LimaString m_unit;               /**< expression (when element is unit) */
  ElementType m_type;           /**< type of the element */
  std::vector<AutomatonString> m_parts; /**< parts of the expression (when 
                                           element is sequence or choice)*/
  std::string m_automId; /** id of automaton */
  
  // possible modifiers
  int m_minOccurrences;
  int m_maxOccurrences;
  bool m_keep; /**< the expression is kept in the recognized expression */
  bool m_negative; /**< the expression is negative */
  
  // possible constraints
  std::vector<Constraint> m_constraints;

  // if the sequence has been added to ease construction
  bool m_artificialSequence;

  // information kept on artifical split to handle
  // constraints on repetitive structures
  bool m_isSplittedFirst;
  bool m_isSplittedLast;

  // private methods
  LimaString syntaxCorrection(const LimaString& s) const;
  void splitOnChar(const LimaString& str,
                   const LimaChar c,
                   const std::vector<Gazeteer>& gazeteers,
                   const std::vector<SubAutomaton>& subAutomatons,
                   const int begin=0,
                   const int size=-1);


  AutomatonString& findSubPart(std::vector <Lima::LinguisticProcessing::Automaton::AutomatonString >::size_type index);
  const AutomatonString& findSubPart(std::vector <Lima::LinguisticProcessing::Automaton::AutomatonString >::size_type index) const;

};

//**********************************************************************
// inline functions
//**********************************************************************
inline const std::string & AutomatonString::getId() const {
  return m_automId;
}

inline bool AutomatonString::empty() const {
  return (m_unit.isEmpty() && m_parts.empty());
}

inline std::vector<AutomatonString>& AutomatonString::getParts() {
  return m_parts;
}

inline const std::vector<AutomatonString>& AutomatonString::getParts() const {
  return m_parts;
}

inline bool AutomatonString::hasModifiers() const {
  return ( m_maxOccurrences != 0 || !m_keep || m_negative );
}
inline bool AutomatonString::isKept() const {
  return m_keep;
}
inline bool AutomatonString::isNegative() const {
  return m_negative;
}
inline bool AutomatonString::isOptional() const {
  return (m_maxOccurrences != 0);
}

inline int AutomatonString::getMinOccurrences() const {
  return m_minOccurrences;
}
inline int AutomatonString::getMaxOccurrences() const {
  return m_maxOccurrences;
}

inline void AutomatonString::setType(const ElementType type) {
  m_type=type;
}
inline void AutomatonString::setKeep(const bool keep) {
  m_keep=keep;
}
inline void AutomatonString::setNegative(const bool negative) {
  m_negative=negative;
}
inline void AutomatonString::setArtificialSequence(const bool isArtificial) {
  m_artificialSequence=isArtificial;
}

inline void AutomatonString::setOccurrences(const int min,
                                            const int max) {
  m_minOccurrences=min;
  m_maxOccurrences=max;
}

inline void AutomatonString::addConstraint(const Constraint& c) {
  m_constraints.push_back(c);
}
inline void AutomatonString::addConstraints(const std::vector<Constraint>& c) {
  if (! c.empty()) {
    m_constraints.insert(m_constraints.end(),c.begin(),c.end());
  }
}

inline bool AutomatonString::isArtificialSequence() const { 
  return m_artificialSequence; 
}
inline bool AutomatonString::isUnit() const { 
  return (m_type == UNIT); 
}
inline bool AutomatonString::isSimpleGazeteer() const { 
  return (m_type == SIMPLE_GAZETEER); 
}
inline bool AutomatonString::isSequence() const { 
  return (m_type == SEQUENCE); 
}
inline bool AutomatonString::isAlternative() const {
  return (m_type == ALTERNATIVE);
}

inline bool AutomatonString::hasConstraint() const {
  return (! m_constraints.empty());
}

inline bool AutomatonString::hasParts() const {
  return (! m_parts.empty());
}

inline bool AutomatonString::existsConstraint(int& c) const {
  if (m_constraints.empty()) {
    c=0;
    return false;
  }
  else {
    c=m_constraints.back().index();
    return true;
  }
}

inline void AutomatonString::copyParts(const AutomatonString& r) {
  m_parts=r.m_parts;
}

inline void AutomatonString::copyConstraints(const AutomatonString& r) {
  m_constraints=r.m_constraints;
}

inline void AutomatonString::copyModifiers(const AutomatonString& r) {
  m_minOccurrences=r.m_minOccurrences;
  m_maxOccurrences=r.m_maxOccurrences;
  m_keep=r.m_keep;
  m_negative=r.m_negative;
  m_constraints=r.m_constraints;
}


} // end namespace
} // end namespace
} // end namespace

#endif
