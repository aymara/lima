// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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

class TextPrivate
{
friend class Text;
public:
  TextPrivate(MediaId lang, std::shared_ptr<CharChart> charChart);
  ~TextPrivate();
  TextPrivate(const Text&) = delete;
  TextPrivate& operator=(const Text&) = delete;

  LimaString m_text;
  int m_curPtr;
  int m_debPtr;
  LinguisticAnalysisStructure::TStatus m_curSettings;
  LinguisticGraph* m_tTokenGraph;
  LinguisticGraphVertex m_currentVx;
  LinguisticGraphVertex m_lastVx;

  FsaStringsPool* m_stringsPool;
  std::shared_ptr<CharChart> m_charChart;
};


TextPrivate::TextPrivate(MediaId lang, std::shared_ptr<CharChart> charChart) :
    m_text(),
    m_curPtr(0),
    m_debPtr(0),
    m_curSettings(),
    m_tTokenGraph(0),
    m_currentVx(0),
    m_lastVx(0),
    m_stringsPool(&Common::MediaticData::MediaticData::changeable().stringsPool(lang)),
    m_charChart(charChart)
{
}

TextPrivate::~TextPrivate()
{
}

Text::Text(MediaId lang, std::shared_ptr<CharChart> charChart) :
    m_d(new TextPrivate(lang, charChart))
{
}

Text::~Text()
{
  delete m_d;
}

LimaChar Text::currentChar() const
{
  if (m_d->m_curPtr >= m_d->m_text.size())
  {
    return QChar();
  }
  return m_d->m_text[m_d->m_curPtr];
}

void Text::setText(const Lima::LimaString& text)
{
  m_d->m_text = text;
  m_d->m_curPtr = 0;
  m_d->m_debPtr = 0;
  m_d->m_tTokenGraph = 0;
  m_d->m_currentVx = 0;
  m_d->m_lastVx = 0;
}

int Text::position() const
{
  return m_d->m_curPtr;
}

int Text::size() const
{
  return m_d->m_text.size();
}

void Text::setStatus(const LinguisticAnalysisStructure::TStatus& status)
{
  m_d->m_curSettings = status;
}


// Clear the entirely class and structure to accept new text
void Text::clear()
{
  //    _tTokenList = 0; Not destroyed here
}

void Text::setGraph(LinguisticGraphVertex position, LinguisticGraph* graph)
{
  m_d->m_currentVx=position;
  m_d->m_tTokenGraph = graph;
  // go one step forward on the new path
  LinguisticGraphAdjacencyIt adjItr,adjItrEnd;
  boost::tie (adjItr,adjItrEnd) = adjacent_vertices(m_d->m_currentVx, *m_d->m_tTokenGraph);
  if (adjItr==adjItrEnd)
  {
    TOKENIZERLOGINIT;
    LIMA_LP_EXCEPTION("Tokenizer Text : no token forward !");
  }
  m_d->m_lastVx=*adjItr;
  if (++adjItr!=adjItrEnd) {
    TOKENIZERLOGINIT;
    LIMA_LP_EXCEPTION("Tokenizer Text : more than one next token !");
  }
  //remove_edge(m_d->m_currentVx,m_d->m_lastVx,*m_d->m_tTokenGraph);
}

void Text::finalizeAndUnsetGraph()
{
  add_edge(m_d->m_currentVx,m_d->m_lastVx,*m_d->m_tTokenGraph);
  m_d->m_tTokenGraph=0;
}

// increments text pointer
Lima::LimaChar Text::advance()
{
#ifdef DEBUG_LP
  TOKENIZERLOGINIT;
#endif
  if (m_d->m_curPtr+1 >= m_d->m_text.size())
  {
#ifdef DEBUG_LP
    LDEBUG << "Trying to move after text end.";
#endif
    m_d->m_curPtr++;
    return QChar();
  }
  if (m_d->m_text.at(m_d->m_curPtr).isHighSurrogate())
  {
    m_d->m_curPtr++;
  }
  m_d->m_curPtr++;
#ifdef DEBUG_LP
  LDEBUG << "Text::advance : new current=" << m_d->m_curPtr << " from='"
          << m_d->m_text[m_d->m_curPtr-1] << "' to='" << m_d->m_text[m_d->m_curPtr] << "'";
#endif
  return m_d->m_text[m_d->m_curPtr];
}

