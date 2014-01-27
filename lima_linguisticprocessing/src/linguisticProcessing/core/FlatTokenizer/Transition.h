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

#ifndef LIMA_LINGUISTICPROCESSING_FLATTOKENIZER_Transition_H
#define LIMA_LINGUISTICPROCESSING_FLATTOKENIZER_Transition_H

#include "FlatTokenizerExport.h"
#include "common/Data/LimaString.h"
#include "common/Data/strwstrtools.h"
#include "ReturnStatus.h"
#include "Text.h"
#include "Events.h"
#include "State.h"
#include "Condition.h"

#include "linguisticProcessing/core/LinguisticAnalysisStructure/TStatus.h"
// #include "linguisticProcessing/core/Tokenizer/ParseMarkup.h"

namespace Lima
{
namespace LinguisticProcessing
{
namespace FlatTokenizer
{

class LIMA_FLATTOKENIZER_EXPORT Transition
{
public:
  enum Setting
  {
    SET_T_ALPHA,               // 0
    SET_T_NUMERIC,             // 1
    SET_T_PATTERN,             // 2
    SET_T_WORD_BRK,            // 3
    SET_T_SENTENCE_BRK,        // 4
    SET_T_ALPHANUMERIC,        // 5
    SET_T_ALPHA_HYPHEN,        // 6
    SET_T_ALPHA_POSSESSIVE,    // 7
    SET_T_CAPITAL,             // 8
    SET_T_SMALL,               // 9
    SET_T_CAPITAL_1ST,         // 10
    SET_T_ACRONYM,             // 11
    SET_T_CAPITAL_SMALL,       // 12
    SET_T_CARDINAL_ROMAN,      // 13
    SET_T_ORDINAL_ROMAN,       // 14
    SET_T_NOT_ROMAN,           // 15
    SET_T_INTEGER,             // 16
    SET_T_COMMA_NUMBER,        // 17
    SET_T_DOT_NUMBER,          // 18
    SET_T_FRACTION,            // 19
    SET_T_ORDINAL_INTEGER,     // 20
    SET_T_ALPHA_CONCAT_ABBREV, // 21
    SET_T_PARAGRAPH_BRK,       // 22
    SET_T_ARABIC,              // 23
    SET_T_LATIN_ARABIC,        // 24
    SET_T_ART_DEF,             // 25
    SET_T_ACRONYM_ARABIC,      // 26
    SET_T_ACRONYM_LATIN_ARABIC,// 27
    SET_T_TWITTER              // 28
  };
  static const char* SettingNames[];

  explicit Transition(const State* state);
  virtual ~Transition();

  // for run-time use. Transition does its works.
  // Returns true if open, false otherwise
  const State* run(Text& text) const;

  inline const Lima::LimaString nextStateName() const
  {
      if (_toState) return (_toState->name());
    else return Common::Misc::utf8stdstring2limastring("NONE");
  }

  inline const Condition& conditions() const {return _condition;}
  inline Condition& conditions() {return _condition;}

  inline const Events& events() const {return _events;}
  inline Events& events() {return _events;}

//   inline const State* toState() const {return _toState;}
//   inline State* toState() {return _toState;}

  inline void setToState(State* state) {_toState = state;}

/*  inline const LinguisticAnalysisStructure::TStatus& setting() const {return m_status;}
  inline LinguisticAnalysisStructure::TStatus& setting() {return m_status;}*/
  bool setSetting(const LimaString& s);

  inline bool tokenize() const {return m_tokenize;}
  inline void setTokenize(bool b) {m_tokenize = b;}
  
  inline bool flush() const {return m_flush;}
  inline void setFlush(bool f) {m_flush = f;}
  
  inline const Lima::LimaString& defaultKey() const {return m_defaultKey;}
  inline void setDefaultKey(const Lima::LimaString& key) {m_defaultKey = key;}

private:
  Transition(const Transition&);
  Transition& operator=(const Transition&);
  void applySettings(Text& text) const;
  
  const State* m_state;
  State* _toState;
  Events _events;
  Condition _condition;            //0 means no conditions
  bool m_tokenize;
  bool m_flush;
  Lima::LimaString m_defaultKey;

//   LinguisticAnalysisStructure::TStatus m_status;

  std::vector<Setting> m_settings;
};

} //namespace FlatTokenizer
} // namespace LinguisticProcessing
} // namespace Lima

#endif
