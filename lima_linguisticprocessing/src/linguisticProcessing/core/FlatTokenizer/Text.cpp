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

// NAUTITIA
//
// jys 24-JUL-2002
//
// Text is the class which reads original text and does its
// 1st transformation into characters classes string.

#include "Text.h"

#include "CharChart.h"

#include "common/Data/strwstrtools.h"
#include "linguisticProcessing/client/LinguisticProcessingException.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/Token.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/TStatus.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticData.h"

using namespace Lima;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::Common::Misc;

namespace Lima
{
namespace LinguisticProcessing
{
namespace FlatTokenizer
{


Text::Text(MediaId lang, const CharChart* charChart) :
    m_text(),
    _curPtr(0),
    _debPtr(0),
    _curSettings(),
    _tTokenGraph(0),
    _currentVx(0),
    _lastVx(0),
    _isDone(false),
    _thereIsUnknown(false),
    _stringsPool(0),
    m_charChart(charChart)
{
  _stringsPool=&Common::MediaticData::MediaticData::changeable().stringsPool(lang);
}

Text::~Text()
{
  //    cout << "Text::~Text" << endl;
}

void Text::setText(const Lima::LimaString& text)
{
  m_text = text;
  _curPtr = 0;
  _debPtr = 0;
  _tTokenGraph = 0;
  _currentVx = 0;
  _lastVx = 0;
  _isDone = false;
  _thereIsUnknown = false;
}


// Clear the entirely class and structure to accept new text
void Text::clear()
{
  //    _tTokenList = 0; Not destroyed here
  _thereIsUnknown = false;
}

void Text::setGraph(LinguisticGraphVertex position,LinguisticGraph* graph)
{
  _currentVx=position;
  _tTokenGraph=graph;
  // go one step forward on the new path
  LinguisticGraphAdjacencyIt adjItr,adjItrEnd;
  boost::tie (adjItr,adjItrEnd) = adjacent_vertices(_currentVx,*_tTokenGraph);
  if (adjItr==adjItrEnd)
  {
    TOKENIZERLOGINIT;
    LERROR << "Tokenizer Text : no token forward !";
    throw LinguisticProcessingException();
  }
  _lastVx=*adjItr;
  if (++adjItr!=adjItrEnd) {
    TOKENIZERLOGINIT;
    LERROR << "Tokenizer Text : more than one next token !";
    throw LinguisticProcessingException();
  }
  //remove_edge(_currentVx,_lastVx,*_tTokenGraph);
}

void Text::finalizeAndUnsetGraph()
{
  add_edge(_currentVx,_lastVx,*_tTokenGraph);
  _tTokenGraph=0;
}

// increments text pointer
Lima::LimaChar Text::advance()
{
#ifdef DEBUG_LP
  TOKENIZERLOGINIT;
#endif
  if (_curPtr+1 >= m_text.size())
  {
#ifdef DEBUG_LP
    LDEBUG << "Trying to move after text end.";
#endif
    _curPtr++;
    return 0;
  }
  if (m_text[_curPtr] >= 0xD800)
  {
    _curPtr++;
  }
  _curPtr++;
#ifdef DEBUG_LP
  LDEBUG << "Text::advance : new current=" << _curPtr << " from='" << Common::Misc::limastring2utf8stdstring(LimaString()+m_text[_curPtr-1]) << "' to='" << Common::Misc::limastring2utf8stdstring(LimaString()+m_text[_curPtr]) << "'";
#endif
  return m_text[_curPtr];
}

const CharClass* Text::currentClass() const
{
#ifdef DEBUG_LP
  TOKENIZERLOGINIT;
    if (_curPtr+1 >= m_text.size())
    {
      LDEBUG << "currentClass() at " << _curPtr << ". No char after text end";
    }
    else
    {
      LDEBUG << "currentClass() at " << _curPtr << ", for " << m_text[_curPtr];
    }
#endif
  if (_curPtr >= m_text.size())
  {
    return m_charChart->charClass(0);
  }
  if (m_text[_curPtr] >= 0xD800)
  {
    if (_curPtr+1 >= m_text.size())
    {
      return m_charChart->charClass(0);
    }
    return m_charChart->charClass( m_text[_curPtr], m_text[_curPtr+1] );
  }
  else
  {
    return m_charChart->charClass( m_text[_curPtr] );
  }

}


// flushes current token
void Text::flush()
{
  _debPtr = _curPtr;
}

// takes a token
LimaString Text::token()
{
#ifdef DEBUG_LP
  TOKENIZERLOGINIT;
#endif
  // Creates a new token
  uint64_t delta = _curPtr;
  if (m_text[_curPtr] >= 0xD800 || _curPtr == _debPtr)
  {
    delta++;
  }
  if (_debPtr >= m_text.size())
  {
    TOKENIZERLOGINIT;
    LERROR << "Empty token !";
    _debPtr = delta;
    _curSettings.reset();
    return utf8stdstring2limastring("");
  }
  LimaString str=m_text.mid( _debPtr, (delta-_debPtr));
#ifdef DEBUG_LP
  LDEBUG << "      Adding token '" << str << "'";
#endif
  StringsPoolIndex form=(*_stringsPool)[str];
  Token *tToken = new Token(form,str,_debPtr+1,(delta-_debPtr));
  if (tToken == 0) throw MemoryErrorException();
  // @todo: set default status here, according to structured status (alpha,numeric etc...)
  // instead of setting it at each change of status (setAlphaCapital, setNumeric etc...)
  tToken->setStatus(_curSettings);
//   LDEBUG << "      _curSettings is " << _curSettings.toString();
#ifdef DEBUG_LP
  LDEBUG << "      status is " << tToken->status().toString();
#endif
  // Adds on the path
  LinguisticGraphVertex newVx=add_vertex(*_tTokenGraph);
  put(vertex_token,*_tTokenGraph,newVx,tToken);
  put(vertex_data,*_tTokenGraph,newVx,new MorphoSyntacticData());
  add_edge(_currentVx,newVx,*_tTokenGraph);
  _currentVx=newVx;
  _debPtr = delta;
  _curSettings.reset();
  return str;
}

// performs a trace
void Text::trace()
{}

LimaChar Text::operator[] (int i) const {
  if (((static_cast<int>(_curPtr)+i) < 0) || (static_cast<int>(_curPtr)+i >= m_text.size()))
    throw BoundsErrorException();
  return m_text[_curPtr+i];
}

void Text::setAlphaCapital(const LinguisticAnalysisStructure::AlphaCapitalType alphaCapital) 
{
#ifdef DEBUG_LP
  TOKENIZERLOGINIT;
  LDEBUG << "Text::setAlphaCapital " << alphaCapital;
#endif
  _curSettings.setAlphaCapital(alphaCapital);
  switch (alphaCapital)  
  {
    case T_CAPITAL:
      _curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_capital"));
      break;
    case T_SMALL:
      _curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_small"));
      break;
    case T_CAPITAL_1ST:
      _curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_capital_1st"));
      break;
    case T_ACRONYM:
      _curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_acronym"));
      break;
    case T_CAPITAL_SMALL:
      _curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_capital_small"));
      break;
    case T_ABBREV:
      _curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_abbrev"));
      break;
    default:
    _curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_small"));
  }
}

void Text::setAlphaRoman(const LinguisticAnalysisStructure::AlphaRomanType alphaRoman)
{
#ifdef DEBUG_LP
  TOKENIZERLOGINIT;
  LDEBUG << "Text::setAlphaRoman " << alphaRoman;
#endif
  _curSettings.setAlphaRoman(alphaRoman);
  switch (alphaRoman)
  {
    case T_CARDINAL_ROMAN:
      _curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_cardinal_roman"));
      break;
    case T_ORDINAL_ROMAN:
      _curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_ordinal_roman"));
      break;
    case T_NOT_ROMAN:
      _curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_not_roman"));
      break;
    default:;
  }
}

void Text::setAlphaHyphen(const unsigned char isAlphaHyphen)
{
#ifdef DEBUG_LP
  TOKENIZERLOGINIT;
  LDEBUG << "Text::setAlphaHyphen " << isAlphaHyphen;
#endif
  _curSettings.setAlphaHyphen(isAlphaHyphen);
}

void Text::setAlphaPossessive(const unsigned char isAlphaPossessive)
{
#ifdef DEBUG_LP
  TOKENIZERLOGINIT;
  LDEBUG << "Text::setAlphaPossessive " << isAlphaPossessive;
#endif
  _curSettings.setAlphaPossessive(isAlphaPossessive);
/*  if (isAlphaPossessive > 0)
    _curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_alpha_possessive"));*/
}

void Text::setAlphaConcatAbbrev(const unsigned char isConcatAbbreviation)
{
#ifdef DEBUG_LP
  TOKENIZERLOGINIT;
  LDEBUG << "Text::setAlphaConcatAbbrev " << isConcatAbbreviation;
#endif
  _curSettings.setAlphaConcatAbbrev(isConcatAbbreviation);
  if (isConcatAbbreviation> 0)
    _curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_alpha_concat_abbrev"));
}

void Text::setTwitter(const unsigned char isTwitter)
{
#ifdef DEBUG_LP
  TOKENIZERLOGINIT;
  LDEBUG << "Text::setTwitter " << isTwitter;
#endif
  _curSettings.setTwitter(isTwitter);
  if (isTwitter> 0)
    _curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_twitter"));
}

void Text::setNumeric(const LinguisticAnalysisStructure::NumericType numeric)
{
#ifdef DEBUG_LP
  TOKENIZERLOGINIT;
  LDEBUG << "Text::setNumeric " << numeric;
#endif
  _curSettings.setNumeric(numeric);
  switch (numeric)
  {
    case T_INTEGER:
      _curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_integer"));
      break;
    case T_COMMA_NUMBER:
      _curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_comma_number"));
      break;
    case T_DOT_NUMBER:
      _curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_dot_number"));
      break;
    case T_FRACTION:
      _curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_fraction"));
      break;
    case T_ORDINAL_INTEGER:
      _curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_ordinal_integer"));
      break;
    default:;
  }
}

void Text::setStatus(const LinguisticAnalysisStructure::StatusType status)
{
#ifdef DEBUG_LP
  TOKENIZERLOGINIT;
  LDEBUG << "Text::setStatus " << status;
#endif
  LinguisticAnalysisStructure::StatusType previousStatus = _curSettings.getStatus();
  _curSettings.setStatus(status);
  switch (status)
  {
    case T_ALPHA:
      if (previousStatus != T_ALPHA)
        _curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_alphanumeric"));
      break;
    case T_NUMERIC:
      _curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_alphanumeric"));
      break;
    case T_ALPHANUMERIC:
      _curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_alphanumeric"));
      break;
    case T_PATTERN:
      _curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_pattern"));
      break;
    case T_WORD_BRK:
      _curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_word_brk"));
      break;
    case T_SENTENCE_BRK:
      _curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_sentence_brk"));
      break;
    case T_PARAGRAPH_BRK:
      _curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_paragraph_brk"));
      break;
    default:;
  }

}

void Text::setDefaultKey(const Lima::LimaString& defaultKey) 
{
#ifdef DEBUG_LP
  TOKENIZERLOGINIT;
  LDEBUG << "Text::setDefaultKey " << Common::Misc::limastring2utf8stdstring(defaultKey);
#endif
  _curSettings.setDefaultKey(defaultKey);
}

// set default key in status according to other elements in status
void Text::computeDefaultStatus()
{
  std::string defaultKey;
  switch (_curSettings.getStatus()) {
    case T_ALPHA : {
      switch (_curSettings.getAlphaCapital()) {
        case T_CAPITAL       : defaultKey = "t_capital"      ; break;
        case T_SMALL         : defaultKey = "t_small"        ; break;
        case T_CAPITAL_1ST   : defaultKey = "t_capital_1st"  ; break;
        case T_ACRONYM       : defaultKey = "t_acronym"      ; break;
        case T_CAPITAL_SMALL : defaultKey = "t_capital_small"; break;
        case T_ABBREV       : defaultKey = "t_abbrev"      ; break;
        default : break;
      }
      switch (_curSettings.getAlphaRoman()) { // Roman supersedes Cardinal
        case T_CARDINAL_ROMAN : defaultKey = "t_cardinal_roman"; break;
        case T_ORDINAL_ROMAN  : defaultKey = "t_ordinal_roman" ; break;
        case T_NOT_ROMAN      : defaultKey = "t_not_roman"     ; break;
        default : break;
      }
      if (_curSettings.isAlphaHyphen()) {
        //no change
        //defaultKey = "t_alpha_hyphen";
      }
      if (_curSettings.isAlphaPossessive()) {
        defaultKey = "t_alpha_possessive";
      }
      break;
    } // end T_ALPHA
    case T_NUMERIC : {
      switch (_curSettings.getNumeric()) {
        case T_INTEGER         : defaultKey = "t_integer"       ; break;
        case T_COMMA_NUMBER    : defaultKey = "t_comma_number"  ; break;
        case T_DOT_NUMBER      : defaultKey = "t_dot_number"    ; break;
        case T_FRACTION        : defaultKey = "t_fraction"      ; break;
        case T_ORDINAL_INTEGER : defaultKey = "t_ordinal_integer"; break;
        default: break;
      }
      break;
    }
    case T_ALPHANUMERIC : defaultKey = "t_alphanumeric" ; break;
    case T_PATTERN      : defaultKey = "t_pattern"      ; break;
    case T_WORD_BRK     : defaultKey = "t_word_brk"     ; break;
    case T_SENTENCE_BRK : defaultKey = "t_sentence_brk" ; break;
    case T_PARAGRAPH_BRK: defaultKey = "t_paragraph_brk" ; break;
    default: defaultKey = "t_fallback";
  }
#ifdef DEBUG_LP
  TOKENIZERLOGINIT;
  LDEBUG << "Text::computeDefaultKey " << defaultKey;
#endif
  _curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring(defaultKey));
}


} //namespace FlatTokenizer
} // namespace LinguisticProcessing
} // namespace Lima
