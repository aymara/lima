/*
    Copyright 2002-2019 CEA LIST

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
/** @brief          The status of a token found by Tokenizer for the use of
  *                 Morphologic Analysis modules
  *
  * @file           TStatus.cpp
  * @author         Jean-Yves Sages (NAUTITIA)

  *                 Copyright 2002,2003 by CEA
  * @date           Created on Jul, 4 2002
  * @version        $Id$
  */

#include "TStatus.h"
#include "common/Data/strwstrtools.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"

namespace Lima {
namespace LinguisticProcessing {
namespace LinguisticAnalysisStructure {

class TStatusPrivate
{
  friend class TStatus;

public:
    TStatusPrivate();
    TStatusPrivate(const TStatusPrivate&);
    TStatusPrivate(
        AlphaCapitalType capital,
        AlphaRomanType roman,
        bool isHyphen,
        bool isPossessive,
        bool isConcatAbbrev,
        bool isTwitter,
        NumericType numeric,
        StatusType status);
    virtual ~TStatusPrivate();

    // Assignment operator. Copies the specified TStatus
    // into self. Returns a reference to self.
    TStatusPrivate& operator=(const TStatusPrivate&);

    // comparison operator (so that status can be sorted)
    bool operator<(const TStatusPrivate& s) const;
    bool operator==(const TStatusPrivate& s) const;

