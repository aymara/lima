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
// jys 22-OCT-2002
//
// DefaultProperties is the implementation of the last module of
// Morphological Analysis. Each token from the main tokens
// path which remains without linguuistic properties receives
// properties by default found into dictionnary using Tokenizer
// status names as keys.
//<if token has any linguistic properties, directly or by its
// orthographic alternatives, default properties are not applied>

#include "DefaultProperties.h"

#include "common/MediaticData/mediaticData.h"
#include "common/time/timeUtilsController.h"
#include "common/tools/FileUtils.h"
#include "common/Data/strwstrtools.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "linguisticProcessing/core/FlatTokenizer/CharChart.h"
#include "linguisticProcessing/core/AnalysisDict/AbstractAnalysisDictionary.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/Token.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"

#include <fstream>

using namespace boost;
using namespace Lima::Common::MediaticData;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace std;

namespace Lima
{
namespace LinguisticProcessing
{
namespace MorphologicAnalysis
{

SimpleFactory<MediaProcessUnit,DefaultProperties> defaultPropertiesFactory(DEFAULTPROPERTIES_CLASSID);

DefaultProperties::DefaultProperties() : m_language(std::numeric_limits<unsigned char>::max())
{}

DefaultProperties::~DefaultProperties()
{}

void DefaultProperties::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)
{
  MORPHOLOGINIT;
  m_language = manager->getInitializationParameters().media;
  std::deque<std::string> skipUnmarkStatus;
  try
  {
    QString file = Common::Misc::findFileInPaths(Common::MediaticData::MediaticData::single().getResourcesPath().c_str(),  unitConfiguration.getParamsValueAtKey("defaultPropertyFile").c_str());
    readDefaultsFromFile(file.toUtf8().constData());
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    LERROR << "no param 'defaultPropertyFile' in DefaultProperties group for language " << (int) m_language;
    throw InvalidConfiguration();
  }
  try
  {
    string dico = unitConfiguration.getParamsValueAtKey("charChart");
    AbstractResource* res = LinguisticResources::single().getResource(m_language,dico);
    m_charChart = static_cast< FlatTokenizer::CharChart* >(res);
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    LERROR << "no param 'charChart' in DefaultProperties group for language " << (int) m_language;
    throw InvalidConfiguration();
  }
  
  try
  {
    skipUnmarkStatus=unitConfiguration.getListsValueAtKey("skipUnmarkStatus");
  }
  catch (Common::XMLConfigurationFiles::NoSuchList& )
  {
    // empty display
  }
  //m_skipUnmarkStatus is a set of tokenization status that don't need desaccentuation like t_dot_number. We keep the dot. 
  for( std::deque<std::string>::iterator src = skipUnmarkStatus.begin() ; src != skipUnmarkStatus.end() ; src++ )
  {
    m_skipUnmarkStatus.insert(Common::Misc::utf8stdstring2limastring(*src));
  }

}

/** @brief Each token of the specified path without dictionnary entry is
  *        searched into the specified dictionary
  * @author Jean-Yves Sages
  *
  */
LimaStatusCode DefaultProperties::process(
  AnalysisContent& analysis) const
{
  Lima::TimeUtilsController timer("DefaultProperties");
  MORPHOLOGINIT;

  AnalysisGraph* tokenList=static_cast<AnalysisGraph*>(analysis.getData("AnalysisGraph"));
  LinguisticGraph* g=tokenList->getGraph();

  try
  {

    LinguisticGraphVertexIt v, vEnd;
    boost::tie(v, vEnd) = vertices(*g);

    for (; v != vEnd; v++)
    {
      MorphoSyntacticData* currentData=get(vertex_data, *g, *v);
      if (currentData == 0)
      {
        continue;
      } // NULL for first and last vertex

      Token* currentToken=get(vertex_token, *g, *v);

      //<if token has any linguistic properties, directly or by its
      // orthographic alternatives, default properties are not applied>
      if (currentData->empty())
      {
        std::map<LimaString,std::vector<LinguisticCode> >::const_iterator it=m_defaults.find(currentToken->status().defaultKey());
        if (it!=m_defaults.end()) {
          LinguisticElement elem;
          elem.inflectedForm=currentToken->form();
          LimaString str=currentToken->stringForm();
          if(m_skipUnmarkStatus.find(currentToken->status().defaultKey())==m_skipUnmarkStatus.end()){
            str = m_charChart->unmark(currentToken->stringForm());
          }
          elem.lemma= Common::MediaticData::MediaticData::changeable().stringsPool(m_language)[str];
          elem.normalizedForm=elem.lemma;
          elem.type=UNKNOWN_WORD;
          
          for (std::vector<LinguisticCode>::const_iterator codeItr=it->second.begin();
               codeItr!=it->second.end();
               codeItr++)
          {
            elem.properties=*codeItr;
            currentData->push_back(elem);
          }
        } else {
          LWARN << "No default property for " 
            << Common::Misc::limastring2utf8stdstring(currentToken->stringForm()) << ". Status : "
            << Common::Misc::limastring2utf8stdstring(currentToken->status().defaultKey());
        }
      }
    }
  }
  catch (std::exception &exc)
  {
    MORPHOLOGINIT;
    LWARN << "Exception in Default Properties :  " << exc.what();
    return UNKNOWN_ERROR;
  }

  return SUCCESS_ID;
}

void DefaultProperties::readDefaultsFromFile(const std::string& filename)
{
  MORPHOLOGINIT;
  LINFO << "read default properties from file : " << filename;
  ifstream fin(filename.c_str(), std::ifstream::binary);
  string line;
  string type;
  LinguisticCode props;
  while (fin.good() && !fin.eof()) {
    line = Lima::Common::Misc::readLine(fin);
    if (line.size()>0) {
      istringstream is(line);
      is >> type;
      is >> props;
#ifdef DEBUG_LP
      LDEBUG << "read default " << type << " => " << props;
#endif
      m_defaults[Common::Misc::utf8stdstring2limastring(type)].push_back(props);
    }
  }
}

} // MorphologicAnalysis
} // LinguisticProcessing
} // Lima
