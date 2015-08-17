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
// jys 23-JUL-2002
//
// State is the recovery point for automatons. State is a
// collection of Transition and eventually inner automatons.
// Inner automatons are called once when state is entered at
// first time. Inner automatons are called before checking
// transition. Inner automatons can return a status which
// ccan be used by transitions.

#include "State.h"

#include "Transition.h"
#include "common/misc/Exceptions.h"

using namespace Lima::Common::Misc;

namespace Lima
{
namespace LinguisticProcessing
{
namespace FlatTokenizer
{


State::State(const LimaString& name, Automaton& automaton) :
    m_automaton(automaton),
    m_transitions(),
    m_name(name)
{
}

State::~State()
{
    for (std::list< Transition* >::iterator it = m_transitions.begin();
            it != m_transitions.end(); it++)
    {
        delete (*it); (*it) = 0;
    }
    m_transitions.clear();
}

// Run-Time.
// Returns true if a transition was founs open
const State* State::run(Text& text) const
{
#ifdef DEBUG_LP
  TOKENIZERLOGINIT;
  LDEBUG << "--------Entering State " << limastring2utf8stdstring(name());
#endif
//   Lima::LimaChar returnedStatus;
//   Lima::LimaChar innerStatus = 0;

  std::list< Transition* >::const_iterator it = m_transitions.begin();
  std::list< Transition* >::const_iterator it_end = m_transitions.end();
  for (; it != it_end; it++)
  {
    const Transition* transition = (*it);
    if (transition->events().size() == 0)
    {
#ifdef DEBUG_LP
      LDEBUG << "| Empty transition... continuing.";
#endif
      continue;
    }
    
#ifdef DEBUG_LP
    LDEBUG << "| state " << limastring2utf8stdstring(name())
        << " running transition first event="
        << (transition->events()[0]==0?"":limastring2utf8stdstring(transition->events()[0]->id()))
        <<" / '"
        << (transition->events()[0]==0?"":limastring2utf8stdstring(transition->events()[0]->name()))
        << "'";
    LDEBUG << "|   (text position " << text.position() << " ; char: '" << Common::Misc::limastring2utf8stdstring(LimaString()+text.currentChar()) << "')";
#endif
    const State* toState = transition->run(text);
    if (toState != 0)
    {
#ifdef DEBUG_LP
          LDEBUG << "| transition from " << Common::Misc::limastring2utf8stdstring(this->name()) << " to " << Common::Misc::limastring2utf8stdstring(transition-> nextStateName()) << " succeeded.";
          LDEBUG << "| text position is now " << text.position() << " ; char: '" << Common::Misc::limastring2utf8stdstring(LimaString()+text.currentChar()) << "'";
#endif
        return toState;
    }
  }
#ifdef DEBUG_LP
  LDEBUG << "--------All transitions failed: state failed";
#endif
  return 0;
}

} //namespace FlatTokenizer
} // namespace LinguisticProcessing
} // namespace Lima