const CharClass* Text::currentClass() const
{
#ifdef DEBUG_LP
  TOKENIZERLOGINIT;
    if (m_d->m_curPtr+1 >= m_d->m_text.size())
    {
      LDEBUG << "currentClass() at " << m_d->m_curPtr << ". No char after text end";
    }
    else
    {
      LDEBUG << "currentClass() at " << m_d->m_curPtr << ", for " << m_d->m_text[m_d->m_curPtr];
    }
#endif
  if (m_d->m_curPtr >= m_d->m_text.size())
  {
    return m_d->m_charChart->charClass(QChar());
  }
  QChar c = m_d->m_text[m_d->m_curPtr];
  if (c.isHighSurrogate())
  {
    if (m_d->m_curPtr+1 >= m_d->m_text.size())
    {
      return m_d->m_charChart->charClass(QChar());
    }
    return m_d->m_charChart->charClass( m_d->m_text[m_d->m_curPtr], m_d->m_text[m_d->m_curPtr+1] );
  }
  else
  {
    return m_d->m_charChart->charClass( m_d->m_text[m_d->m_curPtr] );
  }

}


// flushes current token
void Text::flush()
{
  m_d->m_debPtr = m_d->m_curPtr;
}

// takes a token
LimaString Text::token()
{
#ifdef DEBUG_LP
  TOKENIZERLOGINIT;
#endif
  // Creates a new token
  uint64_t delta = m_d->m_curPtr;
  if (m_d->m_curPtr < m_d->m_text.size()
    && (m_d->m_text.at(m_d->m_curPtr).isHighSurrogate() || m_d->m_curPtr == m_d->m_debPtr))
  {
    delta++;
  }
  if (m_d->m_debPtr >= m_d->m_text.size())
  {
    TOKENIZERLOGINIT;
    LERROR << "Empty token !";
    m_d->m_debPtr = delta;
    m_d->m_curSettings.reset();
    return utf8stdstring2limastring("");
  }
  LimaString str=m_d->m_text.mid( m_d->m_debPtr, (delta-m_d->m_debPtr));
#ifdef DEBUG_LP
  LDEBUG << "      Adding token '" << str << "'";
#endif
  StringsPoolIndex form=(*m_d->m_stringsPool)[str];
  Token *tToken = new Token(form,str,m_d->m_debPtr+1,(delta-m_d->m_debPtr));
  if (tToken == 0) throw MemoryErrorException();
  // @todo: set default status here, according to structured status (alpha,numeric etc...)
  // instead of setting it at each change of status (setAlphaCapital, setNumeric etc...)
  tToken->setStatus(m_d->m_curSettings);
//   LDEBUG << "      m_d->m_curSettings is " << m_d->m_curSettings.toString();
#ifdef DEBUG_LP
  LDEBUG << "      status is " << tToken->status().toString();
#endif
  // Adds on the path
  LinguisticGraphVertex newVx = add_vertex(*m_d->m_tTokenGraph);
  put(vertex_token, *m_d->m_tTokenGraph, newVx, tToken);
  put(vertex_data, *m_d->m_tTokenGraph, newVx, new MorphoSyntacticData());
  add_edge(m_d->m_currentVx, newVx, *m_d->m_tTokenGraph);
  m_d->m_currentVx=newVx;
  m_d->m_debPtr = delta;
  m_d->m_curSettings.reset();
  return str;
}

// performs a trace
void Text::trace()
{}

LimaChar Text::operator[] (int i) const {
  if (((static_cast<int>(m_d->m_curPtr)+i) < 0) || (static_cast<int>(m_d->m_curPtr)+i >= m_d->m_text.size()))
    throw BoundsErrorException();
  return m_d->m_text[m_d->m_curPtr+i];
}

void Text::setAlphaCapital(const LinguisticAnalysisStructure::AlphaCapitalType alphaCapital)
{
#ifdef DEBUG_LP
  TOKENIZERLOGINIT;
  LDEBUG << "Text::setAlphaCapital " << alphaCapital;
#endif
  m_d->m_curSettings.setAlphaCapital(alphaCapital);
  switch (alphaCapital)
  {
    case T_CAPITAL:
      m_d->m_curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_capital"));
      break;
    case T_SMALL:
      m_d->m_curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_small"));
      break;
    case T_CAPITAL_1ST:
      m_d->m_curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_capital_1st"));
      break;
    case T_ACRONYM:
      m_d->m_curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_acronym"));
      break;
    case T_CAPITAL_SMALL:
      m_d->m_curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_capital_small"));
      break;
    case T_ABBREV:
      m_d->m_curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_abbrev"));
      break;
    default:
    m_d->m_curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_small"));
  }
}

