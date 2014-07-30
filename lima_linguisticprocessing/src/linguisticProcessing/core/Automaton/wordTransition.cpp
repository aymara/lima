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
* File        : wordTransition.cpp
* Author      : Romaric Besancon (besanconr@zoe.cea.fr)
* Created on  : Mon Oct 14 2002
* Copyright   : (c) 2002 by CEA
* Version     : $Id$
*
*************************************************************************/


#include "wordTransition.h"
#include "common/MediaticData/mediaticData.h"
#include <iostream>

using namespace std;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::Common::MediaticData;


namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

/***********************************************************************/
// constructors
/***********************************************************************/
WordTransition::WordTransition():
TransitionUnit(),
m_word() 
{
}

WordTransition::WordTransition(Tword s, bool keep):
TransitionUnit(keep),
m_word(s) 
{
}

WordTransition::WordTransition(const WordTransition& t):
TransitionUnit(t),
m_word(t.word()) 
{
//   copyProperties(t);
}

WordTransition::~WordTransition() {}

WordTransition& WordTransition::operator = (const WordTransition& t) {
  if (this != &t) {
    m_word = t.word();
    copyProperties(t);
  }
  return *this;
}


std::string WordTransition::printValue() const {
  ostringstream oss;
//   const FsaStringsPool& sp=Common::MediaticData::MediaticData::single().stringsPool(0);
//   LimaString s = sp[(StringsPoolIndex)m_word];
//   oss << Common::Misc::limastring2utf8stdstring(s);
  oss << m_word;
  return oss.str();
}

LimaString WordTransition::wordString(const FsaStringsPool& sp) const {
  return sp[m_word];
}

/***********************************************************************/
// operators ==
/***********************************************************************/
bool WordTransition::operator== (const TransitionUnit& tright) const {
  if ( (type() == tright.type())
       && (m_word == static_cast<const WordTransition&>(tright).word())
       ) {
    return compareProperties(tright);
  }
  else {
    return false;
  }
}

bool WordTransition::
compare(const LinguisticAnalysisStructure::AnalysisGraph& /*graph*/,
        const LinguisticGraphVertex& /*vertex*/,
        AnalysisContent& /*analysis*/,
        const LinguisticAnalysisStructure::Token* token,
        const LinguisticAnalysisStructure::MorphoSyntacticData* data) const
{
  //AULOGINIT;
//   LDEBUG << "WordTransition compare " << Common::MediaticData::MediaticData::changeable().stringsPool()[token->form()] << " and " << Common::MediaticData::MediaticData::changeable().stringsPool()[m_word];
  if (token->form() == m_word) {
//     LDEBUG << "true";
    return true;
  }
/*  std::vector<StringsPoolIndex>::const_iterator
    it=token->orthographicAlternatives().begin(),
    it_end=token->orthographicAlternatives().end();
  for (; it!=it_end; it++) {
    if (*it == m_word) {
      return true;
    }
  }*/
  for (MorphoSyntacticData::const_iterator it=data->begin();
       it!=data->end();
       it++)
  {
    if (it->inflectedForm == m_word) {
      return true;
    }
  }
//   LDEBUG << "false";
  return false;
}

} // namespace end
} // namespace end
} // namespace end
