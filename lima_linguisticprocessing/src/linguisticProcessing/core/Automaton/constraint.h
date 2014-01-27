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
 * @file       constraint.h
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Tue Nov 26 2002
 * copyright   Copyright (C) 2002 by CEA LIST
 * Project     Automaton
 * 
 * @brief      this class is the description of the constraints applied 
 * on the transitions of the automata
 * 
 * 
 ***********************************************************************/

#ifndef CONSTRAINT_H
#define CONSTRAINT_H

#include "AutomatonExport.h"
#include "constraintFunction.h"
#include "constraintCheckList.h"
#include "automatonReaderWriter.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "common/ProcessUnitFramework/AnalysisContent.h"
#include "common/Data/LimaString.h"
#include <vector>
#include <fstream>

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

class LIMA_AUTOMATON_EXPORT Constraint
{
friend class AutomatonReader;
friend class AutomatonWriter;

 public:
  Constraint(); 
  Constraint(const uint64_t i, 
             const std::string& functionName, 
             const ConstraintAction a,
             MediaId language,
             const LimaString& complement=LimaString(),
             const bool negative=false); 
  Constraint(const Constraint&);
  ~Constraint();
  Constraint& operator = (const Constraint&);
  
  friend LIMA_AUTOMATON_EXPORT std::ostream& operator << (std::ostream&, const Constraint&);
  friend LIMA_AUTOMATON_EXPORT QDebug& operator << (QDebug&, const Constraint&);
  friend bool operator == (const Constraint&,const Constraint&);

  static const uint64_t noindex=UINT_MAX;

  void reinit();
  
  uint64_t index() const;
  ConstraintFunction* functionAddr() const;
  std::string functionName() const;
  ConstraintAction action() const;
  LimaString str() const;
  void setIndex(const uint64_t);
  void setFunctionAddr(ConstraintFunction* f);
  void setAction(const ConstraintAction);

  /** 
   * check a constraint
   * 
   * @param graph the graph of the analysis
   * @param vertex the current vertex
   * @param analysis the current result of the analysis (may be modified)
   * @param constraintCheckList the constraint checklist
   * 
   * @return 
   */
  bool checkConstraint(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                       const LinguisticGraphVertex& vertex,
                       AnalysisContent& analysis,
                       ConstraintCheckList& constraintCheckList) const;

  /** 
   * apply an action  (only for EXECUTE type of action)
   * 
   * @param graph the graph 
   * @param analysis the current result of the analysis (may be modified)
   * @param constraintCheckList the current constraint checklist
   * @param success a boolean indicating if the rule succeeded or not
   * (action is conditionned by that)
   * @param result the expression recognized by the rule, if found 
   * (actions are applied after end of rule)
   * 
   * @return 
   */
  bool apply(const LinguisticAnalysisStructure::AnalysisGraph& graph,
             AnalysisContent& analysis,
             ConstraintCheckList& constraintCheckList,
             const bool success,
             RecognizerMatch* result) const;

 private:
  uint64_t m_index;  /**< index of the constraint in the constraint check
                            list of the rule */
  ConstraintFunction* m_functionAddr; /**< function defining the constraint */
  ConstraintAction m_action;  /**< action to be performed */
  bool m_negative;              /**< check the inverse of the action */

  // private functions 
  void writeConstraintAction(std::ofstream& file,
                             const ConstraintAction& a);
  ConstraintAction readConstraintAction(std::ifstream& file);

  // for debug output
  std::string checkStringDebug(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                               const LinguisticGraphVertex vertex) const;
  std::string applyStringDebug(const LinguisticGraphVertex firstArg=0,
                               const LinguisticGraphVertex secondArg=0) const;
  std::string actionString() const;
  
};

//**********************************************************************
// inline functions
inline uint64_t Constraint::index() const { return m_index; }
inline ConstraintAction Constraint::action() const { return m_action; }
inline void Constraint::setIndex(const uint64_t i) { m_index = i; }
inline void Constraint::setAction(const ConstraintAction a) { m_action = a; }

inline ConstraintFunction* Constraint::functionAddr() const { 
  return m_functionAddr; 
}
inline void Constraint::setFunctionAddr(ConstraintFunction* f) { 
  m_functionAddr=f; 
}



} // end namespace
} // end namespace
} // end namespace


#endif
