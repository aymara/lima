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
 * @file       transitionUnit.h
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Fri Oct 04 2002
 * copyright   Copyright (C) 2002 by CEA LIST
 * Project     Automaton
 * 
 * @brief      abstract base class for generic transitions
 * 
 ***********************************************************************/

#ifndef TRANSITIONUNIT_H
#define TRANSITIONUNIT_H

#include "AutomatonExport.h"
#include "common/Data/LimaString.h"
#include "constraint.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "common/ProcessUnitFramework/AnalysisContent.h"

#include <vector>

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

// the Transition class is a abstract base class : it is instantiated
// as a transition of one of the following transition types
typedef enum {
  T_EPSILON,
  T_STAR,
  T_WORD,
  T_POS,
  T_LEMMA,
  T_NUM,
  T_TSTATUS,
  T_AND,
  T_SET,
  T_DEACCENTUATED,
  T_ENTITY,
  T_ENTITY_GROUP,
  T_GAZETEER
} TypeTransition; // useful for the read/write functions

class LIMA_AUTOMATON_EXPORT TransitionUnit
{
 public:
  TransitionUnit();
  TransitionUnit(const bool keep, const bool negative=false);
  TransitionUnit(const TransitionUnit& transition);
  virtual ~TransitionUnit();
  TransitionUnit& operator = (const TransitionUnit& transition);

  virtual TransitionUnit* clone() const =0;
  virtual TransitionUnit* create() const =0;

  bool keep() const;
  void setKeep(const bool);
  bool negative() const;
  void setNegative(const bool);
  bool head() const;
  bool isHead() const;
  void setHead(const bool);
  const std::string& getId() const;
  void setId(const std::string& id);

  const Constraint& constraint(const uint64_t) const;
  uint64_t numberOfConstraints() const;
  void addConstraint(const Constraint&);
  void copyProperties(const TransitionUnit& t);
  bool compareProperties(const TransitionUnit& t) const;
  virtual std::string printValue() const { return ""; }
  virtual TypeTransition type() const =0;

  virtual bool operator== (const TransitionUnit&) const =0;
  
  /** 
   * comparison of the transition with a token : can use the graph and
   * vertex info, associated with general analysis content. Takes also
   * the token and morphosyntacticData associated to the vertex so
   * that this access is not done for each transition when testing a
   * vertex
   * 
   * @param graph the graph
   * @param vertex the vertex to compare 
   * @param token the token associated to the vertex
   * @param data the data associated to the vertex
   * 
   * @return 
   */
  virtual bool compare(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                       const LinguisticGraphVertex& vertex,
                       AnalysisContent& analysis,
                       const LinguisticAnalysisStructure::Token* token,
                       const LinguisticAnalysisStructure::MorphoSyntacticData* data) const =0;

  // check constraints
  bool checkConstraints(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                        const LinguisticGraphVertex& vertex,
                        AnalysisContent& analysis, 
                        ConstraintCheckList&) const;

  void popConstraints(ConstraintCheckList&) const;
  
  virtual bool isEpsilonTransition() const { return false; }
  
  friend LIMA_AUTOMATON_EXPORT std::ostream& operator << (std::ostream& os, const TransitionUnit& t);
  friend LIMA_AUTOMATON_EXPORT QDebug& operator << (QDebug& os, const TransitionUnit& t);
  
 protected:
  bool m_keep; // indicates if the transition must be kept 
               // in the recognized expression
  bool m_negative;  // indicates if the transition is negative
  bool m_head;
  std::string m_id;   // id of transition = ruleElementId
  std::vector<Constraint> m_constraints;
};

/***********************************************************************/
// inline access functions
/***********************************************************************/
inline bool TransitionUnit::keep() const { return m_keep; }
inline void TransitionUnit::setKeep(const bool keep) { m_keep = keep; }
inline bool TransitionUnit::negative() const { return m_negative; }
inline void TransitionUnit::setNegative(const bool negative) { m_negative = negative; }
inline bool TransitionUnit::head() const { return m_head; }
inline bool TransitionUnit::isHead() const { return m_head; }
inline void TransitionUnit::setHead(const bool h) { m_head = h; }
inline const std::string& TransitionUnit::getId() const { return m_id; }
inline void TransitionUnit::setId(const std::string& id) { m_id = id; }

inline const Constraint& TransitionUnit::constraint(const uint64_t i) const {
  return m_constraints[i];
}
inline uint64_t TransitionUnit::numberOfConstraints() const {
  return m_constraints.size();
}
inline void TransitionUnit::addConstraint(const Constraint& c) {
  m_constraints.push_back(c);
}

} // namespace end
} // namespace end
} // namespace end

#endif
