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
 * @file       rule.h
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Tue Oct 15 2002
 * copyright   Copyright (C) 2002 by CEA LIST
 * Project     Automaton
 * 
 * @brief      A rule is composed of a trigger, that can 
 * be a word or a pos (a TransitionUnit) and of two automata for 
 * the contexts around the trigger
 * 
 ***********************************************************************/

#ifndef RULE_H
#define RULE_H

#include "AutomatonExport.h"
#include "automaton.h"
#include "constraint.h"
#include "automatonReaderWriter.h"
#include "common/MediaticData/EntityType.h"
#include "common/Data/LimaString.h"
#include <iostream>
#include <fstream>
#include <string>

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

class LIMA_AUTOMATON_EXPORT Rule
{
friend class AutomatonReader;
friend class AutomatonWriter;

 public:
  Rule(); 
/*   Rule(const LimaString&, Tchar sep=CHAR_SEP_RULE);  */
  Rule(const Rule&);
  ~Rule();
  Rule& operator = (const Rule&);

  // comparison operator (to sort rules by their weight)
  bool operator<(const Rule& r);

  void reinit();
  
  // access functions
  TransitionUnit* getTrigger() const;
  Automaton const& leftAutomaton() const;
  Automaton const& rightAutomaton() const;
  Common::MediaticData::EntityType getType() const;
  LinguisticCode getLinguisticProperties() const;
  const LimaString& getNormalizedForm() const;
  uint64_t numberOfConstraints() const;
  bool contextual() const;
  bool negative() const;
  const std::vector<Constraint>& getActions() const;
  double getWeight() const { return m_weight; }
  const std::string& getRuleId() const { return m_ruleId; }

  void setTrigger(TransitionUnit*);
  void setLeftAutomaton(const Automaton&);
  void setRightAutomaton(const Automaton&);
  void setType(const Common::MediaticData::EntityType&);
  void setLinguisticProperties(const LinguisticCode&);
  void setNormalizedForm(const LimaString&);
  void setNumberOfConstraints(const uint64_t);
  void incrementNumberOfConstraints();
  void setContextual(const bool);
  void setNegative(const bool);
  void addAction(const Constraint& c);
  void setWeight(const double& w) { m_weight=w; }
  void setHasLeftRightConstraint(const bool v) 
    { m_hasLeftRightConstraint=v; }
  void setRuleId(const std::string& ruleId) { m_ruleId=ruleId; }

  /** 
   * test the rule on a morphological graph : the trigger has already
   * been tested: test only left and right context
   * 
   * @param graph the graph
   * @param leftContext the vertex at the beginning of the left context
   * @param rightContext the vertex at the beginning of the right context
   * @param leftLimit the limit vertex for the left context
   * @param rightLimit the limit vertex for the right context
   * @param analysis the content of the analysis (may be modified)
   * @param leftmatch the resulting match for left context
   * @param rightmatch the resulting match for right context 
   * 
   * @return true if a match was found, false otherwise
   */
  bool test(const LinguisticProcessing::LinguisticAnalysisStructure::AnalysisGraph& graph,
            const LinguisticGraphVertex& leftContext,
            const LinguisticGraphVertex& rightContext,
            const LinguisticGraphVertex& leftLimit,
            const LinguisticGraphVertex& rightLimit,
            AnalysisContent& analysis,
            RecognizerMatch& leftmatch,
            RecognizerMatch& rightmatch,
            ConstraintCheckList&,
            ForwardSearch& forward,
            BackwardSearch& backward,
            const AutomatonControlParams& controlParams) const;

  bool executeActions(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                      AnalysisContent& analysis,
                      ConstraintCheckList& constraintCheckList,
                      const bool success,
                      RecognizerMatch* result) const;
  
  // text output
  friend LIMA_AUTOMATON_EXPORT std::ostream& operator << (std::ostream&, const Rule&);
  friend LIMA_AUTOMATON_EXPORT QDebug& operator << (QDebug&, const Rule&);
                      
 protected:
  TransitionUnit* m_trigger; // the trigger
  Automaton m_left;  // left context (i.e. preceding context)
  Automaton m_right; // right context (i.e. following context)
  Common::MediaticData::EntityType m_type; // type of the expression
  LinguisticCode m_lingProp; // linguistic properties associated 
                             // to the recognized expression
  LimaString m_normalizedForm;      // normalized form of the expression
  uint64_t m_numberOfConstraints; // possible agreement constraints
  bool m_contextual; // is the expression ambiguous or not (ambiguity must 
                     // be solved by context if it is the case)

  bool m_negative; // the rule is negative
  bool m_hasLeftRightConstraint; // the rule has at least one 
                                 // binary constraint that deals with left AND right parts
  std::vector<Constraint> m_actions;

  double m_weight; // weight of the rule
  std::string m_ruleId; // id of the rule

  // private functions
  //**********************************************************************
  // helper functions for constructors and destructors
  void init();
  void copy(const Rule& r);
  void freeMem();

};

/***********************************************************************/
// inline access functions
/***********************************************************************/
inline TransitionUnit* Rule::getTrigger() const { return m_trigger; }
inline Automaton const& Rule::leftAutomaton() const  { return m_left; }
inline Automaton const& Rule::rightAutomaton() const { return m_right; }
inline Common::MediaticData::EntityType Rule::getType() const { return m_type; }
inline LinguisticCode Rule::getLinguisticProperties() const { 
  return m_lingProp; }
inline const LimaString& Rule::getNormalizedForm() const { 
  return m_normalizedForm; }
inline uint64_t Rule::numberOfConstraints() const { 
  return m_numberOfConstraints; }
inline const std::vector<Constraint>& Rule::getActions() const { 
  return m_actions; 
}

inline bool Rule::contextual() const { return m_contextual; }
inline void Rule::setContextual(const bool c) { m_contextual = c; }

inline bool Rule::negative() const { return m_negative; }
inline void Rule::setNegative(const bool c) { m_negative = c; }
inline void Rule::setNumberOfConstraints(const uint64_t n) {
  m_numberOfConstraints=n;
}
inline void Rule::incrementNumberOfConstraints() {
  m_numberOfConstraints++;
}
inline void Rule::setTrigger(TransitionUnit* t){ m_trigger=t; }
inline void Rule::setLeftAutomaton(const Automaton& a) { m_left=a; }
inline void Rule::setRightAutomaton(const Automaton& a) { m_right=a; }
inline void Rule::setType(const Common::MediaticData::EntityType& t) { m_type=t; }
inline void Rule::setLinguisticProperties(const LinguisticCode& l) { 
  m_lingProp=l; 
}
inline void Rule::setNormalizedForm(const LimaString& s) { 
  m_normalizedForm=s; 
}
inline void Rule::addAction(const Constraint& c) { 
  m_actions.push_back(c); 
}

} // namespace end
} // namespace end
} // namespace end

#endif
