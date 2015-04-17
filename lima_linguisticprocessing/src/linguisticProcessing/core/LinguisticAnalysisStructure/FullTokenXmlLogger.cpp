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
/** @brief      logger for xml-formatted linguistic data in graph.
  *
  * @file       MAxmlLogger.cpp
  * @author     Benoit Mathieu <mathieub@zoe.cea.fr>
  *             Copyright (c) 2003 by CEA
  */

#include "FullTokenXmlLogger.h"
#include "common/time/traceUtils.h"
#include "common/Data/strwstrtools.h"
#include "common/MediaticData/mediaticData.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/Token.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AgglutinatedToken.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticData.h"

#include <iostream>
#include <fstream>
#include <ctime>

using namespace std;
using namespace boost;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::XMLConfigurationFiles;

namespace Lima
{
namespace LinguisticProcessing
{
namespace LinguisticAnalysisStructure
{

SimpleFactory<MediaProcessUnit,FullTokenXmlLogger> fullTokenXmlLoggerFactory(FULLTOKENXMLLOGGER_CLASSID);

FullTokenXmlLogger::FullTokenXmlLogger():
AbstractLinguisticLogger(".output.xml")
{}

FullTokenXmlLogger::~FullTokenXmlLogger()
{}

void FullTokenXmlLogger::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  AbstractLinguisticLogger::init(unitConfiguration,manager);
  try
  {
    m_graphId=unitConfiguration.getParamsValueAtKey("graph");
  }
  catch (NoSuchParam& )
  {
    m_graphId=string("AnalysisGraph");
  }
  m_language=manager->getInitializationParameters().media;
  m_propertyCodeManager= &(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager());
}


// Each token of the specified path is
// searched into the specified dictionary.
LimaStatusCode FullTokenXmlLogger::process(
  AnalysisContent& analysis) const
{
  TimeUtils::updateCurrentTime();
  LinguisticMetaData* metadata=static_cast<LinguisticMetaData*>(analysis.getData("LinguisticMetaData"));
  if (metadata == 0)
  {
    DICTIONARYLOGINIT;
    LERROR << "no LinguisticMetaData ! abort";
    return MISSING_DATA;
  }

  AnalysisGraph* tokenList=static_cast<AnalysisGraph*>(analysis.getData(m_graphId));
  std::ofstream fout;
  if (!openLogFile(fout,metadata->getMetaData("FileName"))) {
    MORPHOLOGINIT;
    LERROR << "Error: cannot open log file";
    return CANNOT_OPEN_FILE_ERROR;
  }

  dump(fout, *tokenList);
  fout.close();
  TimeUtils::logElapsedTime("FullTokenXmlLogger");
  return SUCCESS_ID;
}

//**********************************************************************
// define a visitor to go through the graph and output the fulltokens

class DumpXMLVisitor : public default_bfs_visitor
{
  std::ostream& m_ostream;
  std::map<LinguisticGraphVertex,uint64_t> m_depthSource;
  std::map<LinguisticGraphVertex,uint64_t> m_depth;
  const Common::PropertyCode::PropertyCodeManager& m_propertyCodeManager;
  const FsaStringsPool& m_stringsPool;
public:
  DumpXMLVisitor(std::ostream& os,
                 const Common::PropertyCode::PropertyCodeManager& pcm,
                 const FsaStringsPool& stringsPool):
  m_ostream(os),m_depthSource(),m_depth(),m_propertyCodeManager(pcm),m_stringsPool(stringsPool) {}

  void discover_vertex(LinguisticGraphVertex v, const LinguisticGraph& g);
  void examine_edge(LinguisticGraphEdge v, const LinguisticGraph& g);
};

void DumpXMLVisitor::examine_edge(LinguisticGraphEdge e,
                                  const LinguisticGraph& g)
{
  LinguisticGraphVertex vsource=source(e,g);
  LinguisticGraphVertex vtarget=target(e,g);

  // hack : skip edge between first and last vertex
  if (vsource == 0 && vtarget == 1) { return; }

  std::map<LinguisticGraphVertex,uint64_t>::iterator
  d=m_depthSource.find(vsource);
  if (d == m_depthSource.end())
  {
    m_depthSource[vsource]=0;
    m_depth[vtarget]=0;
  }
  else
  {
    m_depthSource[vtarget]=(*d).second;
    (*d).second++;
    m_depth[vtarget]=(*d).second;
  }
}

void DumpXMLVisitor::discover_vertex(LinguisticGraphVertex v,
                                     const LinguisticGraph& g)
{

  m_ostream << "<vertex id=\"" << v << "\">" << std::endl;
  Token* t = get(vertex_token,g,v);
  if (t != 0) 
  {
    m_ostream << "  <token>" << std::endl;
    t->outputXml(m_ostream,m_propertyCodeManager,m_stringsPool);
    m_ostream << "  </token>" << std::endl;
  }
  MorphoSyntacticData* data = get(vertex_data,g,v);
  if (data != 0 )
  {
    data->outputXml(m_ostream,m_propertyCodeManager,m_stringsPool);
  }
  m_ostream << "</vertex>" << std::endl;
}

//**********************************************************************
// dumps memory structure on XML file

void FullTokenXmlLogger::dump(std::ostream& xmlStream,
                              AnalysisGraph& tTokenList) const
{
  //LASLOGINIT;
  xmlStream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
  xmlStream << "<!--generated by MM project on ";
  //    const uint64_t dateLen = strlen("Tue Oct 22 13:42:36 2002");
  time_t aclock;
  time(&aclock);                   /* Get time in seconds */
  std::string str(ctime(&aclock));
  xmlStream << str;
  xmlStream << "-->" << std::endl;
  xmlStream << "<?xml-stylesheet type=\"text/xsl\" href=\"DataStructure.xslt\"?>" << std::endl;
  xmlStream << "<data_structure xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"";
  xmlStream << " xsi:noNamespaceSchemaLocation=\"DataStructure.xsd\">" << std::endl;

  // dump the graph
  const FsaStringsPool& sp=Common::MediaticData::MediaticData::single().stringsPool(m_language);
  DumpXMLVisitor vis(xmlStream,*m_propertyCodeManager,sp);
  breadth_first_search(*(tTokenList.getGraph()),
                       tTokenList.firstVertex(),
                       visitor(vis));

  xmlStream << "</data_structure>" << std::endl;
}

} // LinguisticAnalysisStructure
} // LinguisticProcessing
} // Lima
