/*
    Copyright 2002-2020 CEA LIST

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
  * @file       fullXmlDumper.cpp
  * @author     Gael de Chalendar <Gael.de-Chalendar@cea.fr>

  *             Copyright (C) 2004 by CEA LIST
  * @author     Besancon Romaric (besanconr@zoe.cea.fr)
  * @date       Mon Apr  26 2004
  *
  * @brief      dump the full content of the analysis graph in XML format
  *
  * @change_log:
  * 12/2007:   JGF: changed breadth_first_visit instead of breadth_first_search
  * 03/2007:    JGF: correct duplicate vertex error in ::outputVertex
  */

#include "linguisticProcessing/common/BagOfWords/bowDocument.h"

#include "fullXmlDumper.h"
// #include "linguisticProcessing/core/LinguisticProcessors/HandlerStreamBuf.h"
#include "common/MediaProcessors/HandlerStreamBuf.h"

#include "common/Data/strwstrtools.h"
#include "common/time/traceUtils.h"
#include "common/MediaticData/mediaticData.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "linguisticProcessing/common/BagOfWords/bowToken.h"
#include "linguisticProcessing/common/BagOfWords/bowNamedEntity.h"
#include "linguisticProcessing/common/BagOfWords/bowTerm.h"
// #include "linguisticProcessing/common/BagOfWords/bowFileHeader.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
//#include "linguisticProcessing/common/annotationGraph/AnnotationGraphXmlDumper.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "linguisticProcessing/core/TextSegmentation/SegmentationData.h"
#include "linguisticProcessing/core/SyntacticAnalysis/DependencyGraph.h"
#include <fstream>
#include "linguisticProcessing/client/AnalysisHandlers/AbstractTextualAnalysisHandler.h"

using namespace std;
//using namespace boost;
using namespace boost::tuples;

using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::BagOfWords;

using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
//using namespace Lima::LinguisticProcessing::Automaton;
using namespace Lima::LinguisticProcessing::SyntacticAnalysis;

typedef boost::color_traits<boost::default_color_type> Color;

