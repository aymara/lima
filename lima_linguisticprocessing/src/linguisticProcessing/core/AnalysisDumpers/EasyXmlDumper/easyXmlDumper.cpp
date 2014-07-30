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
/**
  *
  * @file       easyXmlDumper.cpp
  * @author     Damien Nouvel <Damien.Nouvel@cea.fr> 

  *             Copyright (C) 2004 by CEA LIST
  * @author     Gael de Chalendar <Gael.de-Chalendar@cea.fr>
  * @date       Mon Oct 07 2008
  *
  * @brief      dump the content of the analysis graph in Easy XML format
  *
  */

#include "linguisticProcessing/common/BagOfWords/bowDocument.h"

#include "easyXmlDumper.h"
#include "EasyDumper.h"
#include "ConstituantAndRelationExtractor.h"
// #include "linguisticProcessing/core/LinguisticProcessors/HandlerStreamBuf.h"
#include "common/MediaProcessors/HandlerStreamBuf.h"
#include "common/time/traceUtils.h"
#include "common/MediaticData/mediaticData.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "linguisticProcessing/common/BagOfWords/bowToken.h"
#include "linguisticProcessing/common/BagOfWords/bowNamedEntity.h"
#include "linguisticProcessing/common/BagOfWords/bowTerm.h"
// #include "linguisticProcessing/common/BagOfWords/bowFileHeader.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationGraphXmlDumper.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "linguisticProcessing/core/TextSegmentation/SegmentationData.h"
#include "linguisticProcessing/core/SyntacticAnalysis/DependencyGraph.h"
#include "linguisticProcessing/client/AnalysisHandlers/AbstractTextualAnalysisHandler.h"

using namespace std;
using namespace boost;
using namespace boost::tuples;

using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::Common::BagOfWords;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::AnnotationGraphs;

using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
//using namespace Lima::LinguisticProcessing::Automaton;
using namespace Lima::LinguisticProcessing::SyntacticAnalysis;
using namespace Lima::LinguisticProcessing::AnalysisDumpers::EasyXmlDumper;

typedef color_traits<default_color_type> Color;

