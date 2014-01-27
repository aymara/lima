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
* File        : setTransition.cpp
* Author      : Romaric Besan�n (besanconr@zoe.cea.fr)
* Created on  : Fri Sept 19 2003
* Copyright   : (c) 2003 by CEA
*
*************************************************************************/


#include "setTransition.h"
#include <iostream>
#include <sstream>

using namespace std;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

namespace Lima
{
namespace LinguisticProcessing
{
namespace Automaton
{

/***********************************************************************/
// constructors
/***********************************************************************/
SetTransition::SetTransition():
    TransitionUnit(),
    m_words()
{}

SetTransition::SetTransition(const std::set<Tword>& words,
                               bool keep):
      TransitionUnit(keep),
      m_words(words)
{}

SetTransition::SetTransition(const SetTransition& t):
    TransitionUnit(t),
    m_words()
{
  copy(t);
}

/***********************************************************************/
// destructor
/***********************************************************************/
SetTransition::~SetTransition()
{
  clean();
}

/***********************************************************************/
// copy
/***********************************************************************/
SetTransition& SetTransition::operator = (const SetTransition& t)
{
  if (this != &t)
  {
    clean();
    copy(t);
    copyProperties(t);
  }
  return *this;
}

//helper functions for copy constructor and assignment operator
void SetTransition::clean()
{
  m_words.clear();
}

void SetTransition::copy(const SetTransition& t)
{
  m_words=t.m_words;
}

std::string SetTransition::printValue() const
{
  ostringstream oss;
  oss << "SET:";
  if (m_words.empty())
  {
    return oss.str();
  }
  set<Tword>::const_iterator w=m_words.begin();
  oss << *w;
  w++;
  for (; w!=m_words.end(); w++)
  {
    oss << ";" << *w;
  }
  return oss.str();
}


/***********************************************************************/
// comparison tests
/***********************************************************************/
bool SetTransition::operator== (const TransitionUnit& tright) const
{
  if (type() == tright.type())
  {
    const SetTransition& other=static_cast<const SetTransition&>(tright);
    if ( m_words == other.m_words)
    {
      return compareProperties(tright);
    }
  }
  return false;
}

bool SetTransition::
compare(const LinguisticAnalysisStructure::AnalysisGraph& /*graph*/,
        const LinguisticGraphVertex& /*vertex*/,
        AnalysisContent& /*analysis*/,
        const LinguisticAnalysisStructure::Token* token,
        const LinguisticAnalysisStructure::MorphoSyntacticData* data) const
{
  if (m_words.find(token->form())!=m_words.end())
  {
    return true;
  }
/*  std::vector<StringsPoolIndex>::const_iterator
  it=token->orthographicAlternatives().begin(),
  it_end=token->orthographicAlternatives().end();
  for (; it!=it_end; it++)
  {
    if (m_words.find(*it)!=m_words.end())
    {
      return true;
    }
  }*/
  
  StringsPoolIndex current = static_cast<StringsPoolIndex>(0);
  for (MorphoSyntacticData::const_iterator it=data->begin();
       it!=data->end();
       it++)
  {
    if (it->inflectedForm == current) continue;
    current = it->inflectedForm;
    if (m_words.find(current)!=m_words.end())
    {
      return true;
    }
  }
  return false;
}

} // namespace end
} // namespace end
} // namespace end
