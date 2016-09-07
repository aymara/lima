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
  NumericTransition(double, double, double, bool keep=true); 
  NumericTransition(const NumericTransition&);
  virtual ~NumericTransition();
  NumericTransition& operator = (const NumericTransition&);
  
  NumericTransition* clone() const;
  NumericTransition* create() const;

  std::string printValue() const;
  bool operator== (const TransitionUnit&) const;
  bool operator== (const double&) const;

  bool compare(const LinguisticAnalysisStructure::AnalysisGraph& graph,
               const LinguisticGraphVertex& vertex,
               AnalysisContent& analysis,
               const LinguisticAnalysisStructure::Token* token,
               const LinguisticAnalysisStructure::MorphoSyntacticData* data) const;
  
  TypeTransition type() const;

  double value() const;
  double min() const;
  double max() const;
  void setValue(double);
  void setMin(double);
  void setMax(double);
  void setMinMax(double, double);
  bool isInterval() const;

  static const double noValue ;
  
 private:
  double m_value;
  double m_min; 
  double m_max;
};

/***********************************************************************/
// inline access functions
/***********************************************************************/
inline double NumericTransition::value() const { return m_value; }
inline double NumericTransition::min() const { return m_min; }
inline double NumericTransition::max() const { return m_max; }
inline void NumericTransition::setValue(double val) { m_value = val; }
inline void NumericTransition::setMin(double min) { m_min = min; }
inline void NumericTransition::setMax(double max) { m_max = max; }
inline void NumericTransition::setMinMax(double min,
                     double max) {
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
