// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
