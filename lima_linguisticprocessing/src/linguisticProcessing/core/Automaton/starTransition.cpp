// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
