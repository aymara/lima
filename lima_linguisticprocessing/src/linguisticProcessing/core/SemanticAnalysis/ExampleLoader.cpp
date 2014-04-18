/*
    Copyright 2002-2014 CEA LIST

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
/************************************************************************
 *
 * @file       ExampleLoader.cpp
 * @author     Clémence Filmont <clemence.filmont@cea.fr>
 * @date       2014-04-17
 * copyright   Copyright (C) 2014 by CEA LIST
 *
 ***********************************************************************/

#include "ExampleLoader.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "common/Data/strwstrtools.h"
#include "linguisticProcessing/core/Automaton/recognizerMatch.h"
#include "linguisticProcessing/core/Automaton/recognizerData.h"
#include "common/MediaticData/mediaticData.h"
#include <queue>


using namespace std;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::LinguisticProcessing::ApplyRecognizer;

namespace Lima {
namespace LinguisticProcessing {
namespace Example {

SimpleFactory<MediaProcessUnit,ExampleLoader> ExampleLoaderFactory(EXAMPLELOADER_CLASSID);

//***********************************************************************
// constructors and destructors
ExampleLoader::ExampleLoader():
m_language(0),
m_graph("AnalysisGraph")
{
  m_parser = new QXmlSimpleReader();
}

ExampleLoader::~ExampleLoader()
{
}

//***********************************************************************
void ExampleLoader::init(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
          Manager* manager)
{
  PROCESSORSLOGINIT;

  m_language=manager->getInitializationParameters().media;

  AnalysisLoader::init(unitConfiguration,manager);

}

LimaStatusCode ExampleLoader::process(AnalysisContent& analysis) const
{
  // get linguistic graph
  AnalysisGraph* anaGraph=static_cast<AnalysisGraph*>(analysis.getData("PosGraph"));
  LinguisticGraph* lingGraph=anaGraph->getGraph();
  if (lingGraph==0)
  {
    PROCESSORSLOGINIT;
    LERROR << "no graph 'PosGraph' available !";
    return MISSING_DATA;
  }
  else{
    try{
      ExampleLoader::XMLHandler handler(m_language,analysis,anaGraph);
      m_parser->setContentHandler(&handler);
      m_parser->setErrorHandler(&handler);
      QFile file("/tmp/mm-lp.morphoSyntacticalAnalysis_modif.tmp");
      if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        throw XMLException();
      if (!m_parser->parse( QXmlInputSource(&file)))
      {
        throw XMLException();
      }
      LinguisticGraph::vertex_iterator vxItr,vxItrEnd;
      boost::tie(vxItr,vxItrEnd) = boost::vertices(*lingGraph);
      for (;vxItr!=vxItrEnd;vxItr++){
       MorphoSyntacticData* morphoData=get(vertex_data,*lingGraph, vxItr);
        Token* ft=get(vertex_token,*lingGraph,*vxItr);
        if( ft!=0){
          const QString tag=QString::fromStdString(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyManager("MICRO").getPropertySymbolicValue(handler.m_tagIndex[ft->position()]));
          //si différence entre valeur de la map et noeud du graphe à la position n, remplacer la valeur du noeud //par la valeur de la map
          if(tag!=ft->stringForm()){
              cout << "le token a la position " << ft->position() << "passe de " << ft->stringForm() << "a " << tag << endl;
              cout << " il y a " << morphoData->size() << " catégories dans le graphe " << endl; 
              //morphoData
            }
        }
      }
    }
    catch (const XMLException& ){
      PROCESSORSLOGINIT;
      LERROR << "Error: failed to parse XML input file";
    }
     return SUCCESS_ID;
  }
}


//***********************************************************************
// XML handler
ExampleLoader::XMLHandler::XMLHandler(MediaId language, AnalysisContent& analysis, AnalysisGraph* graph):
m_tagIndex(),
m_language(language),
m_analysis(analysis),
m_graph(graph),
m_position(0),
m_length(0),
m_type(),
m_string(),
m_currentElement()
{
  PROCESSORSLOGINIT;
  LDEBUG << "ExampleLoader::XMLHandler constructor";
}

ExampleLoader::XMLHandler::~XMLHandler()
{
}


bool ExampleLoader::XMLHandler::startElement(const QString & namespaceURI, const QString & eltName, const QString & qName, const QXmlAttributes & attributes)
{
  LIMA_UNUSED(namespaceURI);
  LIMA_UNUSED(qName);
  //PROCESSORSLOGINIT;
  //LDEBUG << "ExampleLoader::XMLHandler start element "  << eltName;

  if (eltName=="w")
  {
    LinguisticCode posInt=static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyManager("MICRO").getPropertyValue(attributes.value("pos").toStdString());
    m_tagIndex[attributes.value("p").toInt()] = posInt;
  }
  return true;
}

bool ExampleLoader::XMLHandler::warning(const QXmlParseException& e)
{
  PROCESSORSLOGINIT;
  LERROR << "Error at file " << e.systemId()
         << ", line " << e.lineNumber()
         << ", char " << e.columnNumber()
         << "  Message: " << e.message();
         return true;
}
bool ExampleLoader::XMLHandler::error(const QXmlParseException& e)
{
  PROCESSORSLOGINIT;
  LERROR << "Fatal error at file " << e.systemId()
         << ", line " << e.lineNumber()
         << ", char " << e.columnNumber()
         << "  Message: " << e.message();
         return false;
}
bool ExampleLoader::XMLHandler::fatalError(const QXmlParseException& e)
{
  PROCESSORSLOGINIT;
  LWARN << "Warning at file " << e.systemId()
        << ", line " << e.lineNumber()
        << ", char " << e.columnNumber()
        << "  Message: " << e.message();
        return false;
}


} // end namespace
} // end namespace
} // end namespace
