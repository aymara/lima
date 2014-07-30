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

#include "linguisticProcessing/core/FlatTokenizer/TokenizerAutomaton.h"

#include "linguisticProcessing/core/FlatTokenizer/Automaton.h"
#include "linguisticProcessing/core/FlatTokenizer/State.h"
#include "linguisticProcessing/core/FlatTokenizer/CharChart.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/core/LinguisticProcessors/LimaStringText.h"
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
  
  try
  {
    string resourcesPath=Common::MediaticData::MediaticData::single().getResourcesPath();
    string fileName=resourcesPath +"/"+unitConfiguration.getParamsValueAtKey("automatonFile");

  }
  catch (NoSuchParam& )
  {
    LERROR << "no param 'automatonFile' in TokenizerAutomaton group configuration (language="
    << (int) _language << ")";
    throw InvalidConfiguration();
  }
  // when input XML file is syntactically wrong
  catch (XmlSyntaxException exc)
  {
    std::ostringstream mess;
    mess << "XmlSyntaxException at line "<<exc._lineNumber<<" cause: ";
    switch (exc._why)
    {
        case XmlSyntaxException::SYNTAX_EXC : mess << "SYNTAX_EXC"; break;
        case XmlSyntaxException::NO_DATA_EXC : mess << "NO_DATA_EXC"; break;
        case XmlSyntaxException::DOUBLE_EXC : mess << "DOUBLE_EXC"; break;
        case XmlSyntaxException::FWD_CLASS_EXC : mess << "FWD_CLASS_EXC"; break;
        case XmlSyntaxException::MULT_CLASS_EXC : mess << "MULT_CLASS_EXC"; break;
        case XmlSyntaxException::EOF_EXC : mess << "EOF_EXC"; break;
        case XmlSyntaxException::NO_CODE_EXC : mess << "NO_CODE_EXC"; break;
        case XmlSyntaxException::BAD_CODE_EXC : mess << "BAD_CODE_EXC"; break;
        case XmlSyntaxException::NO_CLASS_EXC : mess << "NO_CLASS_EXC"; break;
        case XmlSyntaxException::UNK_CLASS_EXC : mess << "UNK_CLASS_EXC"; break;
        case XmlSyntaxException::INT_ERROR_EXC : mess << "INT_ERROR_EXC"; break;
        case XmlSyntaxException::INV_CLASS_EXC : mess << "INV_CLASS_EXC"; break;
        default: mess << "??";
    }
    LERROR << mess.str();
    throw InvalidConfiguration();
  }
  catch (std::exception &exc)
  {
    // @todo remove all causes of InfiniteLoopException
    LERROR << exc.what();
    throw InvalidConfiguration();
  }

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
