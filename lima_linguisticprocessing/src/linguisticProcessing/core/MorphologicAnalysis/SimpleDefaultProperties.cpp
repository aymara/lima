// Copyright 2002-2020 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

//
// DefaultProperties is the implementation of the last module of
// Morphological Analysis. Each token from the main tokens
// path which remains without linguuistic properties receives
// properties by default found into dictionnary using Tokenizer
// status names as keys.
//<if token has any linguistic properties, directly or by its
// orthographic alternatives, default properties are not applied>

#include "SimpleDefaultProperties.h"

#include "common/MediaticData/mediaticData.h"

#include "linguisticProcessing/core/FlatTokenizer/CharChart.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/Token.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/TStatus.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticData.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/time/traceUtils.h"

using namespace boost;
using namespace std;
using namespace Lima::Common::MediaticData;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

namespace Lima
{
namespace LinguisticProcessing
{
namespace MorphologicAnalysis
{

SimpleFactory<MediaProcessUnit,SimpleDefaultProperties> simpleDefaultPropertiesFactory(SIMPLEDEFAULTPROPERTIES_CLASSID);

SimpleDefaultProperties::SimpleDefaultProperties()
{}

SimpleDefaultProperties::~SimpleDefaultProperties()
{}

void SimpleDefaultProperties::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)
{
  MORPHOLOGINIT;
  m_language = manager->getInitializationParameters().media;
  const Common::PropertyCode::PropertyManager& microManager = static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyManager("MICRO");

  try
  {
    deque<string> cats=unitConfiguration.getListsValueAtKey("defaultCategories");
    for (deque<string>::iterator it=cats.begin();
         it!=cats.end();
         it++)
    {
      LinguisticCode microId=microManager.getPropertyValue(*it);
      LDEBUG << "default category : " << microId;
      if (!microManager.getPropertyAccessor().empty(microId) )
      {
        m_defaultProperties.push_back(microId);
      }
      else
      {
        LWARN << "micro categorie seems to be undefined !";
        throw InvalidConfiguration();
      }
    }
  }
  catch (Lima::Common::XMLConfigurationFiles::NoSuchList& )
  {
    MORPHOLOGINIT;
    LWARN << "SimpleDefaultCateogries: no default properties defined !";
    m_defaultProperties.push_back(L_NONE);
  }
  try
  {
    string dico = unitConfiguration.getParamsValueAtKey("charChart");
    AbstractResource* res = LinguisticResources::single().getResource(m_language,dico);
    m_charChart = static_cast< FlatTokenizer::CharChart* >(res);
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    LERROR << "no param 'charChart' in SimpleDefaultCateogries group for language " << (int)m_language;
    throw InvalidConfiguration();
  }

}

/** @brief Each token of the specified path without dictionnary entry is
  *        searched into the specified dictionary
  * @author Jean-Yves Sages
  *
  */
LimaStatusCode SimpleDefaultProperties::process(
  AnalysisContent& analysis) const
{
  TimeUtils::updateCurrentTime();
  MORPHOLOGINIT;
  LINFO << "MorphologicalAnalysis: starting process DefaultProperties";

  auto tokenList=std::dynamic_pointer_cast<AnalysisGraph>(analysis.getData("AnalysisGraph"));

  // Affectation des propriétés par défaut
  affectPropertiesOnePath(*tokenList);

  // Graphe après l'affectation des propriétés par défaut
  // std::cout << std::endl;
  // std::cout << "Graph after Default Properties:" << std::endl;

  // Fin de l'analyse morphologique
  // std::cout << "End of Morphological Analysis" << std::endl;
  // std::cout << std::endl;

  LINFO << "MorphologicalAnalysis: ending process SimpleDefaultProperties";
  TimeUtils::logElapsedTime("SimpleDefaultProperties");
  return SUCCESS_ID;
}


// directly use the list of vertices (do not need to go through the graph)
void SimpleDefaultProperties::affectPropertiesOnePath(
  AnalysisGraph& anagraph) const
{
  graph_traits<LinguisticGraph>::vertex_iterator vBegin, vEnd, v;
  boost::tie(vBegin, vEnd) = vertices(*(anagraph.getGraph()));
  FsaStringsPool& sp= Common::MediaticData::MediaticData::changeable().stringsPool(m_language);

  for (v = vBegin; v != vEnd; v++)
  {
    MorphoSyntacticData* currentData=get(vertex_data, *(anagraph.getGraph()), *v);
    if (currentData == 0)
    {
      continue;
    } // NULL for first and last vertex

    //<if token has any linguistic properties, directly or by its
    // orthographic alternatives, default properties are not applied>
    if (currentData->empty())
    {
      Token* token=get(vertex_token, *(anagraph.getGraph()), *v);
      LimaString str = m_charChart->unmark(token->stringForm());
      LinguisticElement elem;
      elem.inflectedForm=token->form();
      elem.lemma=sp[str];
      elem.normalizedForm=elem.lemma;
      elem.type=UNKNOWN_WORD;
      for (vector<LinguisticCode>::const_iterator it=m_defaultProperties.begin();
           it!=m_defaultProperties.end();
           it++)
      {
        elem.properties=*it;
        currentData->push_back(elem);
      }
    }
  }
}

} // MorphologicAnalysis
} // LinguisticProcessing
} // Lima
