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
 * @file       numericTransition.h
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Tue Oct 29 2002
 * copyright   Copyright (C) 2002 by CEA LIST
 * Project     Automaton
 * 
 * @brief      numeric transition (can be en interval value)
 * 
 ***********************************************************************/

#ifndef NUMERICTRANSITION_H
#define NUMERICTRANSITION_H

#include "AutomatonExport.h"
#include <iostream>
#include <limits>
#include "linguisticProcessing/core/Automaton/transitionUnit.h"

#ifdef WIN32
#undef min
#undef max
#endif
namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

class LIMA_AUTOMATON_EXPORT NumericTransition : public TransitionUnit
{
 public:
  NumericTransition(); 
  NumericTransition(uint64_t, uint64_t, uint64_t, bool keep=true); 
  NumericTransition(const NumericTransition&);
  virtual ~NumericTransition();
  NumericTransition& operator = (const NumericTransition&);
  
  NumericTransition* clone() const;
  NumericTransition* create() const;

  std::string printValue() const;
  bool operator== (const TransitionUnit&) const;
  bool operator== (const uint64_t&) const;

  bool compare(const LinguisticAnalysisStructure::AnalysisGraph& graph,
               const LinguisticGraphVertex& vertex,
               AnalysisContent& analysis,
               const LinguisticAnalysisStructure::Token* token,
               const LinguisticAnalysisStructure::MorphoSyntacticData* data) const;
  
  TypeTransition type() const;

  uint64_t value() const;
  uint64_t min() const;
  uint64_t max() const;
  void setValue(uint64_t);
  void setMin(uint64_t);
  void setMax(uint64_t);
  void setMinMax(uint64_t, uint64_t);
  bool isInterval() const;

  static const uint64_t noValue ;
  
 private:
  uint64_t m_value;
  uint64_t m_min; 
  uint64_t m_max;
};

/***********************************************************************/
// inline access functions
/***********************************************************************/
inline uint64_t NumericTransition::value() const { return m_value; }
inline uint64_t NumericTransition::min() const { return m_min; }
inline uint64_t NumericTransition::max() const { return m_max; }
inline void NumericTransition::setValue(uint64_t val) { m_value = val; }
inline void NumericTransition::setMin(uint64_t min) { m_min = min; }
inline void NumericTransition::setMax(uint64_t max) { m_max = max; }
inline void NumericTransition::setMinMax(uint64_t min,
                     uint64_t max) {
  m_min = min;
  m_max = max; }

/***********************************************************************/
// other inline functions
/***********************************************************************/
inline NumericTransition* NumericTransition::clone() const { 
  return new NumericTransition(*this); } 
inline NumericTransition* NumericTransition::create() const {
  return new NumericTransition(); }

inline TypeTransition NumericTransition::type() const { return T_NUM; }

inline bool NumericTransition::isInterval() const { 
  if (m_value == noValue) { return true; }
  return false;
}


} // end namespace
} // end namespace
} // end namespace

#endif
