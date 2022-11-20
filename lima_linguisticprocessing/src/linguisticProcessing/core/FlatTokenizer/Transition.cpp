// Copyright 2002-2019 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

// NAUTITIA
//
// jys 15-JUL-2002
//
// Transition is the main composant of automatons. Actions
// are performed only during transitions. Automatons evoluate
// only with transitions.
// Components of transitions are :
// o "check" components to determine if transition is "open"
//        o allowed events : to compare the current character
//            class with. (mandatory)
//        o static conditions : to check characters classes
//            before and after current character class and
//            inner automaton return status. (optionnal)
// o "action" components to determine what to do if transition
//    has been found open.
//        o next state : on the next character class, automaton
//            will be on that state (optionnal)
//        o action : move the pointer on character classes buffer,
//            take a token, flush the token memory, ... (optionnal)
//        o setting : set up flags into tokens data structure
//        o return_status if automaton is an inner one. (optionnal)

#include "Transition.h"

#include "Condition.h"
#include "State.h"
#include "common/misc/Exceptions.h"
#include "common/Data/LimaString.h"

using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::Common::Misc;

namespace Lima
{
namespace LinguisticProcessing
{
namespace FlatTokenizer
{

const char* Transition::SettingNames[] = {
    "SET_T_ALPHA",               // 0
    "SET_T_NUMERIC",             // 1
    "SET_T_PATTERN",             // 2
    "SET_T_WORD_BRK",            // 3
    "SET_T_SENTENCE_BRK",        // 4
    "SET_T_ALPHANUMERIC",        // 5
    "SET_T_ALPHA_HYPHEN",        // 6
    "SET_T_ALPHA_POSSESSIVE",    // 7
    "SET_T_CAPITAL",             // 8
    "SET_T_SMALL",               // 9
    "SET_T_CAPITAL_1ST",         // 10
    "SET_T_ACRONYM",             // 11
    "SET_T_CAPITAL_SMALL",       // 12
    "SET_T_CARDINAL_ROMAN",      // 13
    "SET_T_ORDINAL_ROMAN",       // 14
    "SET_T_NOT_ROMAN",           // 15
    "SET_T_INTEGER",             // 16
    "SET_T_COMMA_NUMBER",        // 17
    "SET_T_DOT_NUMBER",          // 18
    "SET_T_FRACTION",            // 19
    "SET_T_ORDINAL_INTEGER",     // 20
    "SET_T_ALPHA_CONCAT_ABBREV", // 21
    "SET_T_PARAGRAPH_BRK",       // 22
    "SET_T_ARABIC",              // 23
    "SET_T_LATIN_ARABIC",        // 24
    "SET_T_ART_DEF",             // 25
    "SET_T_ACRONYM_ARABIC",      // 26
    "SET_T_ACRONYM_LATIN_ARABIC",// 27
    "SET_T_TWITTER",             // 28
    "SET_T_ABBREV"               // 29
};

Transition::Transition(const State* state) :
  m_state(state),
  _toState(0),
  _events(state->automaton().charChart()),
  _condition(Condition(state->automaton().charChart())),
  m_tokenize(false),
  m_flush(false),
  m_defaultKey()
{
}

Transition::~Transition()
{
}

// for run-time use. Transition does its works
const State* Transition::run(Text& text) const
{
#ifdef DEBUG_LP
  TOKENIZERLOGINIT;
#endif
  const CharClass* currentClass = text.currentClass();
  if (currentClass == 0)
  {
    TOKENIZERLOGINIT;
    LERROR << "Transition::run Null Class for char '"<< limastring2utf8stdstring(LimaString()+text.currentChar()) << "'";
    return 0;
  }
  LimaChar chcl = text.currentChar();
#ifdef DEBUG_LP
  LDEBUG << "| | looking at transition "<<this<<" with char (" << chcl << " ; " << currentClass->id() << " ; " << currentClass->name() << ")";
#endif
  if (!_events.isRecognized(chcl))
  {
#ifdef DEBUG_LP
    LDEBUG << "| | event " << chcl << " not recognized.";
    LDEBUG << "| | transition failed";
#endif
    return 0;
  }
  else if (!_condition.isFulfilled(text))
  {
#ifdef DEBUG_LP
    LDEBUG << "| | event " << chcl << " recognized but conditions not fullfilled.";
    LDEBUG << "| | transition failed";
#endif
    return 0;
  }
#ifdef DEBUG_LP
  LDEBUG << "| | event " << chcl << " recognized: taking actions length="<<m_defaultKey.length()<<", tokenize: "<<m_tokenize<<", flush: "<<m_flush<<".";
#endif
  if (text.position() == 0)
  {
    applySettings(text);
  }
  // Transition is opened
//   LDEBUG << "Setting token StatusType to " << m_status.getStatus();
//   text.setStatus(m_status);

  if (m_defaultKey.length() != 0)
  {
#ifdef DEBUG_LP
    LDEBUG << "Setting token default key to " << limastring2utf8stdstring(m_defaultKey);
#endif
    text.setDefaultKey(m_defaultKey);
  }

  if (m_tokenize)
  {
#ifdef DEBUG_LP
    LDEBUG << "| | | adding token";
#endif
    text.token();
  }
  if (m_flush)
  {
#ifdef DEBUG_LP
    LDEBUG << "| | | flushing";
#endif
    text.flush();
  }

  if (text.position() != 0)
  {
    applySettings(text);
  }

#ifdef DEBUG_LP
  LDEBUG << "| |  "<<this<<" transition succeeded (next state is "
      << nextStateName() << ") on "
      << currentClass->name();
#endif
  text.advance();
  return _toState;
}

void Transition::applySettings(Text& text) const
{
#ifdef DEBUG_LP
  TOKENIZERLOGINIT;
#endif
  for (auto it = m_settings.cbegin(), it_end = m_settings.end();
       it != it_end; it++)
  {
#ifdef DEBUG_LP
    LDEBUG << "Putting status setting to text: " << Transition::SettingNames[*it];
#endif
    switch (*it)
    {
      case SET_T_ALPHA : text.setStatus(T_ALPHA); break;
      case SET_T_NUMERIC : text.setStatus(T_NUMERIC); break;
      case SET_T_PATTERN : text.setStatus(T_PATTERN); break;
      case SET_T_WORD_BRK : text.setStatus(T_WORD_BRK); break;
      case SET_T_SENTENCE_BRK : text.setStatus(T_SENTENCE_BRK); break;
      case SET_T_ALPHANUMERIC : text.setStatus(T_ALPHANUMERIC); break;
      case SET_T_ALPHA_HYPHEN : text.setAlphaHyphen(true); break;
      case SET_T_ALPHA_POSSESSIVE : text.setAlphaPossessive(true); break;
      case SET_T_CAPITAL : text.setAlphaCapital(T_CAPITAL); break;
      case SET_T_SMALL : text.setAlphaCapital(T_SMALL); break;
      case SET_T_CAPITAL_1ST : text.setAlphaCapital(T_CAPITAL_1ST); break;
      case SET_T_ACRONYM : text.setAlphaCapital(T_ACRONYM); break;
      case SET_T_CAPITAL_SMALL : text.setAlphaCapital(T_CAPITAL_SMALL); break;
      case SET_T_ABBREV : text.setAlphaCapital(T_ABBREV); break;
      case SET_T_CARDINAL_ROMAN : text.setAlphaRoman(T_CARDINAL_ROMAN); break;
      case SET_T_ORDINAL_ROMAN : text.setAlphaRoman(T_ORDINAL_ROMAN); break;
      case SET_T_NOT_ROMAN : text.setAlphaRoman(T_NOT_ROMAN); break;
      case SET_T_INTEGER : text.setNumeric(T_INTEGER); break;
      case SET_T_COMMA_NUMBER : text.setNumeric(T_COMMA_NUMBER); break;
      case SET_T_DOT_NUMBER : text.setNumeric(T_DOT_NUMBER); break;
      case SET_T_FRACTION : text.setNumeric(T_FRACTION); break;
      case SET_T_ORDINAL_INTEGER : text.setNumeric(T_ORDINAL_INTEGER); break;
      case SET_T_ALPHA_CONCAT_ABBREV : text.setAlphaConcatAbbrev(true); break;
      case SET_T_TWITTER : text.setTwitter(true); break;
      case SET_T_PARAGRAPH_BRK : text.setStatus(T_PARAGRAPH_BRK); break;
      case SET_T_ARABIC : text.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_arabic")); break;
      case SET_T_LATIN_ARABIC : text.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_latin_arabic")); break;
      case SET_T_ART_DEF : text.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_art_def")); break;
      case SET_T_ACRONYM_ARABIC : text.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_acronym_arabic")); break;
      case SET_T_ACRONYM_LATIN_ARABIC : text.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_acronym_latin_arabic")); break;
      default: ;
    }
  }
}

bool Transition::setSetting(const LimaString& s)
{
  std::string str = limastring2utf8stdstring(s);
#ifdef DEBUG_LP
  TOKENIZERLOADERLOGINIT;
  LDEBUG << "    Setting transition status setting to " << str;
#endif
  if (str == "T_CAPITAL")
  {
    m_settings.push_back(SET_T_CAPITAL);
  }
  else if (str == "T_SMALL")
  {
    m_settings.push_back(SET_T_SMALL);
  }
  else if (str == "T_CAPITAL_1ST")
  {
    m_settings.push_back(SET_T_CAPITAL_1ST);
  }
  else if (str == "T_ACRONYM")
  {
    m_settings.push_back(SET_T_ACRONYM);
  }
  else if (str == "T_CAPITAL_SMALL")
  {
    m_settings.push_back(SET_T_CAPITAL_SMALL);
  }
  else if (str == "T_CARDINAL_ROMAN")
  {
    m_settings.push_back(SET_T_CARDINAL_ROMAN);
  }
  else if (str == "T_ORDINAL_ROMAN")
  {
    m_settings.push_back(SET_T_ORDINAL_ROMAN);
  }
  else if (str == "T_NOT_ROMAN")
  {
    m_settings.push_back(SET_T_NOT_ROMAN);
  }
  else if (str == "T_INTEGER")
  {
    m_settings.push_back(SET_T_INTEGER);
  }
  else if (str == "T_COMMA_NUMBER")
  {
    m_settings.push_back(SET_T_COMMA_NUMBER);
  }
  else if (str == "T_DOT_NUMBER")
  {
    m_settings.push_back(SET_T_DOT_NUMBER);
  }
  else if (str == "T_FRACTION")
  {
    m_settings.push_back(SET_T_FRACTION);
  }
  else if (str == "T_ORDINAL_INTEGER")
  {
    m_settings.push_back(SET_T_ORDINAL_INTEGER);
  }
  else if (str == "T_ALPHA")
  {
    m_settings.push_back(SET_T_ALPHA);
  }
  else if (str == "T_NUMERIC")
  {
    m_settings.push_back(SET_T_NUMERIC);
  }
  else if (str == "T_ALPHANUMERIC")
  {
    m_settings.push_back(SET_T_ALPHANUMERIC);
  }
  else if (str == "T_PATTERN")
  {
    m_settings.push_back(SET_T_PATTERN);
  }
  else if (str == "T_WORD_BRK")
  {
    m_settings.push_back(SET_T_WORD_BRK);
  }
  else if (str == "T_SENTENCE_BRK")
  {
    m_settings.push_back(SET_T_SENTENCE_BRK);
  }
  else if (str == "T_PARAGRAPH_BRK")
  {
    m_settings.push_back(SET_T_PARAGRAPH_BRK);
    m_settings.push_back(SET_T_SENTENCE_BRK);
  }
  else if (str == "T_HYPHEN_WORD")
  {
    m_settings.push_back(SET_T_ALPHA_HYPHEN);
  }
  else if (str == "T_POSSESSIVE")
  {
    m_settings.push_back(SET_T_ALPHA_POSSESSIVE);
  }
  else if (str == "T_ALPHA_CONCAT_ABBREV")
  {
    m_settings.push_back(SET_T_ALPHA_CONCAT_ABBREV);
  }
  else if (str == "T_ARABIC")
  {
    m_settings.push_back(SET_T_ARABIC);
  }
  else if (str == "T_LATIN_ARABIC")
  {
    m_settings.push_back(SET_T_LATIN_ARABIC);
  }
  else if (str == "T_ART_DEF")
  {
    m_settings.push_back(SET_T_ART_DEF);
  }
  else if (str == "T_ACRONYM_ARABIC")
  {
    m_settings.push_back(SET_T_ACRONYM_ARABIC);
  }
  else if (str == "T_ACRONYM_LATIN_ARABIC")
  {
    m_settings.push_back(SET_T_ACRONYM_LATIN_ARABIC);
  }
  else if (str == "T_TWITTER")
  {
    m_settings.push_back(SET_T_TWITTER);
  }
  else if (str == "T_ABBREV")
  {
    m_settings.push_back(SET_T_ABBREV);
  }
  else
  {
    TOKENIZERLOGINIT;
    LERROR << "Transition::setSetting at " << __FILE__ << ", line " << __LINE__
        << ": Unkown satus setting '"<<str<<"'";
    return false;
  }
  return true;
}

} //namespace FlatTokenizer
} // namespace LinguisticProcessing
} // namespace Lima