void Text::setAlphaRoman(const LinguisticAnalysisStructure::AlphaRomanType alphaRoman)
{
#ifdef DEBUG_LP
  TOKENIZERLOGINIT;
  LDEBUG << "Text::setAlphaRoman " << alphaRoman;
#endif
  m_d->m_curSettings.setAlphaRoman(alphaRoman);
  switch (alphaRoman)
  {
    case T_CARDINAL_ROMAN:
      m_d->m_curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_cardinal_roman"));
      break;
    case T_ORDINAL_ROMAN:
      m_d->m_curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_ordinal_roman"));
      break;
    case T_NOT_ROMAN:
      m_d->m_curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_not_roman"));
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
  m_d->m_curSettings.setAlphaHyphen(isAlphaHyphen);
}

void Text::setAlphaPossessive(const unsigned char isAlphaPossessive)
{
#ifdef DEBUG_LP
  TOKENIZERLOGINIT;
  LDEBUG << "Text::setAlphaPossessive " << isAlphaPossessive;
#endif
  m_d->m_curSettings.setAlphaPossessive(isAlphaPossessive);
/*  if (isAlphaPossessive > 0)
    m_d->m_curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_alpha_possessive"));*/
}

void Text::setAlphaConcatAbbrev(const unsigned char isConcatAbbreviation)
{
#ifdef DEBUG_LP
  TOKENIZERLOGINIT;
  LDEBUG << "Text::setAlphaConcatAbbrev " << isConcatAbbreviation;
#endif
  m_d->m_curSettings.setAlphaConcatAbbrev(isConcatAbbreviation);
  if (isConcatAbbreviation> 0)
    m_d->m_curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_alpha_concat_abbrev"));
}

void Text::setTwitter(const unsigned char isTwitter)
{
#ifdef DEBUG_LP
  TOKENIZERLOGINIT;
  LDEBUG << "Text::setTwitter " << isTwitter;
#endif
  m_d->m_curSettings.setTwitter(isTwitter);
  if (isTwitter> 0)
    m_d->m_curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_twitter"));
}

void Text::setNumeric(const LinguisticAnalysisStructure::NumericType numeric)
{
#ifdef DEBUG_LP
  TOKENIZERLOGINIT;
  LDEBUG << "Text::setNumeric " << numeric;
#endif
  m_d->m_curSettings.setNumeric(numeric);
  switch (numeric)
  {
    case T_INTEGER:
      m_d->m_curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_integer"));
      break;
    case T_COMMA_NUMBER:
      m_d->m_curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_comma_number"));
      break;
    case T_DOT_NUMBER:
      m_d->m_curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_dot_number"));
      break;
    case T_FRACTION:
      m_d->m_curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_fraction"));
      break;
    case T_ORDINAL_INTEGER:
      m_d->m_curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_ordinal_integer"));
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
  LinguisticAnalysisStructure::StatusType previousStatus = m_d->m_curSettings.getStatus();
  m_d->m_curSettings.setStatus(status);
  switch (status)
  {
    case T_ALPHA:
      if (previousStatus != T_ALPHA)
        m_d->m_curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_alphanumeric"));
      break;
    case T_NUMERIC:
      m_d->m_curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_alphanumeric"));
      break;
    case T_ALPHANUMERIC:
      m_d->m_curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_alphanumeric"));
      break;
    case T_PATTERN:
      m_d->m_curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_pattern"));
      break;
    case T_WORD_BRK:
      m_d->m_curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_word_brk"));
      break;
    case T_SENTENCE_BRK:
      m_d->m_curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_sentence_brk"));
      break;
    case T_PARAGRAPH_BRK:
      m_d->m_curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_paragraph_brk"));
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
  m_d->m_curSettings.setDefaultKey(defaultKey);
}

// set default key in status according to other elements in status
void Text::computeDefaultStatus()
{
  std::string defaultKey;
  switch (m_d->m_curSettings.getStatus()) {
    case T_ALPHA : {
      switch (m_d->m_curSettings.getAlphaCapital()) {
        case T_CAPITAL       : defaultKey = "t_capital"      ; break;
        case T_SMALL         : defaultKey = "t_small"        ; break;
        case T_CAPITAL_1ST   : defaultKey = "t_capital_1st"  ; break;
        case T_ACRONYM       : defaultKey = "t_acronym"      ; break;
        case T_CAPITAL_SMALL : defaultKey = "t_capital_small"; break;
        case T_ABBREV       : defaultKey = "t_abbrev"      ; break;
        default : break;
      }
      switch (m_d->m_curSettings.getAlphaRoman()) { // Roman supersedes Cardinal
        case T_CARDINAL_ROMAN : defaultKey = "t_cardinal_roman"; break;
        case T_ORDINAL_ROMAN  : defaultKey = "t_ordinal_roman" ; break;
        case T_NOT_ROMAN      : defaultKey = "t_not_roman"     ; break;
        default : break;
      }
      if (m_d->m_curSettings.isAlphaHyphen()) {
        //no change
        //defaultKey = "t_alpha_hyphen";
      }
      if (m_d->m_curSettings.isAlphaPossessive()) {
        defaultKey = "t_alpha_possessive";
      }
      break;
    } // end T_ALPHA
    case T_NUMERIC : {
      switch (m_d->m_curSettings.getNumeric()) {
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
  m_d->m_curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring(defaultKey));
}


} //namespace FlatTokenizer
} // namespace LinguisticProcessing
} // namespace Lima
