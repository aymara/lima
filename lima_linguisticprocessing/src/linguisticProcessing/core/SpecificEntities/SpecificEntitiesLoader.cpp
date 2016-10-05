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
/************************************************************************
 *
 * @file       SpecificEntitiesLoader.cpp
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Thu Jun 16 2011
 * copyright   Copyright (C) 2011 by CEA LIST
 * 
 ***********************************************************************/

#include "SpecificEntitiesLoader.h"
#include "SpecificEntitiesConstraints.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "common/tools/FileUtils.h"
#include "common/Data/strwstrtools.h"
#include "linguisticProcessing/core/Automaton/recognizerMatch.h"
#include "linguisticProcessing/core/Automaton/recognizerData.h"
#include <queue>

using namespace std;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::LinguisticProcessing::ApplyRecognizer;

namespace Lima {
namespace LinguisticProcessing {
namespace SpecificEntities {

SimpleFactory<MediaProcessUnit,SpecificEntitiesLoader> SpecificEntitiesLoaderFactory(SPECIFICENTITIESLOADER_CLASSID);

//***********************************************************************
// constructors and destructors
SpecificEntitiesLoader::SpecificEntitiesLoader():
m_language(0),
m_graph("AnalysisGraph"),
m_parser(0)
{
}

SpecificEntitiesLoader::~SpecificEntitiesLoader() 
{
}

//***********************************************************************
void SpecificEntitiesLoader::
init(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
          Manager* manager)
  
{
  LOGINIT("LP::SpecificEntities");

  m_language=manager->getInitializationParameters().media;
  
  AnalysisLoader::init(unitConfiguration,manager);
  try {
    m_graph=unitConfiguration.getParamsValueAtKey("graph");
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {} // keep default value

  try {
    // may need to initialize a modex, to know about the entities in external file
    deque<string> modex=unitConfiguration.getListsValueAtKey("modex");
    for (deque<string>::const_iterator it=modex.begin(),it_end=modex.end();it!=it_end;it++) {
      LDEBUG << "loader: initialize modex " << *it;
      QString filename = Common::Misc::findFileInPaths(Common::MediaticData::MediaticData::single().getConfigPath().c_str(),(*it).c_str());
      Common::XMLConfigurationFiles::XMLConfigurationFileParser parser(filename.toUtf8().constData());
      Common::MediaticData::MediaticData::changeable().initEntityTypes(parser);
    }
  }
  catch (Common::XMLConfigurationFiles::NoSuchList& ) {
    LWARN << "loader: no modex specified in parameter: types in file loaded may not be known";
  }

  //  Create a SAX parser object. 
  m_parser = new QXmlSimpleReader();
}

LimaStatusCode SpecificEntitiesLoader::
process(AnalysisContent& analysis) const
{
  // get analysis graph
  AnalysisGraph* graph=static_cast<AnalysisGraph*>(analysis.getData(m_graph));
  if (graph==0)
  {
    LOGINIT("LP::SpecificEntities");
    LERROR << "no graph '" << m_graph << "' available !";
    return MISSING_DATA;
  }

  //create a RecognizerData (such as in ApplyRecognizer) to be able to use
  //CreateSpecificEntity actions
  RecognizerData* recoData=new RecognizerData;
  analysis.setData("RecognizerData",recoData);
  RecognizerResultData* resultData=new RecognizerResultData(m_graph);
  recoData->setResultData(resultData);
  
  try
  {
    SpecificEntitiesLoader::XMLHandler handler(m_language,analysis,graph);
    m_parser->setContentHandler(&handler);
    m_parser->setErrorHandler(&handler);
#ifdef ANTINNO_SPECIFIC
    QFile file(getInputFile(analysis).c_str());
#else
    QFile file(getInputFile(analysis));
#endif
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
      throw XMLException();
    if (!m_parser->parse( QXmlInputSource(&file)))
    {
      throw XMLException();
    }
  }
  catch (const XMLException& )
  {
    LOGINIT("LP::SpecificEntities");
    LERROR << "Error: failed to parse XML input file";
  }

  // remove recognizer data (used only internally to this process unit)
  recoData->deleteResultData();
  resultData=0;
  analysis.removeData("RecognizerData");

  return SUCCESS_ID;
}

//***********************************************************************
void SpecificEntitiesLoader::XMLHandler::
addSpecificEntity(AnalysisContent& analysis,
                  LinguisticAnalysisStructure::AnalysisGraph* anagraph,
                  const std::string& str,
                  const std::string& type, 
                  uint64_t position, 
                  uint64_t length)
{
  LOGINIT("LP::SpecificEntities");
  LDEBUG << "loader: add entity " << str << "," << type << ",[" << position << "," << length << "]";
  // create RecognizerMatch
  Automaton::RecognizerMatch match(anagraph);

  uint64_t posBegin=position;
  uint64_t posEnd=posBegin+length;

  // find vertices corresponding to pos/len : have to go through the graph
  LinguisticGraph* graph=anagraph->getGraph();
  std::queue<LinguisticGraphVertex> toVisit;
  std::set<LinguisticGraphVertex> visited;
  LinguisticGraphOutEdgeIt outItr,outItrEnd;
 
  // output vertices between begin and end,
  toVisit.push(anagraph->firstVertex());

  bool first=true;
  bool inEntity=false;
  while (!toVisit.empty()) {
    LinguisticGraphVertex v=toVisit.front();
    toVisit.pop();
    if (v == anagraph->lastVertex()) {
      break;
    }

    if (first) {
      first=false;
    }
    else if (inEntity) {
      LinguisticAnalysisStructure::Token* t=get(vertex_token,*graph,v);
      if (t->position() >= posEnd) {
        inEntity=false;
        break; // no need to go further
      }
      else {
        // OME?? valeur true?,"SE"?
        match.addBackVertex(v,true,"SE");
      }
    }
    else {
      LinguisticAnalysisStructure::Token* t=get(vertex_token,*graph,v);
      if(t!=0) {
        if (t->position() == posBegin) {
          match.addBackVertex(v,true,"SE");
          inEntity=true;
        }
      }
    }

    // add next vertices
    for (boost::tie(outItr,outItrEnd)=out_edges(v,*graph); outItr!=outItrEnd; outItr++) 
    {
      LinguisticGraphVertex next=target(*outItr,*graph);
      if (visited.find(next)==visited.end())
      {
        visited.insert(next);
        toVisit.push(next);
      }
    }
  }

  if (match.size()==0) {
    LWARN << "Warning: no matching vertices for given position/length";
  }

  // set entity properties
  match.setType(Common::MediaticData::MediaticData::single().getEntityType(Common::Misc::utf8stdstring2limastring(type)));
  // set normalized form similar to string (otherwise, may cause problem when trying to access the created specific entity)
  match.features().setFeature(DEFAULT_ATTRIBUTE,Common::Misc::utf8stdstring2limastring(str));
  
  // create specific entity from RecognizerMatch using default action
  CreateSpecificEntity createEntity(m_language);
  createEntity(match,analysis);
}

//***********************************************************************
// xerces XML handler
SpecificEntitiesLoader::XMLHandler::XMLHandler(MediaId language, AnalysisContent& analysis, AnalysisGraph* graph):
m_language(language),
m_analysis(analysis),
m_graph(graph),
m_position(0),
m_length(0),
m_type(),
m_string(),
m_currentElement()
{
  LOGINIT("LP::SpecificEntities");
  LDEBUG << "SpecificEntitiesLoader::XMLHandler constructor";
}

SpecificEntitiesLoader::XMLHandler::~XMLHandler()
{
}

bool SpecificEntitiesLoader::XMLHandler::endElement(const QString & namespaceURI, const QString & eltName, const QString & qName)
{
  LIMA_UNUSED(namespaceURI);
  LIMA_UNUSED(qName);
  //LOGINIT("LP::SpecificEntities");
  //LDEBUG << "SpecificEntitiesLoader::XMLHandler end element "  << toString(eltName);
  string name=toString(eltName);
  if (name=="specific_entity") {
    LOGINIT("LP::SpecificEntities");
    LDEBUG << "SpecificEntitiesLoader::XMLHandler add SE "  << m_type << "," << m_position << "," << m_length << "," << m_graph;
    addSpecificEntity(m_analysis, m_graph, m_string, m_type, m_position, m_length);
  }
  // no more current element
  m_currentElement="";
  return true;
}

bool SpecificEntitiesLoader::XMLHandler::characters(const QString& chars)
{
  //LOGINIT("LP::SpecificEntities");
  //LDEBUG << "SpecificEntitiesLoader::XMLHandler characters in "  << m_currentElement;
  if (m_currentElement=="position") {
    std::string pos=toString(chars);
    m_position=atoi(pos.c_str());
  }
  else if (m_currentElement=="length") {
    std::string len=toString(chars);
    m_length=atoi(len.c_str());
  }
  else if (m_currentElement=="type") {
    m_type=toString(chars);
  }
  else if (m_currentElement=="string") {
    m_string=toString(chars);
  }
  return true;
}

bool SpecificEntitiesLoader::XMLHandler::
startElement(const QString & namespaceURI, const QString & eltName, const QString & qName, const QXmlAttributes & attributes)
{
  LIMA_UNUSED(namespaceURI);
  LIMA_UNUSED(qName);
  LIMA_UNUSED(attributes);
  //LOGINIT("LP::SpecificEntities");
  //LDEBUG << "SpecificEntitiesLoader::XMLHandler start element "  << toString(eltName);
  m_currentElement=toString(eltName);

  if (m_currentElement=="specific_entity") { // clear stored values
    m_string="";
    m_type="";
    m_position=0;
    m_length=0;
  }
  return true;
}

bool SpecificEntitiesLoader::XMLHandler::warning(const QXmlParseException& e)
{
  LOGINIT("LP::SpecificEntities");
  LERROR << "Error at file " << toString(e.systemId())
         << ", line " << e.lineNumber()
         << ", char " << e.columnNumber()
         << "  Message: " << toString(e.message());
         return true;
}
bool SpecificEntitiesLoader::XMLHandler::error(const QXmlParseException& e)
{
  LOGINIT("LP::SpecificEntities");
  LERROR << "Fatal error at file " << toString(e.systemId())
         << ", line " << e.lineNumber()
         << ", char " << e.columnNumber()
         << "  Message: " << toString(e.message());
         return false;
}
bool SpecificEntitiesLoader::XMLHandler::fatalError(const QXmlParseException& e)
{
  LOGINIT("LP::SpecificEntities");
  LWARN << "Warning at file " << toString(e.systemId())
        << ", line " << e.lineNumber()
        << ", char " << e.columnNumber()
        << "  Message: " << toString(e.message());
        return false;
}

std::string SpecificEntitiesLoader::XMLHandler::toString(const QString& xercesString)
{
  return Common::Misc::limastring2utf8stdstring(xercesString);
}

} // end namespace
} // end namespace
} // end namespace
