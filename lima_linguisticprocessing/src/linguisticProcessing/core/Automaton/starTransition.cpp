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
* File        : starTransition.cpp
* Author      : Besancon Romaric (besanconr@zoe.cea.fr)
* Created on  : Mon Nov 18 2002
* Copyright   : (c) 2002 by CEA
* Version     : $Id$
*
*************************************************************************/

#include "starTransition.h"
#include "common/Data/strwstrtools.h"
#include <iostream>
#include <fstream>

using namespace std;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

/***********************************************************************/
// constructors
/***********************************************************************/
StarTransition::StarTransition():TransitionUnit() {} 
StarTransition::StarTransition(const bool keep):TransitionUnit(keep) {} 
StarTransition::StarTransition(const StarTransition& t):
TransitionUnit(t) 
{
}
StarTransition::StarTransition(const TransitionUnit& t):
TransitionUnit(t) 
{
}

/***********************************************************************/
// destructor
/***********************************************************************/
StarTransition::~StarTransition() {}

/***********************************************************************/
// assignment operator
/***********************************************************************/
StarTransition& StarTransition::operator = (const StarTransition& t) {   
  if (this != &t) {
    copyProperties(t);
  }
  return *this;
}

/***********************************************************************/
// comparison tests
/***********************************************************************/
bool StarTransition::operator== (const TransitionUnit& t) const {
  if ( type() == t.type() ) { 
    return compareProperties(t); 
  }
  else { return false; }
} 

bool StarTransition::
compare(const LinguisticAnalysisStructure::AnalysisGraph& /*graph*/,
        const LinguisticGraphVertex& /*vertex*/,
        AnalysisContent& /*analysis*/,
        const LinguisticAnalysisStructure::Token* /*token*/,
        const LinguisticAnalysisStructure::MorphoSyntacticData* /*data*/) const
{
  return true;
}

std::string StarTransition::printValue() const { 
  return "starT"; 
}

} // namespace end
} // namespace end
} // namespace end
