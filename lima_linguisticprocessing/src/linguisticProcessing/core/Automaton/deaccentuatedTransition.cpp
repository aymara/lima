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
 * @file       deaccentuatedTransition.cpp
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Mon Jan  3 2005
 * @version    $Id$
 * copyright   Copyright (C) 2005-2012 by CEA LIST
 * 
 ***********************************************************************/

#include "deaccentuatedTransition.h"
#include "common/Data/strwstrtools.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/core/FlatTokenizer/CharChart.h"

using namespace std;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

//***********************************************************************
// constructors
//***********************************************************************
DeaccentuatedTransition::DeaccentuatedTransition():
TransitionUnit(),
m_deaccentuatedForm(),
m_language(0),
m_charchart(0)
{
}

DeaccentuatedTransition::
DeaccentuatedTransition(const LimaString& deaccentuatedForm,
                        MediaId language):
TransitionUnit(),
m_deaccentuatedForm(deaccentuatedForm),
m_language(language),
m_charchart(0)
{
  AbstractResource* res=LinguisticResources::single().
    getResource(m_language,"charchart");
  m_charchart=static_cast<FlatTokenizer::CharChart*>(res);
}

DeaccentuatedTransition::
DeaccentuatedTransition(const DeaccentuatedTransition& t):
TransitionUnit(t),
m_deaccentuatedForm(t.m_deaccentuatedForm),
m_language(t.m_language),
m_charchart(0)
{
  AbstractResource* res=LinguisticResources::single().
    getResource(m_language,"charchart");
  m_charchart=static_cast<FlatTokenizer::CharChart*>(res);
}
  
//***********************************************************************
// destructor
//***********************************************************************
DeaccentuatedTransition::~DeaccentuatedTransition() {
}

//***********************************************************************
// assignment operator
//***********************************************************************
DeaccentuatedTransition& DeaccentuatedTransition::
operator = (const DeaccentuatedTransition& t) {
  if (this != &t) {
    TransitionUnit::operator=(t);
    m_deaccentuatedForm=t.getDeaccentuatedForm();
    m_language=t.m_language;
    AbstractResource* res=LinguisticResources::single().
      getResource(m_language,"charchart");
    m_charchart=static_cast<FlatTokenizer::CharChart*>(res);
  }
  return *this;
}

//***********************************************************************
// member functions
//***********************************************************************
DeaccentuatedTransition* DeaccentuatedTransition::clone() const {
  return new DeaccentuatedTransition(*this);
}
DeaccentuatedTransition* DeaccentuatedTransition::create() const {
  return new DeaccentuatedTransition();
}

TypeTransition DeaccentuatedTransition::type() const {
  return T_DEACCENTUATED;
}
  
std::string DeaccentuatedTransition::printValue() const {
  return string("~")+Common::Misc::limastring2utf8stdstring(m_deaccentuatedForm);
}

bool DeaccentuatedTransition::operator== (const TransitionUnit& t) const {
  if (type() == t.type() && 
      m_deaccentuatedForm == 
      static_cast<const DeaccentuatedTransition&>(t).
      getDeaccentuatedForm()) {
    return compareProperties(t);
  }
  return false;
}
  
bool DeaccentuatedTransition::
compare(const LinguisticAnalysisStructure::AnalysisGraph& /*graph*/,
        const LinguisticGraphVertex& /*vertex*/,
        AnalysisContent& /*analysis*/,
        const LinguisticAnalysisStructure::Token* token,
        const LinguisticAnalysisStructure::MorphoSyntacticData* /*data*/) const
{
  return (m_charchart->unmark(token->stringForm()) == m_deaccentuatedForm);
}
  
} // end namespace
} // end namespace
} // end namespace
