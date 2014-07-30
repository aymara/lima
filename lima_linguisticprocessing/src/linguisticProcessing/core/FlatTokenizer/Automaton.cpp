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

#include "Automaton.h"
#include "SpiritTokenizerParser.hpp"
#include "SpiritTokenizerLoader.hpp"
#include "State.h"

#include "common/misc/Exceptions.h"
#include "common/Data/strwstrtools.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"


using namespace Lima;
using namespace Lima::Common;
using namespace Lima::Common::Misc;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::XMLConfigurationFiles;

namespace Lima
{
namespace LinguisticProcessing
{
namespace FlatTokenizer
{

SimpleFactory<AbstractResource,Automaton> flatAutomatonFactory(FLATAUTOMATON_CLASSID);

Automaton::Automaton() : AbstractResource(),
    m_startState(0), m_states()
{
}

Automaton::~Automaton()
{
  for (std::vector< State* >::iterator it = m_states.begin();
          it != m_states.end(); it++)
  {
    delete (*it);
  }
}

void Automaton::init(
                     Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
                     Manager* manager)
    
{
  TOKENIZERLOGINIT;
  LDEBUG << "Creating a Tokenizer Automaton (loads file)";
  MediaId language=manager->getInitializationParameters().language;

  try {
    std::string resourcePath=Common::MediaticData::MediaticData::single().getResourcesPath();
    std::string charChartFileName=resourcePath + "/" + unitConfiguration.getParamsValueAtKey("automatonFile");
    loadFromFile(charChartFileName);

  } catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    LERROR << "no parameter 'automatonFile' in tokenizer group for language " << (int) language << " !";
    throw InvalidConfiguration();
  }
}


// Run-time. Returns true if a Transition was found open
const State* Automaton::run(Text& text, const State* state) const
{
  const State* currentState = state;
  if (currentState == 0)
  {
    if (m_startState == 0)
    {
      TOKENIZERLOGINIT;
      LERROR << "No start state";
      return 0;
    }
    currentState = m_startState;
  }
  TOKENIZERLOGINIT;
  LDEBUG << "Running Automaton ; currentState is '"
      << ((currentState==0)?"":Common::Misc::limastring2utf8stdstring(currentState->name())) << "'";
  const State* result =  currentState->run(text);
  if (result == 0 && text.position() < text.size())
  {
    LWARN << "Current state failed: backtracking to start state";
    result = m_startState;
  }
  return result;
}

void Automaton::loadFromFile(const std::string& fileName)
{
  TOKENIZERLOGINIT;
  LDEBUG << "Loading tokenizer automaton from " << fileName;
  SpiritTokenizerLoader parser(*this, fileName);
}

State* Automaton::stateNamed(const LimaString& name)
{
  std::vector<State*>::iterator it, it_end;
  it = m_states.begin(); it_end = m_states.end();
  for (; it != it_end; it++)
  {
    if ( (*it)->name() == name )
    {
      return *it;
    }
  }
  return 0;
}

const State* Automaton::stateNamed(const LimaString& name) const
{
  std::vector<State*>::const_iterator it, it_end;
  it = m_states.begin(); it_end = m_states.end();
  for (; it != it_end; it++)
  {
    if ( (*it)->name() == name )
    {
      return *it;
    }
  }
  return 0;
}



} //namespace FlatTokenizer
} // namespace LinguisticProcessing
} // namespace Lima