namespace Lima {
namespace LinguisticProcessing {
namespace AnalysisDumpers {


//***********************************************************************
// constructors
//***********************************************************************
SimpleFactory<MediaProcessUnit,FullXmlDumper> fullXmlDumperFactory(FULLXMLDUMPER_CLASSID);

FullXmlDumper::FullXmlDumper()
: MediaProcessUnit(),
      m_annotXmlDumperPtr(0),
      m_dumpFullTokens(true),
      m_handler()
{
}

FullXmlDumper::~FullXmlDumper()
{
}

void FullXmlDumper::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  DUMPERLOGINIT;
  LDEBUG << "fullXmlDumper init!";
  m_language=manager->getInitializationParameters().media;
  m_propertyCodeManager= &(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager());
  try
  {
    m_dumpFullTokens = (unitConfiguration.getParamsValueAtKey("dumpTokens") == "true");
  }
  catch (NoSuchParam& )
  {
    LWARN << "dumpTokens parameter not found, using default: "
        << (m_dumpFullTokens?"true":"false");
  }
  try
  {
    m_graph=unitConfiguration.getParamsValueAtKey("graph");
  }
  catch (NoSuchParam& )
  {
    m_graph=string("PosGraph");
  }
  try
  {
    m_handler=unitConfiguration.getParamsValueAtKey("handler");
  }
  catch (NoSuchParam& )
  {
    DUMPERLOGINIT;
    LERROR << "FullXmlDumper::init: Missing parameter handler in FullXmlDumper configuration";
    throw InvalidConfiguration();
  }
  m_annotXmlDumperPtr = new AnnotationGraphXmlDumper(m_language,m_handler);

}

LimaStatusCode FullXmlDumper::process(AnalysisContent& analysis) const
{
  TimeUtils::updateCurrentTime();
  DUMPERLOGINIT;

  LinguisticMetaData* metadata=static_cast<LinguisticMetaData*>(analysis.getData("LinguisticMetaData"));
  if (metadata == 0) {
      LERROR << "FullXmlDumper::process:  no LinguisticMetaData ! abort";
      return MISSING_DATA;
  }
  LDEBUG << "handler will be: " << m_handler;
//   MediaId langid = static_cast<const  Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(metadata->getMetaData("Lang"))).getMedia();
  AnalysisHandlerContainer* h = static_cast<AnalysisHandlerContainer*>(analysis.getData("AnalysisHandlerContainer"));
  AbstractTextualAnalysisHandler* handler = static_cast<AbstractTextualAnalysisHandler*>(h->getHandler(m_handler));
  if (handler==0)
  {
    LERROR << "FullXmlDumper::process: handler " << m_handler << " has not been given to the core client";
    return MISSING_DATA;
  }

  AnalysisGraph* graph=static_cast<AnalysisGraph*>(analysis.getData(m_graph));
  if (graph==0) {
    graph=new AnalysisGraph(m_graph,m_language,true,true);
    analysis.setData(m_graph,graph);
  }

  SyntacticData* syntacticData=static_cast<SyntacticData*>(analysis.getData("SyntacticData"));
  if (syntacticData==0)
  {
    syntacticData=new SyntacticAnalysis::SyntacticData(static_cast<AnalysisGraph*>(analysis.getData(m_graph)),0);
    syntacticData->setupDependencyGraph();
    analysis.setData("SyntacticData",syntacticData);
  }

/*  AnalysisGraph* anagraph=static_cast<AnalysisGraph*>(analysis.getData(m_graph));
  if (anagraph==0)
  {
    LERROR << "FullXmlDumper::process: no AnalysisGraph ! abort";
    return MISSING_DATA;
  }*/


  // Are sentences bounds right?
  SegmentationData* sb=static_cast<SegmentationData*>(analysis.getData("SentenceBoundaries"));
  if (sb==0)
  {
    LDEBUG << "FullXmlDumper: SentenceBounds not found "<< analysis.getData("SentenceBoundaries");
    sb=new SegmentationData(m_graph);
    analysis.setData("SentenceBoundaries",sb);
  }
  AnnotationData* annotationData = static_cast< AnnotationData* >(analysis.getData("AnnotationData"));
  if (annotationData==0)
  {
    annotationData=new AnnotationData();
    if (static_cast<AnalysisGraph*>(analysis.getData("AnalysisGraph")) != 0)
    {
      static_cast<AnalysisGraph*>(analysis.getData("AnalysisGraph"))->populateAnnotationGraph(annotationData, "AnalysisGraph");
    }
    analysis.setData("AnnotationData",annotationData);
  }

  handler->startAnalysis();
  HandlerStreamBuf hsb(handler);
  std::ostream outputStream(&hsb);
  std::set< std::pair<size_t, size_t> > alreadyDumped;

  LDEBUG << "FullXmlDumper::process before printing heading";

  outputStream << "<?xml version='1.0' encoding='UTF-8'?>" << std::endl;
  outputStream << "<!DOCTYPE lima_analysis_dump SYSTEM \"lima-xml-output.dtd\">" << std::endl;
  outputStream << "<lima_analysis_dump>" << std::endl;

  // ??OME2 SegmentationData::iterator sbItr=sb->begin();
  std::vector<Segment>::iterator sbItr=(sb->getSegments()).begin();

  AnalysisGraph* anagraph=static_cast<AnalysisGraph*>(analysis.getData("AnalysisGraph"));
  if (anagraph != 0)
  {
    LDEBUG << "FullXmlDumper:: inside if anagraph: ";
    std::vector< bool > alreadyDumpedTokens;
    std::map< LinguisticAnalysisStructure::Token*, uint64_t > fullTokens;

    LinguisticGraphVertexIt i, i_end;
    uint64_t id = 0;
    alreadyDumpedTokens.resize(num_vertices(*anagraph->getGraph()));
    LDEBUG << "FullXmlDumper: begin of anagraph for"<< id;
    for (boost::tie(i, i_end) = vertices(*anagraph->getGraph()); i != i_end; ++i)
    {
      alreadyDumpedTokens[id] = false;
      fullTokens[get(vertex_token, *anagraph->getGraph(), *i)] = id;
      id++;
      LDEBUG << " a"<< id;
    }
    LDEBUG << "FullXmlDumper: end of anagraph for"<< id;
    dumpLimaData(outputStream,
                anagraph->firstVertex(),
                anagraph->lastVertex(),
                anagraph,
                syntacticData,
                "AnalysisGraph",
                false, alreadyDumpedTokens, fullTokens);
  }
  AnalysisGraph* posgraph = static_cast<AnalysisGraph*>(analysis.getData("PosGraph"));
  if (posgraph != 0)
  {
    LDEBUG << "FullXmlDumper:: inside if posgraph: ";
    std::vector< bool > alreadyDumpedTokens;
    std::map< LinguisticAnalysisStructure::Token*, uint64_t > fullTokens;
    LinguisticGraphVertexIt i, i_end;
    uint64_t id = 0;
    alreadyDumpedTokens.resize(num_vertices(*posgraph->getGraph()));
    LDEBUG << "FullXmlDumper: begin of posgraph for"<< id;
    for (boost::tie(i, i_end) = vertices(*posgraph->getGraph()); i != i_end; ++i)
    {
      alreadyDumpedTokens[id] = false;
      fullTokens[get(vertex_token, *posgraph->getGraph(), *i)] = id;
      id++;
      LDEBUG << " p"<< id;
    }
    // ??OME2 LDEBUG << "FullXmlDumper: end of posgraph for: sbItr "<< sbItr->getLastVertex() << " sb-end() " << sb->end()->getLastVertex();
    LDEBUG << "FullXmlDumper: end of posgraph for: sbItr "<< sbItr->getLastVertex() << " sb-end() " << (sb->getSegments()).end()->getLastVertex();
    outputStream << "<PosGraph>" << std::endl;
    // ??OME2 while (sbItr!=sb->end())
    while (sbItr!=(sb->getSegments()).end())
    {

      LDEBUG << "FullXmlDumper: inside posgraph while ";
      LinguisticGraphVertex sentenceBegin=sbItr->getFirstVertex();
      LinguisticGraphVertex sentenceEnd=sbItr->getLastVertex();
      dumpLimaData(outputStream,
                    sentenceBegin,
                    sentenceEnd,
                    posgraph,
                    syntacticData,
                    "PosGraph",
                    true, alreadyDumpedTokens, fullTokens);

      sbItr++;

  //    compoundsItr++;
    }

    outputStream << "  <DetachedVertices>" << std::endl;
      // dump non non-connex nodes
    LDEBUG << "FullXmlDumper:: dump non-connex nodes and related edges: ";
    id = 0;
    for (boost::tie(i, i_end) = vertices(*posgraph->getGraph()); i != i_end; ++i)
    {
      LDEBUG << " adding non-connex vertex "<< id;
      outputVertex(*i,
                   *posgraph->getGraph(),
                   syntacticData,
                   outputStream,
                   fullTokens,
                   alreadyDumpedTokens,
                   "PosGraph");
      id++;
    }
    outputStream << "  </DetachedVertices>" << std::endl;

    outputStream << "</PosGraph>" << std::endl;

  }

  // dump annotation graph
  m_annotXmlDumperPtr->dump( analysis );

  // annotXmlDumper.dump(outputStream, &(annotationData->getGraph()), *annotationData);

  outputStream << "</lima_analysis_dump>" << std::endl;
  handler->endAnalysis();

  TimeUtils::logElapsedTime("FullXmlDumper");
  return SUCCESS_ID;
}


//***********************************************************************
// main function for outputing the graph
//***********************************************************************
void FullXmlDumper::dumpLimaData(std::ostream& os,
                            const LinguisticGraphVertex begin,
                            const LinguisticGraphVertex end,
                            const AnalysisGraph* anagraph,
                            const SyntacticData* syntacticData,
                            const std::string& graphId,
                            bool bySentence,
                            std::vector< bool >& alreadyDumpedTokens,
                            std::map< LinguisticAnalysisStructure::Token*, uint64_t >& fullTokens) const
{

  DUMPERLOGINIT;

  LDEBUG << "FullXmlDumper::dumpLimaData parameters: ";
  LDEBUG << "begin = "<< begin;
  LDEBUG << "end = " << end ;
  LDEBUG << "anagraph fist vertex= " << anagraph->firstVertex() ;
  LDEBUG << "anagraph last vertex= " << anagraph->lastVertex() ;
  LDEBUG << "graphId= " << graphId ;
  LDEBUG << "bySentence= " << bySentence ;
  // just in case we want to check alreadt dumped tokens' array
  for (uint64_t i=0; i<alreadyDumpedTokens.size(); i++) {
    if (alreadyDumpedTokens[i]) {LDEBUG << "already_dumped_tokens[" << i << "]=" << alreadyDumpedTokens[i];}
  }

  LinguisticGraph* graph = const_cast< LinguisticGraph* >(anagraph->getGraph());
  // go through the graph, add BoWTokens that are not in complex terms
  LDEBUG << "after LinguisticGraph" ;
  DumpGraphVisitor vis(*this, os, end, syntacticData,
      fullTokens, alreadyDumpedTokens,
      m_language, graphId);
  LDEBUG << "after DumpGraphVisitor" ;
  if (bySentence)
  {
    os << "  <sentence>" << std::endl;
  }
  else
  {
    os << "<"<<graphId<<">" << std::endl;
  }
  try
  {
      //breadth_first_search(*graph, begin, visitor(vis));
      // I dont know why the direct call to breadth_first_search does
      // not work (bug when trying to go between vertices 0 and 1; because
      // of the 0->1 edge) :
      // initializing the colors and calling breadth_first_visit
      // works (?)
      LDEBUG << "inside dumpLimaData's try" ;
      LinguisticGraphVertexIt i, i_end;
      for (boost::tie(i, i_end) = vertices(*graph); i != i_end; ++i)
      {
        put(boost::vertex_color, *graph, *i, Color::white());
      }

      boost::breadth_first_visit(*graph, begin, boost::visitor(vis));
      LDEBUG << "breadth_first_visit arguments: ";
//       boost::depth_first_search(*graph, boost::visitor(vis));
  }
  catch (DumpGraphVisitor::EndOfSearch&)
  { //do nothing: normal ending
  }
  if (bySentence)
  {
    LDEBUG << "BY SENTENCE ";
    os << "  </sentence>" << std::endl;
  }
  else
  {
    os << "</"<<graphId<<">" << std::endl;
  }
}

//***********************************************************************
// output functions
//***********************************************************************


LimaString FullXmlDumper::getPosition(const uint64_t position) const
{
  std::ostringstream pos;
  pos << position;
  return Common::Misc::utf8stdstring2limastring(pos.str());
}

void FullXmlDumper::outputVertex(const LinguisticGraphVertex v,
                                const LinguisticGraph& graph,
                                const SyntacticData* syntacticData,
                                std::ostream& xmlStream,
                                std::map< LinguisticAnalysisStructure::Token*, uint64_t >& fullTokens,
                                std::vector< bool >& alreadyDumpedTokens,
                                const std::string& graphId) const
{
 FsaStringsPool& sp=Common::MediaticData::MediaticData::changeable().stringsPool(m_language);
 Token* token = get(vertex_token, graph, v);
 uint64_t tokenId = (*(fullTokens.find(token))).second;
 bool alreadyDumped = alreadyDumpedTokens[tokenId];

// without this condition, there's duplicate vertex in the XML output!!!
 if (!alreadyDumped)
 {
  if (v == syntacticData->iterator()->firstVertex() ||
      v == syntacticData->iterator()->lastVertex())
  {
      xmlStream << "  <vertex id=\"_" << v << "\" />" << std::endl;
      return;
  }
  if (token == 0)
  {
    DUMPERLOGINIT;
    LWARN << "No token (vertex_token) for vertex "  << v;
    xmlStream << "  <vertex id=\"_" << v << "\" />" << std::endl;
    return;
  }

  xmlStream << "  <vertex id=\"_" << v << "\"";
// debugging to take out JGF
//   DUMPERLOGINIT;
  const VertexChainIdProp& chains = get(vertex_chain_id, graph,v);
  if (chains.size() > 0)
  {
      xmlStream << " chains=\"";
      VertexChainIdProp::const_iterator itChains, itChains_end;
      itChains = chains.begin(); itChains_end = chains.end();
      xmlStream << (*itChains); itChains++;
      for (; itChains != itChains_end; itChains++)
      {
          xmlStream << "," << (*itChains);
      }
      xmlStream << "\"";
  }
  xmlStream << " >" << std::endl;

  if (graphId != "AnalysisGraph")
  {
    const DependencyGraph* depGraph = syntacticData->dependencyGraph();
    DependencyGraphVertex depV = syntacticData->depVertexForTokenVertex(v);
    if (out_degree(depV, *depGraph) > 0)
    {
        DUMPERLOGINIT;
        xmlStream << "      <deps>" << std::endl;
        DependencyGraphOutEdgeIt depIt, depIt_end;
        boost::tie(depIt, depIt_end) = out_edges(depV, *depGraph);
        for (; depIt != depIt_end; depIt++)
        {
            DependencyGraphVertex depTargV = target(*depIt, *depGraph);
            LinguisticGraphVertex targV = syntacticData-> tokenVertexForDepVertex(depTargV);
            CEdgeDepRelTypePropertyMap relTypeMap = get(edge_deprel_type, *depGraph);
            // This line changed after MODEX
            std::string relationName = static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getSyntacticRelationName(relTypeMap[*depIt]);
            LDEBUG << "dep " << v << " -> " << targV << " ( " << relationName << " )";
//          CEdgeDepChainIdPropertyMap chainsMap = get(edge_depchain_id, *depGraph);
            xmlStream << "        <dep v=\"_" << targV;
//               xmlStream << "\" c=\"" << chainsMap[*depIt];
            xmlStream << "\" t=\"" << relationName << "\" />" << std::endl;
        }
        xmlStream << "      </deps>" << std::endl;
    }
  }

  MorphoSyntacticData* data = get(vertex_data, graph, v);
  if (data == 0)
  {
    DUMPERLOGINIT;
    LWARN << "No morphosyntactic (vertex_data) data for vertex "  << v;
  }
  else
  {
    data->outputXml(xmlStream, *m_propertyCodeManager,sp);
  }
  if (m_dumpFullTokens && !alreadyDumped)
  {
    token->outputXml(xmlStream, *m_propertyCodeManager,sp);
  }
  else
  {
    xmlStream << "    <ref>" << tokenId << "</ref>" << std::endl;
  }
  alreadyDumpedTokens[tokenId] = true;
  xmlStream << "  </vertex>" << std::endl;
 }
}

void FullXmlDumper::outputEdge(const LinguisticGraphEdge e,
                              const LinguisticGraph& graph,
                              std::ostream& xmlStream) const
{
  xmlStream << "    <edge src=\"" << source(e, graph)
            << "\" targ=\"" << target(e, graph) << "\" />" << std::endl;
}


//***********************************************************************
// visitor definition
//***********************************************************************
// examine_vertex to test if we are at end of search
void FullXmlDumper::DumpGraphVisitor::examine_vertex(LinguisticGraphVertex v,
               const LinguisticGraph& g)
{
//   DUMPERLOGINIT;
//   LDEBUG << "DumpGraphVisitor: examine_vertex " << v;

  if (v == m_lastVertex)
  {
    bool finished = true;
    LinguisticGraphInEdgeIt it, it_end;
    boost::tie(it, it_end) = in_edges(v, g);
    while (finished && (it != it_end))
    {
      if (get(boost::vertex_color, g,source(*it, g)) != Color::black())
      {
        finished = false;
      }
      it++;
    }
    if (finished)
    {
//       LDEBUG << "DumpGraphVisitor: finished";
      throw EndOfSearch();
    }
  }
}

void FullXmlDumper::DumpGraphVisitor::examine_edge(LinguisticGraphEdge e,
               const LinguisticGraph& g)
{
//     DUMPERLOGINIT;
//     LDEBUG << "DumpGraphVisitor: discover_edge " << e;

    m_dumper.outputEdge(e, g, m_os);
}

void FullXmlDumper::DumpGraphVisitor::discover_vertex(LinguisticGraphVertex v,
                const LinguisticGraph& graph)
{
    m_dumper.outputVertex(v, graph, m_syntacticData,
                          m_os, m_fullTokens, m_alreadyDumpedFullTokens, m_graphId);
}

} // end namespace AnalysisDumpers
} // end namespace LinguisticProcessings
} // end namespace Lima
