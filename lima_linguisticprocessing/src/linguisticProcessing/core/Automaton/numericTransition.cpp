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
* File        : numericTransition.cpp
* Author      : Besancon Romaric (besanconr@zoe.cea.fr)
* Created on  : Tue Oct 29 2002
* Copyright   : (c) 2002 by CEA
* Version     : $Id$
*
*************************************************************************/


#include "numericTransition.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/TStatus.h"
#include "common/Data/strwstrtools.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

const uint64_t NumericTransition::noValue = std::numeric_limits< uint64_t >::max();

/***********************************************************************/
// constructors
/***********************************************************************/
NumericTransition::NumericTransition():
  TransitionUnit(),m_value(noValue),m_min(noValue),m_max(noValue) {}
  
NumericTransition::NumericTransition(uint64_t val, 
                     uint64_t min, 
                     uint64_t max,
                     bool keep):
  TransitionUnit(keep),
  m_value(val),
  m_min(min),
  m_max(max) {
}
NumericTransition::NumericTransition(const NumericTransition& t):
  TransitionUnit(t),
  m_value(t.value()),
  m_min(t.min()),
  m_max(t.max()) {
//   copyProperties(t);
}

/***********************************************************************/
// destructor
/***********************************************************************/
NumericTransition::~NumericTransition() {}

/***********************************************************************/
// operators
/***********************************************************************/
NumericTransition& NumericTransition::operator = (const NumericTransition& t) {
  if (this != &t) {
    m_value = t.value();
    m_min = t.min();
    m_max = t.max();
    copyProperties(t);
  }
  return *this;
}

bool NumericTransition::operator== (const TransitionUnit& tright) const {
  if (type() == tright.type()) { 
    const NumericTransition& other=
      static_cast<const NumericTransition&>(tright);
    if (m_value == other.value() 
        && (m_min == other.min())
        && (m_max == other.max())) {
      return compareProperties(tright);
    }
  }
  return false; 
}

bool NumericTransition::operator== (const uint64_t& numValue) const {
  if (isInterval()) {
    if (m_min == noValue) {
      return ( numValue <= m_max);
    }
    else if (m_max == noValue) {
      return ( numValue >= m_min);
    }
    else {
      return ( numValue >= m_min && numValue <= m_max);
    }
  }
  else {
    return (m_value == numValue);
  }
}
  
bool NumericTransition::
compare(const LinguisticAnalysisStructure::AnalysisGraph& /*graph*/,
        const LinguisticGraphVertex& /*vertex*/,
        AnalysisContent& /*analysis*/,
        const LinguisticAnalysisStructure::Token* token,
        const LinguisticAnalysisStructure::MorphoSyntacticData* /*data*/) const
{

  const TStatus& status = token->status();
  if (status.getNumeric() != T_INTEGER) {
    return false;
  }
  uint64_t numValue(token->stringForm().toULong());
  return (*this == numValue);
}

/***********************************************************************/
// output
/***********************************************************************/

std::string NumericTransition::printValue() const {
  ostringstream oss;
  if (isInterval()) {
    oss << "numericT[";
    if (m_min == noValue) { oss << "-"; }
    else { oss << m_min; }
    oss << "," ;
    if (m_max == noValue) { oss << "-"; }
    else { oss << m_max; }
    oss << "]";
  }
  else {
    oss << "numericT(" << m_value << ")";
  }
  return oss.str();
}


} // namespace end
} // namespace end
} // namespace end