    AlphaCapitalType _capital;
    AlphaRomanType _roman;
    bool _isHyphen;
    bool _isPossessive;
    bool m_isConcatAbbrev;
    bool m_isTwitter;
    NumericType _numeric;
    StatusType _status;
    std::vector<Lima::LimaString> _defaultKey;

};

TStatusPrivate::TStatusPrivate() :
_capital(T_NULL_CAPITAL),
_roman(T_NULL_ROMAN),
_isHyphen(false),
_isPossessive(false),
m_isConcatAbbrev(false),
m_isTwitter(false),
_numeric(T_NULL_NUM),
_status(T_NULL_STATUS),
_defaultKey() {
  _defaultKey.push_back(Lima::Common::Misc::utf8stdstring2limastring("t_fallback"));
}

TStatusPrivate::TStatusPrivate(const TStatusPrivate& tStatus) :
    _capital(tStatus._capital),
    _roman(tStatus._roman),
    _isHyphen(tStatus._isHyphen),
    _isPossessive(tStatus._isPossessive),
    m_isConcatAbbrev(tStatus.m_isConcatAbbrev),
    m_isTwitter(tStatus.m_isTwitter),
    _numeric(tStatus._numeric),
    _status(tStatus._status),
    _defaultKey(tStatus._defaultKey)
{
}

TStatusPrivate::TStatusPrivate(AlphaCapitalType capital,
                 AlphaRomanType roman,
                 bool isHyphen,
                 bool isPossessive,
                 bool isConcatAbbrev,
                 bool isTwitter,
                 NumericType numeric,
                 StatusType status) :
_capital(capital),
_roman(roman),
_isHyphen(isHyphen),
_isPossessive(isPossessive),
m_isConcatAbbrev(isConcatAbbrev),
m_isTwitter(isTwitter),
_numeric(numeric),
_status(status),
_defaultKey()  {
  _defaultKey.push_back(Lima::Common::Misc::utf8stdstring2limastring("t_fallback"));
}

TStatusPrivate::~TStatusPrivate() {}

// Assignment operator. Copies the specified TStatus
// into self. Returns a reference to self.
TStatusPrivate& TStatusPrivate::operator=(const TStatusPrivate& tStatus) {
  if (this != &tStatus) {
    _capital = tStatus._capital;
    _roman = tStatus._roman;
    _isHyphen = tStatus._isHyphen;
    _isPossessive = tStatus._isPossessive;
    m_isConcatAbbrev = tStatus.m_isConcatAbbrev;
    m_isTwitter = tStatus.m_isTwitter;
    _numeric = tStatus._numeric;
    _status = tStatus._status;
    _defaultKey = tStatus._defaultKey;
  }
  return *this;
}

bool TStatusPrivate::operator==(const TStatusPrivate& s) const {
  return (_capital == s._capital &&
          _roman == s._roman &&
          _isHyphen == s._isHyphen &&
          _isPossessive == s._isPossessive &&
          m_isConcatAbbrev == s.m_isConcatAbbrev &&
          m_isTwitter == s.m_isTwitter &&
          _numeric == s._numeric &&
          _status == s._status &&
          _defaultKey == s._defaultKey);
}

// Comparison operator
bool TStatusPrivate::operator<(const TStatusPrivate& s) const {
  // only need an order:

//   LOGINIT("LP::Automaton");
//   std::ostringstream oss1,oss2;
//   TStatus(*this).outputXML(oss1);
//   TStatus(s).outputXML(oss2);
//   LDEBUG << "comparing TStatus "
//          << oss1.str()
//          << " with "
//          << oss2.str();

  if (!_defaultKey.empty() && !_defaultKey.back().isEmpty() &&
      !s._defaultKey.empty() && !s._defaultKey.back().isEmpty()) {
    return (_defaultKey.back() < s._defaultKey.back());
  }
  else {
    if (_status < s._status) {
      //LDEBUG << "return true on status";
      return true;
    }
    if (_status == s._status) {
      switch(_status) {
      case T_ALPHA:
        if (_capital < s._capital) {
          //LDEBUG << "return true on capital";
          return true;
        }
        if (_capital == s._capital) {
          if (_roman < s._roman) {
            //LDEBUG << "return true on roman";
            return true;
          }
        }
        //LDEBUG << "return false on other alpha";
        return false;
      case T_NUMERIC:
        if (_numeric < s._numeric) {
          //LDEBUG << "return true on numeric";
          return true;
        }
        //LDEBUG << "return false on other numeric";
        return false;
      default:
        //LDEBUG << "return false on other status";
        return false;
      }
    }
  }
  //LDEBUG << "return false on other";
  return false;
}

// ===================================================

TStatus::TStatus() : m_d(new TStatusPrivate())
{
}

TStatus::TStatus(const TStatus& tStatus) : m_d(new TStatusPrivate(*tStatus.m_d))
{
}

TStatus::TStatus(AlphaCapitalType capital,
                 AlphaRomanType roman,
                 bool isHyphen,
                 bool isPossessive,
                 bool isConcatAbbrev,
                 bool isTwitter,
                 NumericType numeric,
                 StatusType status) :
                 m_d(new TStatusPrivate(capital, roman, isHyphen, isPossessive, isConcatAbbrev, isTwitter, numeric, status))
{
}

TStatus::~TStatus()
{
  delete m_d;
}

// Assignment operator. Copies the specified TStatus
// into self. Returns a reference to self.
TStatus& TStatus::operator=(const TStatus& tStatus) {
    if (this != &tStatus) {
      *m_d = *tStatus.m_d;
    }
    return *this;
}

bool TStatus::operator==(const TStatus& s) const {
  return (*m_d == *s.m_d);
}

// Comparison operator
bool TStatus::operator<(const TStatus& s) const {
  // only need an order:
  return *this->m_d < *s.m_d;
}

// reset Status values
void TStatus::reset() {
    m_d->_capital = T_NULL_CAPITAL;
    m_d->_roman = T_NULL_ROMAN;
    m_d->_isHyphen = false;
    m_d->_isPossessive = false;
    m_d->m_isConcatAbbrev = false;
    m_d->m_isTwitter = false;
    m_d->_numeric = T_NULL_NUM;
    m_d->_status = T_NULL_STATUS;
    m_d->_defaultKey.clear();
    m_d->_defaultKey.push_back(Common::Misc::utf8stdstring2limastring("t_fallback"));
}

void TStatus::setAlphaCapital(const AlphaCapitalType alphaCapital){
    m_d->_capital = alphaCapital;
}

void TStatus::setAlphaRoman(const AlphaRomanType alphaRoman) {
    m_d->_roman = alphaRoman;
}

void TStatus::setAlphaHyphen(bool isAlphaHyphen) {
    m_d->_isHyphen = isAlphaHyphen;
}

void TStatus::setAlphaPossessive(bool isAlphaPossessive) {
  m_d->_isPossessive = isAlphaPossessive;
}

void TStatus::setTwitter(bool isTwitter) {
  m_d->m_isTwitter = isTwitter;
}

void TStatus::setNumeric(const NumericType numeric) {
    m_d->_numeric = numeric;
}

void TStatus::setStatus(const StatusType status) {
    m_d->_status = status;
}

AlphaCapitalType TStatus::getAlphaCapital() const {
    return m_d->_capital;
}

AlphaRomanType TStatus::getAlphaRoman() const {
    return m_d->_roman;
}

bool TStatus::isAlphaHyphen() const {
    return m_d->_isHyphen;
}

bool TStatus::isAlphaPossessive() const {
    return m_d->_isPossessive;
}

bool TStatus::isAlphaConcatAbbrev() const {
  return m_d->m_isConcatAbbrev;
}

bool TStatus::isTwitter() const {
  return m_d->m_isTwitter;
}

void TStatus::setAlphaConcatAbbrev(bool isConcatAbbrev) {
    m_d->m_isConcatAbbrev = isConcatAbbrev;
}

NumericType TStatus::getNumeric() const {
    return m_d->_numeric;
}

StatusType TStatus::getStatus() const {
    return m_d->_status;
}

const Lima::LimaString& TStatus::defaultKey() const {
//     return m_d->_defaultKey.empty() ? LimaString() : m_d->_defaultKey.back();
  if (m_d->_defaultKey.empty())
    throw std::runtime_error("default key vector cannot be empty");
  return m_d->_defaultKey.back();
}

void TStatus::setDefaultKey(const Lima::LimaString& defaultKey) {
#ifdef DEBUG_LP
  TOKENIZERLOGINIT;
  LTRACE << "TStatus::setDefaultKey to" << Common::Misc::limastring2utf8stdstring(defaultKey);
  LTRACE << "TStatus::setDefaultKey previous:" << ((m_d->_defaultKey.empty())?LimaString("EMPTY"):m_d->_defaultKey.back());
  if (!m_d->_defaultKey.empty())
  {
    LTRACE << "TStatus::setDefaultKey previous:" << Common::Misc::limastring2utf8stdstring(m_d->_defaultKey.back());
  }
#endif
  // do not push several times the same value
  if (!m_d->_defaultKey.empty() && m_d->_defaultKey.back() == defaultKey)
  {
    return;
  }
  // remove roman previous keys if setting to not roman
  else if (defaultKey == Common::Misc::utf8stdstring2limastring("t_not_roman"))
  {
#ifdef DEBUG_LP
    LTRACE << "TStatus::setDefaultKey remove roman from all previous";
#endif
    std::vector <Lima::LimaString > newDefaultKey;
    for (std::vector <Lima::LimaString >::const_iterator it = m_d->_defaultKey.begin(); it != m_d->_defaultKey.end(); it++)
    {
      if ( *it != Common::Misc::utf8stdstring2limastring("t_cardinal_roman")
        && (*it != Common::Misc::utf8stdstring2limastring("t_ordinal_roman")) )
      {
        newDefaultKey.push_back(*it);
      }
    }
    m_d->_defaultKey = newDefaultKey;
  }
  // push the new default
  else
  {
    // remove the fallback default key if present
    if (m_d->_defaultKey.size()==1 && Common::Misc::limastring2utf8stdstring(m_d->_defaultKey.back()) == "t_fallback")
      m_d->_defaultKey.pop_back();
    m_d->_defaultKey.push_back(defaultKey);
  }
}


std::string TStatus::toString() const
{
  std::string result;
  if (m_d->_status == T_ALPHA)
  {
    result += "t_alpha ";
    result += "t_alpha_capital: ";
    switch (m_d->_capital)
    {
      case T_CAPITAL : result += "t_capital; "; break;
      case T_SMALL : result += "t_small; "; break;
      case T_CAPITAL_1ST : result += "t_capital_1st; "; break;
      case T_ACRONYM : result += "t_acronym; "; break;
      case T_CAPITAL_SMALL : result += "t_capital_small; "; break;
      default : ;
    }
    if (m_d->_roman != T_NULL_ROMAN)
    {
      result += "t_alpha_roman: ";
      switch (m_d->_roman)
      {
        case T_CARDINAL_ROMAN : result += "t_cardinal_roman; "; break;
        case T_ORDINAL_ROMAN : result += "t_ordinal_roman; "; break;
        case T_NOT_ROMAN : result += "t_not_roman; "; break;
        default : ;
      }
    }
    if (m_d->_isHyphen)
      result += "t_alpha_hyphen; ";
    if (m_d->_isPossessive)
      result += "t_alpha_possessive; ";
    if (m_d->m_isConcatAbbrev)
      result += "t_alpha_concat_abbrev; ";
    if (m_d->m_isTwitter)
      result += "t_twitter; ";
  }
  else if (m_d->_status == T_NUMERIC)
  {
    result += "t_numeric ";
    switch (m_d->_numeric)
    {
      case T_INTEGER : result += "t_integer; "; break;
      case T_COMMA_NUMBER : result += "t_comma_number; "; break;
      case T_DOT_NUMBER : result += "t_dot_number; "; break;
      case T_FRACTION : result += "t_fraction; "; break;
      case T_ORDINAL_INTEGER : result += "t_ordinal_integer; "; break;
      default : result += "unknown; "; break;
    }
  }
  else
  {
    result += "t_other ";
    switch (m_d->_status)
    {
      case T_ALPHANUMERIC : result += "t_alphanumeric; "; break;
      case T_PATTERN : result += "t_pattern; "; break;
      case T_WORD_BRK : result += "t_word_brk; "; break;
      case T_SENTENCE_BRK : result += "t_sentence_brk; "; break;
      case T_PARAGRAPH_BRK : result += "t_paragraph_brk; "; break;
      default : result += "unknown; "; break;
    }
  }
  result += "t_default: " + Common::Misc::limastring2utf8stdstring(m_d->_defaultKey.empty()?LimaString():m_d->_defaultKey.back());
  return result;
}

void TStatus::outputXML(std::ostream& xmlStream) const
{
    if (m_d->_status == T_ALPHA)
    {
        xmlStream << "        <t_alpha>" << std::endl;
        xmlStream << "          <t_alpha_capital>";
        switch (m_d->_capital)
        {
          case T_CAPITAL : xmlStream << "t_capital"; break;
          case T_SMALL : xmlStream << "t_small"; break;
          case T_CAPITAL_1ST : xmlStream << "t_capital_1st"; break;
          case T_ACRONYM : xmlStream << "t_acronym"; break;
          case T_CAPITAL_SMALL : xmlStream << "t_capital_small"; break;
          case T_ABBREV : xmlStream << "t_abbrev"; break;
          default : ;//xmlStream << _capital;
        }
        xmlStream << "</t_alpha_capital>" << std::endl;
        if (m_d->_roman != T_NULL_ROMAN)
        {
          xmlStream << "          <t_alpha_roman>";
          switch (m_d->_roman)
          {
            case T_CARDINAL_ROMAN : xmlStream << "t_cardinal_roman"; break;
            case T_ORDINAL_ROMAN : xmlStream << "t_ordinal_roman"; break;
            case T_NOT_ROMAN : xmlStream << "t_not_roman"; break;
            default : ;//xmlStream << _roman;
          }
          xmlStream << "</t_alpha_roman>" << std::endl;
        }
        if (m_d->_isHyphen)
          xmlStream << "          <t_alpha_hyphen/>" << std::endl;
        if (m_d->_isPossessive)
          xmlStream << "          <t_alpha_possessive/>" << std::endl;
        if (m_d->m_isConcatAbbrev)
          xmlStream << "          <t_alpha_concat_abbrev/>" << std::endl;
        if (m_d->m_isTwitter)
          xmlStream << "          <t_twitter/>" << std::endl;
        xmlStream << "        </t_alpha>" << std::endl;
    }
    else if (m_d->_status == T_NUMERIC)
    {
        xmlStream << "        <t_numeric>";
        switch (m_d->_numeric)
        {
          case T_INTEGER : xmlStream << "t_integer"; break;
          case T_COMMA_NUMBER : xmlStream << "t_comma_number"; break;
          case T_DOT_NUMBER : xmlStream << "t_dot_number"; break;
          case T_FRACTION : xmlStream << "t_fraction"; break;
          case T_ORDINAL_INTEGER : xmlStream << "t_ordinal_integer"; break;
          default : xmlStream << "unknown"; break;
        }
        xmlStream << "</t_numeric>" << std::endl;
    }
    else
    {
        xmlStream << "        <t_other>";
        switch (m_d->_status)
        {
          case T_ALPHANUMERIC : xmlStream << "t_alphanumeric"; break;
          case T_PATTERN : xmlStream << "t_pattern"; break;
          case T_WORD_BRK : xmlStream << "t_word_brk"; break;
          case T_SENTENCE_BRK : xmlStream  << "t_sentence_brk"; break;
          case T_PARAGRAPH_BRK : xmlStream  << "t_paragraph_brk"; break;
          default : xmlStream << "unknown"; break;
        }
        xmlStream << "</t_other>" << std::endl;
    }
    xmlStream << "        <t_default>";
    xmlStream << Lima::Common::Misc::limastring2utf8stdstring(m_d->_defaultKey.empty()?LimaString():m_d->_defaultKey.back());
    xmlStream << "</t_default>" << std::endl;
}

} // closing namespace Data
} // closing namespace LinguisticProcessing
} // closing namespace Lima
