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
#include "SpiritTokenizerLoader.hpp"
#include "SpiritTokenizerParser.hpp"

#include "State.h"
#include "Transition.h"
#include "Condition.h"

#include <fstream>

namespace Lima
{
namespace LinguisticProcessing
{
namespace FlatTokenizer
{

SpiritTokenizerLoader::SpiritTokenizerLoader(Automaton& automaton, const std::string& fileName) :
    m_automaton(automaton)
{
  TOKENIZERLOADERLOGINIT;
  namespace qi = boost::spirit::qi;
  namespace ascii = boost::spirit::ascii;
  using ascii::space;
  typedef std::string::const_iterator iterator_type;
  typedef tokenizer<iterator_type> tokenizer;
  
  tokenizer tokenizer_parser; // Our grammar
  
  std::string str;
  std::ifstream file(fileName.c_str(), std::ifstream::binary);
  Lima::Common::Misc::readStream(file, str);

  std::string::const_iterator iter = str.begin();
  std::string::const_iterator end = str.end();

  tokenizer_automaton tokenizer_automaton;
  bool r = phrase_parse(iter, end, tokenizer_parser, skipper, tokenizer_automaton);

  if (r && iter == end)
  {
    LDEBUG << "Parsing " << fileName << " succeeded: "<<tokenizer_automaton.size()<<" states";
    std::vector<tokenizer_state>::const_iterator it, itBegin, itEnd;
    itBegin = tokenizer_automaton.begin();
    it = tokenizer_automaton.begin(); itEnd = tokenizer_automaton.end();
    for (; it != itEnd; it++)
    {
      const tokenizer_state& state = *it;
      LDEBUG << "Loading state " << state.id;
      State* newState = stateNamed(Common::Misc::utf8stdstring2limastring(state.id));
      std::vector<tokenizer_transition>::const_iterator tit, titEnd;
      tit = state.transitions.begin(); titEnd = state.transitions.end();
      for (; tit != titEnd; tit++)
      {
        // create transition
        const tokenizer_transition& transition = *tit;
        LDEBUG << "  Loading transition to " << transition.target << " (" << stateNamed(Common::Misc::utf8stdstring2limastring(transition.target)) << ") type=" << transition.transition;
        Transition* newTransition = new Transition(newState);
        LDEBUG << "          transition is " << newTransition;
        // setting target set
        newTransition->setToState(stateNamed(Common::Misc::utf8stdstring2limastring(transition.target)));
        // setting events
        std::vector<std::string>::const_iterator eventIt, eventItEnd;
        eventIt = transition.event.begin(); eventItEnd = transition.event.end();
        for (; eventIt!= eventItEnd; eventIt++)
        {
          LDEBUG << "    Adding event " << *eventIt;
          newTransition->events().push_back(m_automaton.charChart()->classNamed(Common::Misc::utf8stdstring2limastring(*eventIt)));
        }
        // setting preconditions
        std::vector<tokenizer_precondition>::const_iterator preconditionsIt, preconditionsItEnd;
        preconditionsIt = transition.preconditions.begin(); preconditionsItEnd = transition.preconditions.end();
        for (; preconditionsIt != preconditionsItEnd; preconditionsIt++)
        {
          LDEBUG << "    Adding precondition";
          newTransition->conditions().before().push_back(Events(m_automaton.charChart()));
          const tokenizer_precondition& precondition = *preconditionsIt;
          tokenizer_precondition::const_iterator preIt, preItEnd;
          preIt = precondition.begin(); preItEnd = precondition.end();
          for (; preIt != preItEnd; preIt++)
          {
            LDEBUG << "      " << *preIt;
            newTransition->conditions().before().back().addEventNamed(Common::Misc::utf8stdstring2limastring(*preIt));
          }
        }
        // setting postconditions
        std::vector<tokenizer_postcondition>::const_iterator postconditionsIt, postconditionsItEnd;
        postconditionsIt = transition.postconditions.begin(); postconditionsItEnd = transition.postconditions.end();
        for (; postconditionsIt != postconditionsItEnd; postconditionsIt++)
        {
          LDEBUG << "    Adding postcondition";
          newTransition->conditions().after().push_back(Events(m_automaton.charChart()));
          const tokenizer_postcondition& postcondition = *postconditionsIt;
          tokenizer_postcondition::const_iterator postIt, postItEnd;
          postIt = postcondition.begin(); postItEnd = postcondition.end();
          for (; postIt != postItEnd; postIt++)
          {
            LDEBUG << "      " << *postIt << " ; postconditions size: " << newTransition->conditions().after().size();
            newTransition->conditions().after().back().addEventNamed(Common::Misc::utf8stdstring2limastring(*postIt));
          }
        }
        // setting parameters
        newTransition->setFlush(transition.transition == FLUSH);
        LDEBUG << "      setTokenize: " << (transition.transition == TOKEN);
        newTransition->setTokenize(transition.transition == TOKEN);
        // setting statuses
        std::vector<std::string>::const_iterator statusesIt, statusesItEnd;
        statusesIt = transition.statuses.begin(); statusesItEnd = transition.statuses.end();
        for (; statusesIt != statusesItEnd; statusesIt++)
        {
          if (!newTransition->setSetting(Common::Misc::utf8stdstring2limastring(*statusesIt)))
          {
            LERROR << "Unknown status setting: " << *statusesIt;
          }
        }
        // store transition
        newState->transitions().push_back(newTransition);
      }
      if (it == itBegin)
      {
        m_automaton.setStartState(newState);
      }
    }
  }
  else
  {
    LERROR << "Parsing of tokenizer file failed: " << fileName;
  }
}

State* SpiritTokenizerLoader::stateNamed(const LimaString& name)
{
  State* newState = m_automaton.stateNamed(name);
  if (newState == 0)
  {
    newState = new State(name, m_automaton);
    m_automaton.states().push_back(newState);
  }
  return newState;
}

}
}
}