namespace Lima {
namespace LinguisticProcessing {
namespace AnalysisDumpers {
namespace EasyXmlDumper {

//***********************************************************************
// constructors
//***********************************************************************
SimpleFactory<MediaProcessUnit,EasyXmlDumper> easyXmlDumperFactory(EASYXMLDUMPER_CLASSID);

EasyXmlDumper::EasyXmlDumper() :
MediaProcessUnit(),
m_handler()
{
}

EasyXmlDumper::~EasyXmlDumper()
{
}

void EasyXmlDumper::init(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
                         Manager* manager)
{
  DUMPERLOGINIT;
  LDEBUG << "EasyXmlDumper:: easyXmlDumper init!";
  m_language = manager->getInitializationParameters().media;
  m_propertyCodeManager = &(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager());
  try
  {
    m_typeMapping = unitConfiguration.getMapAtKey("typeMapping");
    m_srcTag = unitConfiguration.getMapAtKey("srcTag");
    m_tgtTag = unitConfiguration.getMapAtKey("tgtTag");
  }
  catch (NoSuchParam& )
  {
    LERROR << "EasyXmlDumper::init: parameter not found (typeMapping, srcTag and tgtTag must be specified)";
    return;
  }
  try
  {
    m_graph = unitConfiguration.getParamsValueAtKey("graph");
  }
  catch (NoSuchParam& )
  {
    LDEBUG << "EasyXmlDumper:: graph parameter not found, using PosGraph";
    m_graph = string("PosGraph");
  }
  try
  {
    m_handler=unitConfiguration.getParamsValueAtKey("handler");
  }
  catch (NoSuchParam& )
  {
    DUMPERLOGINIT;
    LERROR << "EasyXmlDumper::init: Missing parameter handler in EasyXmlDumper configuration";
    throw InvalidConfiguration();
  }
}

LimaStatusCode EasyXmlDumper::process(AnalysisContent& analysis) const
{
  TimeUtils::updateCurrentTime();
  DUMPERLOGINIT;

  LinguisticMetaData* metadata = static_cast<LinguisticMetaData*>(analysis.getData("LinguisticMetaData"));
  if (metadata == 0) {
    LERROR << "EasyXmlDumper::process no LinguisticMetaData ! abort";
      return MISSING_DATA;
  }
  string filename = metadata->getMetaData("FileName");
  LDEBUG << "EasyXmlDumper::process Filename: " << filename;

  LDEBUG << "handler will be: " << m_handler;
//   MediaId langid = static_cast<const  Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(metadata->getMetaData("Lang"))).getMedia();
  AnalysisHandlerContainer* h = static_cast<AnalysisHandlerContainer*>(analysis.getData("AnalysisHandlerContainer"));
  AbstractTextualAnalysisHandler* handler = static_cast<AbstractTextualAnalysisHandler*>(h->getHandler(m_handler));
  if (handler==0)
  {
    LERROR << "EasyXmlDumper::process: handler " << m_handler << " has not been given to the core client";
    return MISSING_DATA;
  }
  
  AnalysisGraph* graph = static_cast<AnalysisGraph*>(analysis.getData(m_graph));
  if (graph == 0)
  {
    graph = new AnalysisGraph(m_graph,m_language,true,true);
    analysis.setData(m_graph,graph);
  }

  SyntacticData* syntacticData = static_cast<SyntacticData*>(analysis.getData("SyntacticData"));
  if (syntacticData == 0)
  {
    syntacticData = new SyntacticAnalysis::SyntacticData(static_cast<AnalysisGraph*>(analysis.getData(m_graph)),0);
    syntacticData->setupDependencyGraph();
    analysis.setData("SyntacticData",syntacticData);
  }

  AnnotationData* annotationData = static_cast< AnnotationData* >(analysis.getData("AnnotationData"));
  if (annotationData == 0)
  {
    annotationData = new AnnotationData();
    if (static_cast<AnalysisGraph*>(analysis.getData("AnalysisGraph")) != 0)
    {
      static_cast<AnalysisGraph*>(analysis.getData("AnalysisGraph"))->populateAnnotationGraph(annotationData, "AnalysisGraph");
    }
    analysis.setData("AnnotationData",annotationData);
  }

  handler->startAnalysis();
  HandlerStreamBuf hsb(handler);
  std::ostream outputStream(&hsb);

  LDEBUG << "EasyXmlDumper:: process before printing heading";
  AnalysisGraph* anaGraph = static_cast<AnalysisGraph*>(analysis.getData("AnalysisGraph"));
  AnalysisGraph* posGraph = static_cast<AnalysisGraph*>(analysis.getData("PosGraph"));
  if (anaGraph != 0 && posGraph != 0)
  {
    LDEBUG << "EasyXmlDumper:: begin of posgraph";
    std::vector< bool > alreadyDumpedTokens;
    std::map< LinguisticAnalysisStructure::Token*, uint64_t > fullTokens;
    LinguisticGraphVertexIt i, i_end;
    uint64_t id = 0;
    alreadyDumpedTokens.resize(num_vertices(*posGraph->getGraph()));
    for (boost::tie(i, i_end) = vertices(*posGraph->getGraph()); i != i_end; ++i)
    {
      LDEBUG << "EasyXmlDumper:: examine posgraph for " << id;
      alreadyDumpedTokens[id] = false;
      fullTokens[get(vertex_token, *posGraph->getGraph(), *i)] = id;
      id++;
    }
    /* No need for sentence boundaries in Easy input
    LinguisticGraphVertex sentenceBegin = sb->getStartVertex();
    SegmentationData::iterator sbItr = sb->begin();
    LinguisticGraphVertex sentenceBegin = sb->getStartVertex();
    SegmentationData::iterator sbItr = sb->begin();
    */
    LinguisticGraphVertex sentenceBegin = posGraph->firstVertex();
    LinguisticGraphVertex sentenceEnd = posGraph->lastVertex();
    string sentIdPrefix;
    try {
      sentIdPrefix = metadata->getMetaData("docid");
      LDEBUG << "EasyXmlDumper:: retrieve sentence id " << sentIdPrefix;
    }catch (LinguisticProcessingException& ) {
      sentIdPrefix = "";
    }
    if(sentIdPrefix.length() <= 0)
      sentIdPrefix = "E";
    /* No need for sentence boundaries in Easy input
    while (sbItr != sb->end())
    {
      LinguisticGraphVertex sentenceEnd = *sbItr;
    */
    LDEBUG << "EasyXmlDumper:: inside posgraph while ";
    dumpLimaData(outputStream,
                  sentenceBegin,
                  sentenceEnd,
                  *anaGraph,
                  *posGraph,
                  *annotationData,
                  *syntacticData,
                  "PosGraph",
                  alreadyDumpedTokens,
                  fullTokens,
                  sentIdPrefix);
    /* No need for sentence boundaries in Easy input
      sentenceBegin = sentenceEnd;
      sbItr++;
    }
    */
    LDEBUG << "EasyXmlDumper:: end of posgraph";
  }

  return SUCCESS_ID;
}


//***********************************************************************
// main function for outputing the graph
//***********************************************************************
void EasyXmlDumper::dumpLimaData(std::ostream& os,
                                  const LinguisticGraphVertex& begin,
                                  const LinguisticGraphVertex& end,
                                  const AnalysisGraph& anaGraph,
                                  const AnalysisGraph& posGraph,
                                  const AnnotationData& annotationData,
                                  const SyntacticData& syntacticData,
                                  const std::string& graphId,
                                  std::vector< bool >& alreadyDumpedTokens,
                                  std::map< LinguisticAnalysisStructure::Token*, uint64_t >& fullTokens,
                                  std::string sentIdPrefix) const
{

  DUMPERLOGINIT;
  LDEBUG << "EasyXmlDumper:: dumpLimaData parameters: ";
  LDEBUG << "EasyXmlDumper::   begin = " << begin;
  LDEBUG << "EasyXmlDumper::   end = " << end;
  LDEBUG << "EasyXmlDumper::   posgraph first vertex = " << posGraph.firstVertex();
  LDEBUG << "EasyXmlDumper::   posgraph last vertex = " << posGraph.lastVertex();
  LDEBUG << "EasyXmlDumper::   graphId = " << graphId;
  LDEBUG << "EasyXmlDumper::   sentIdPrefix = " << sentIdPrefix;

  // just in case we want to check alreadt dumped tokens' array
  for (uint64_t i = 0; i<alreadyDumpedTokens.size(); i++)
  {
    if (alreadyDumpedTokens[i])
    {
      LDEBUG << "EasyXmlDumper:: already_dumped_tokens[" << i << "] =" << alreadyDumpedTokens[i];
    }
  }

  std::string sentIdStr = sentIdPrefix;
  if(find(m_sentIds.begin(), m_sentIds.end(), sentIdStr) != m_sentIds.end() || sentIdStr == "E" )
  {
    uint64_t sentIdsuffix = 0;
    do{
      sentIdsuffix++;
      std::stringstream sentIdStream;
      sentIdStream << sentIdPrefix << sentIdsuffix;
      sentIdStr = sentIdStream.str();
    }while(find(m_sentIds.begin(), m_sentIds.end(), sentIdStr) != m_sentIds.end());
  }

  LDEBUG << "EasyXmlDumper:: searching and extracting vertices and relations";
  LinguisticGraph* anaGraphL = const_cast<LinguisticGraph*>(anaGraph.getGraph());
  LinguisticGraph* posGraphL = const_cast<LinguisticGraph*>(posGraph.getGraph());
  ConstituantAndRelationExtractor care(m_propertyCodeManager);
  care.visitBoostGraph(begin,
                       end,
                       *anaGraphL,
                       *posGraphL,
                       annotationData,
                       syntacticData,
                       fullTokens,
                       alreadyDumpedTokens,
                       m_language);

  LDEBUG << "EasyXmlDumper:: all found vertices and relations extracted";
  care.replaceSEWithCompounds();
  care.constructionDesRelationsEntrantes();
  care.splitCompoundTenses();
  care.constructionDesGroupes();
  care.addLastFormsInGroups();

  EasyDumper ed(care, m_typeMapping, m_srcTag, m_tgtTag, sentIdStr);
  std::stringstream sentEasyStream;
  ed.dump(sentEasyStream);
  if(sentEasyStream.str().length() > 0)
  {
    // Makes object mutable for adding sentence ID
    EasyXmlDumper* self = const_cast<EasyXmlDumper*>(this);
    self->m_sentIds.push_back(sentIdStr);
    os << "<E id=\"" << sentIdStr << "\">" << std::endl;
    os << sentEasyStream.str();
    os << "</E>" << std::endl;
  }

}

} // end namespace EasyXmlDumper
} // end namespace AnalysisDumpers
} // end namespace LinguisticProcessings
} // end namespace Lima
