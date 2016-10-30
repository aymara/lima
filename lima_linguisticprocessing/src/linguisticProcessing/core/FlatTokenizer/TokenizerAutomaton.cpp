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
// TokenizerAutomaton is the main program of TokenizerAutomaton stuff.

#include "TokenizerAutomaton.h"

#include "Automaton.h"
#include "State.h"
#include "CharChart.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/common/linguisticData/LimaStringText.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "common/Data/strwstrtools.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/MediaticData/mediaticData.h"
#include "common/time/traceUtils.h"
#include "common/misc/Exceptions.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include <string>

using namespace std;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::Common::Misc;

namespace Lima
{
namespace LinguisticProcessing
{
namespace FlatTokenizer
{

SimpleFactory<AbstractResource,TokenizerAutomaton> flatTokenizerFactory(FLATTOKENIZERAUTOMATON_CLASSID);

TokenizerAutomaton::TokenizerAutomaton() : m_text(0)
{}

TokenizerAutomaton::~TokenizerAutomaton()
{
  delete m_text;
  delete _automaton;
}

void TokenizerAutomaton::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  TOKENIZERLOGINIT;
  LDEBUG << "TokenizerAutomaton::init";
  MediaId _language=manager->getInitializationParameters().language;
  try
  {
    string charchartId=unitConfiguration.getParamsValueAtKey("charChart");
    AbstractResource* res=LinguisticResources::single().getResource(_language,charchartId);
    _charChart=static_cast<CharChart*>(res);
  }
  catch (NoSuchParam& )
  {
    LERROR << "no param 'charChart' in TokenizerAutomaton group configuration (language="
    << (int) _language << ")";
    throw InvalidConfiguration();
  }

  m_text=new Text(_language,_charChart);
}

const State* TokenizerAutomaton::stateNamed(const LimaString& name) const
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

State* TokenizerAutomaton::stateNamed(const LimaString& name)
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

} //namespace FlatTokenizerAutomaton
} // namespace LinguisticProcessing
} // namespace Lima
