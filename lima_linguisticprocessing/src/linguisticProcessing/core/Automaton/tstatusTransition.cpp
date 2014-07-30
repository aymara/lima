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
* File        : tstatusTransition.cpp
* Author      : Romaric Besan�n (besanconr@zoe.cea.fr)
* Created on  : Mon Jan 13 2003
* Copyright   : (c) 2002 by CEA
* Version     : $Id$
*
*************************************************************************/

#include "tstatusTransition.h"

#include "linguisticProcessing/LinguisticProcessingCommon.h"

#include <iostream>

using namespace std;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

/***********************************************************************/
// constructors
/***********************************************************************/
TStatusTransition::TStatusTransition():
  TransitionUnit(),
  m_status() 
{}

TStatusTransition::TStatusTransition(TStatus s, bool keep):
  TransitionUnit(keep),
  m_status(s) 
{}

TStatusTransition::TStatusTransition(const TStatusTransition& t):
  TransitionUnit(t),
  m_status(t.status()) 
{
//   copyProperties(t);
}

TStatusTransition::~TStatusTransition() {}

TStatusTransition& TStatusTransition::operator = (const TStatusTransition& t) {
  if (this != &t) {
    m_status = t.status();
    copyProperties(t);
  }
  return *this;
}


std::string TStatusTransition::printValue() const { 
  ostringstream oss;
  switch (m_status.getStatus()) {
  case T_ALPHA : {
    oss << "T_alpha";
    switch (m_status.getAlphaCapital()) {
    case T_CAPITAL       : oss << ";capital"      ; break;
    case T_SMALL         : oss << ";small"        ; break;
    case T_CAPITAL_1ST   : oss << ";capital_1st"  ; break;
    case T_ACRONYM       : oss << ";acronym"      ; break;
    case T_CAPITAL_SMALL : oss << ";capital_small"; break;
    default : break;
    }
    switch (m_status.getAlphaRoman()) { // Roman supersedes Cardinal
    case T_CARDINAL_ROMAN : oss << ";cardinal_roman"; break;
    case T_ORDINAL_ROMAN  : oss << ";ordinal_roman" ; break;
    case T_NOT_ROMAN      : oss << ";not_roman"     ; break;
    default : break;
    }
    if (m_status.isAlphaHyphen()) {
      oss << ";hyphen";
    }
    if (m_status.isAlphaPossessive()) {
      oss << ";possessive";
    }
    break;
  }
  case T_NUMERIC : {
    oss << "T_numeric";
    switch (m_status.getNumeric()) {
    case T_INTEGER         : oss << ";integer"       ; break;
    case T_COMMA_NUMBER    : oss << ";comma_number"  ; break;
    case T_DOT_NUMBER      : oss << ";dot_number"    ; break;
    case T_FRACTION        : oss << ";fraction"      ; break;
    case T_ORDINAL_INTEGER : oss << ";ordinal_integer"; break;
    default: break;
    }
    break;
  }
  case T_ALPHANUMERIC : oss << "T_alphanumeric" ; break;
  case T_PATTERN      : oss << "T_pattern"      ; break;
  case T_WORD_BRK     : oss << "T_word_brk"     ; break;
  case T_SENTENCE_BRK : oss << "T_sentence_brk" ; break;
  case T_PARAGRAPH_BRK : oss << "T_paragraph_brk" ; break;
  default : 
    if (m_status.defaultKey().isEmpty()) {
      oss << "T_no_status"; 
    }
    break;
  }
  if (!m_status.defaultKey().isEmpty()) {
    oss << "T_default=" << Common::Misc::limastring2utf8stdstring(m_status.defaultKey());
  }
  return oss.str(); 
}

/***********************************************************************/
// operators ==
/***********************************************************************/
bool TStatusTransition::operator== (const TransitionUnit& tright) const {
  if ( (type() == tright.type())  
       && (compareTStatus((static_cast<const TStatusTransition&>(tright).status())))) {
    return compareProperties(tright);
  }
  else { 
    return false; 
  }
}

bool TStatusTransition::
compare(const LinguisticAnalysisStructure::AnalysisGraph& /*graph*/,
        const LinguisticGraphVertex& /*vertex*/,
        AnalysisContent& /*analysis*/,
        const LinguisticAnalysisStructure::Token* token,
        const LinguisticAnalysisStructure::MorphoSyntacticData* /*data*/) const
{
  return (compareTStatus(token->status()));
}

bool TStatusTransition::compareTStatus(const Lima::LinguisticProcessing::LinguisticAnalysisStructure::TStatus& t) const {
    AULOGINIT;
    LDEBUG << "TStatusTransition::compareTStatus";
    LDEBUG << "   rule  status: " << m_status.getStatus() << " ; " << m_status.defaultKey();
    LDEBUG << "   token status: " << t.getStatus() << " ; " << t.defaultKey();

//   if (t == 0) { LDEBUG << "   FALSE"; return false; }
  if (t == TStatus()) {
    LDEBUG << "   " << (t == m_status);
    return (t == m_status);
  }
  
  if (!m_status.defaultKey().isEmpty()
    && m_status.defaultKey() != Common::Misc::utf8stdstring2limastring("t_fallback") ) {
    LDEBUG << "   " << ((m_status.defaultKey()==t.defaultKey()) ? "TRUE" : "FALSE");
    return (m_status.defaultKey()==t.defaultKey());
  }

  if (m_status.getStatus() != t.getStatus()) {
    LDEBUG << "   FALSE";
    return false;
  }
  switch(m_status.getStatus()) {
  case T_ALPHA : {
    if (m_status.getAlphaCapital() != T_NULL_CAPITAL &&
        m_status.getAlphaCapital() != t.getAlphaCapital() ) {
      LDEBUG << "   FALSE";
      return false;
    }
    if (m_status.getAlphaRoman() != T_NULL_ROMAN &&
        m_status.getAlphaRoman() != m_status.getAlphaRoman() ) {
      LDEBUG << "   FALSE";
      return false;
    }
    if (m_status.isAlphaHyphen() && (! t.isAlphaHyphen())) {
      LDEBUG << "   FALSE";
      return false;
    }
    if (m_status.isAlphaPossessive() && (! t.isAlphaPossessive())) {
      LDEBUG << "   FALSE";
      return false;
    }
    break;
  }
  case T_NUMERIC : {
    if (m_status.getNumeric() != T_NULL_NUM &&
        m_status.getNumeric() != t.getNumeric() ) {
      LDEBUG << "   FALSE";
      return false;
    }
    break;
  }
  case T_NULL_CAPITAL: {
    if (m_status.defaultKey() != t.defaultKey()) {
      LDEBUG << "   FALSE";
      return false;
    }
    break;
  }
  default: {
    LDEBUG << "   TRUE";
    return true;
  }
  }
  LDEBUG << "   TRUE";
  return true;
}

} // namespace end
} // namespace end
} // namespace end
