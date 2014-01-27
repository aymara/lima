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
* File        : epsilonTransition.cpp
* Author      : Besancon Romaric (besanconr@zoe.cea.fr)
* Created on  : Tue Oct 15 2002
* Copyright   : (c) 2002 by CEA
* Version     : $Id$
*
*************************************************************************/


#include "epsilonTransition.h"
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
EpsilonTransition::EpsilonTransition():TransitionUnit() {} 
EpsilonTransition::EpsilonTransition(const EpsilonTransition& t):
TransitionUnit(t) {}

/***********************************************************************/
// destructor
/***********************************************************************/
EpsilonTransition::~EpsilonTransition() {}

/***********************************************************************/
// assignment operator
/***********************************************************************/
EpsilonTransition& EpsilonTransition::operator = (const EpsilonTransition& e) 
{ 
  TransitionUnit::operator=(e);
  return *this; 
}

/***********************************************************************/
// comparison operators and functions
/***********************************************************************/
bool EpsilonTransition::operator== (const TransitionUnit& t) const {
  if ( type() == t.type() ) { 
    return true; 
  }
  return false;
} 

bool EpsilonTransition::
compare(const LinguisticAnalysisStructure::AnalysisGraph& /*graph*/,
        const LinguisticGraphVertex& /*vertex*/,
        AnalysisContent& /*analysis*/,
        const LinguisticAnalysisStructure::Token* /*token*/,
        const LinguisticAnalysisStructure::MorphoSyntacticData* /*data*/) const
{
  return false;
}

} // namespace end
} // namespace end
} // namespace end
