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
/** @brief       Debug logger for dependency graphs
  *
  * @file        SyntacticAnalysisXmlLogger.cpp
  * @author      Gael de Chalendar (Gael.de-Chalendar@cea.fr) 

  *              Copyright (c) 2005 by CEA
  * @date
  *
  */

#include "SyntacticAnalysisXmlLogger.h"

#include "common/MediaProcessors/DumperStream.h"
#include "common/time/traceUtils.h"
#include "common/Data/strwstrtools.h"
#include "common/MediaticData/mediaticData.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/SyntacticAnalysis/SyntacticData.h"
#include "linguisticProcessing/core/TextSegmentation/SegmentationData.h"
#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"

#include <iostream>
#include <fstream>

using namespace boost;
using namespace boost::tuples;
using namespace Lima::Common::Misc;
using namespace Lima::Common::MediaticData;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

typedef color_traits<default_color_type> Color;

namespace Lima
{
namespace LinguisticProcessing
{
namespace SyntacticAnalysis
{

SimpleFactory<MediaProcessUnit,SyntacticAnalysisXmlLogger> syntacticAnalysisXmlLoggerFactory(SYNTACTICANALYSISXMLLOGGER_CLASSID);

SyntacticAnalysisXmlLogger::SyntacticAnalysisXmlLogger():
AbstractTextualAnalysisDumper()
{}


SyntacticAnalysisXmlLogger::~SyntacticAnalysisXmlLogger()
{}

void SyntacticAnalysisXmlLogger::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  AbstractTextualAnalysisDumper::init(unitConfiguration,manager);

  m_language=manager->getInitializationParameters().media;
  m_propertyCodeManager= &(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager());
}

LimaStatusCode SyntacticAnalysisXmlLogger::process(
  AnalysisContent& analysis) const
{
  TimeUtils::updateCurrentTime();

  LinguisticMetaData* metadata=static_cast<LinguisticMetaData*>(analysis.getData("LinguisticMetaData"));
  if (metadata == 0) {
      SALOGINIT;
      LERROR << "no LinguisticMetaData ! abort";
      return MISSING_DATA;
  }

  DumperStream* dstream=initialize(analysis);
  std::ostream& outputStream=dstream->out();
  /*std::ofstream outputStream;
  if (!openLogFile(outputStream,metadata->getMetaData("FileName")))
  {
    SALOGINIT;
    LERROR << "Can't open log file ";
    return CANNOT_OPEN_FILE_ERROR;
  }*/

  SALOGINIT;

  const SyntacticData* syntacticData=static_cast<const SyntacticData*>(analysis.getData("SyntacticData"));
  if (syntacticData==0)
  {
    LERROR << "no SyntacticData ! abort";
    return MISSING_DATA;
  }

  AnalysisGraph* anagraph=static_cast<AnalysisGraph*>(analysis.getData("PosGraph"));
  if (anagraph==0)
  {
    LERROR << "no AnalysisGraph ! abort";
    return MISSING_DATA;
  }
  SegmentationData* sb=static_cast<SegmentationData*>(analysis.getData("SentenceBoundaries"));
  if (sb==0)
  {
    LERROR << "no SentenceBounds ! abort";
    return MISSING_DATA;
  }
//  LinguisticGraph* graph=anagraph->getGraph();

  std::set< std::pair<size_t, size_t> > alreadyDumped;

  outputStream << "<?xml version='1.0' encoding='UTF-8'?>" << std::endl;
  outputStream << "<syntactic_analysis_dump>" << std::endl;


  // ??OME2 SegmentationData::iterator sbItr=sb->begin();
  std::vector<Segment>::iterator sbItr=(sb->getSegments()).begin();

  while (sbItr!=(sb->getSegments()).end())
  {
    LinguisticGraphVertex beginSentence=sbItr->getFirstVertex();
    LinguisticGraphVertex endSentence=sbItr->getLastVertex();

    dumpLimaData(outputStream,
                  beginSentence,
                  endSentence,
                  anagraph,
                  syntacticData);

    sbItr++;
  }

  outputStream << "</syntactic_analysis_dump>" << std::endl;
  delete dstream;
  TimeUtils::logElapsedTime("SyntacticAnalysisXmlLogger");
  return SUCCESS_ID;
}


//***********************************************************************
// main function for outputing the graph
//***********************************************************************
void SyntacticAnalysisXmlLogger::dumpLimaData(std::ostream& os,
                            const LinguisticGraphVertex begin,
                            const LinguisticGraphVertex end,
                            const AnalysisGraph* anagraph,
                            const SyntacticData* syntacticData,
                            const uint64_t offsetBegin) const
{
  std::vector< bool > alreadyDumpedTokens;
  std::map< LinguisticAnalysisStructure::Token*, uint64_t > tokens;

  LinguisticGraph* graph = const_cast< LinguisticGraph* >(anagraph->getGraph());
  // go through the graph, add BoWTokens that are not in complex terms
  DumpGraphVisitor vis(*this, os,end, syntacticData,
      tokens, alreadyDumpedTokens,
      m_language,offsetBegin);

  os << "<sentence>" << std::endl;
  try
  {
      //breadth_first_search(*graph, begin, visitor(vis));
      // I dont know why the direct call to breadth_first_search does
      // not work (bug when trying to go between vertices 0 and 1; because
      // of the 0->1 edge) :
      // initializing the colors and calling breadth_first_visit
      // works (?)
      LinguisticGraphVertexIt i, i_end;
      uint64_t id = 0;
      alreadyDumpedTokens.resize(num_vertices(*graph));
      for (boost::tie(i, i_end) = vertices(*graph); i != i_end; ++i)
      {
            put(boost::vertex_color, *graph, *i, Color::white());
            alreadyDumpedTokens[id] = false;
            tokens[get(vertex_token, *graph, *i)] = id;
            id++;
      }
      boost::breadth_first_visit(*graph, begin, boost::visitor(vis));
  }
  catch (DumpGraphVisitor::EndOfSearch)
  { //do nothing: normal ending
  }
  os << "</sentence>" << std::endl;
}

//***********************************************************************
// output functions
//***********************************************************************


LimaString SyntacticAnalysisXmlLogger::getPosition(const uint64_t position,
                                     const uint64_t offsetBegin)
{
  std::ostringstream pos;
  pos << (offsetBegin+position);
  return Common::Misc::utf8stdstring2limastring(pos.str());
}

void SyntacticAnalysisXmlLogger::outputVertex(const LinguisticGraphVertex v,
                                const LinguisticGraph& graph,
                                const uint64_t offsetBegin,
                                const SyntacticData* syntacticData,
                                std::ostream& xmlStream,
                                std::map< LinguisticAnalysisStructure::Token*, uint64_t >& tokens,
                                std::vector< bool >& alreadyDumpedTokens) const
{
    if (v == syntacticData->iterator()->firstVertex() ||
        v == syntacticData->iterator()->lastVertex())
    {
        xmlStream << "<vertex id=\"" << v << "\" />" << std::endl;
        return;
    }
    Token* token = get(vertex_token, graph, v);

    uint64_t tokenId = (*(tokens.find(token))).second;
//    bool alreadyDumped = alreadyDumpedTokens[tokenId];

    xmlStream << "<vertex id=\"" << v << "\" form=\"" << limastring2utf8stdstring(token->stringForm()) << "\" pos=\"" << getPosition(token->position(),offsetBegin) << "\" ";
    const VertexChainIdProp& chains = get(vertex_chain_id, graph,v);
    xmlStream << " >" << std::endl;
    if (chains.size() > 0)
    {
        xmlStream << "<chains>" << std::endl;
        VertexChainIdProp::const_iterator itChains, itChains_end;
        itChains = chains.begin(); itChains_end = chains.end();
        for (; itChains != itChains_end; itChains++)
        {
          const ChainIdStruct& ids = (*itChains);
          xmlStream << "<chain type=\"";
          if (ids.chainType() == Common::MediaticData::NO_CHAIN_TYPE)
              xmlStream << "0";
          else if (ids.chainType() == Common::MediaticData::NOMINAL)
              xmlStream << "N";
          else
              xmlStream << "V";
          xmlStream << "\" id=\"" << (ids.chainId()) << "\" />" << std::endl;
        }
        xmlStream << "</chains>" << std::endl;
    }

    const DependencyGraph* depGraph = syntacticData->dependencyGraph();
    DependencyGraphVertex depV = syntacticData->depVertexForTokenVertex(v);
    if (out_degree(depV, *depGraph) > 0)
    {

        xmlStream << "<dependents>" << std::endl;
        DependencyGraphOutEdgeIt depIt, depIt_end;
        boost::tie(depIt, depIt_end) = out_edges(depV, *depGraph);
        for (; depIt != depIt_end; depIt++)
        {
            DependencyGraphVertex depTargV = target(*depIt, *depGraph);
            LinguisticGraphVertex targV = syntacticData-> tokenVertexForDepVertex(depTargV);
//             CEdgeDepChainIdPropertyMap chainsMap = get(edge_depchain_id, *depGraph);
            CEdgeDepRelTypePropertyMap relTypeMap = get(edge_deprel_type, *depGraph);
            xmlStream << "<dep v=\"" << targV;
//             xmlStream << "\" c=\"" << chainsMap[*depIt];
            std::string relName=static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getSyntacticRelationName(relTypeMap[*depIt]);
            if (relName.empty()) 
            {
              relName="UNKNOWN";
            }
            xmlStream << "\" t=\"" << relName << "\" />" << std::endl;
        }
        xmlStream << "</dependents>" << std::endl;
    }
    
    const FsaStringsPool& sp=Common::MediaticData::MediaticData::single().stringsPool(m_language);
    
    MorphoSyntacticData* word = get(vertex_data, graph, v);
    word->outputXml(xmlStream,*m_propertyCodeManager,sp);
    xmlStream << "<ref>" << tokenId << "</ref>" << std::endl;
    alreadyDumpedTokens[tokenId] = true;
    xmlStream << "</vertex>" << std::endl;

}

void SyntacticAnalysisXmlLogger::outputEdge(const LinguisticGraphEdge e,
                              const LinguisticGraph& graph,
                              std::ostream& xmlStream) const
{
  xmlStream << "<edge src=\"" << source(e, graph)
          << "\" targ=\"" << target(e, graph) << "\" />" << std::endl;
}


//***********************************************************************
// visitor definition
//***********************************************************************
// examine_vertex to test if we are at end of search
void SyntacticAnalysisXmlLogger::DumpGraphVisitor::examine_vertex(LinguisticGraphVertex v,
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

void SyntacticAnalysisXmlLogger::DumpGraphVisitor::examine_edge(LinguisticGraphEdge e,
               const LinguisticGraph& g)
{
//     DUMPERLOGINIT;
//     LDEBUG << "DumpGraphVisitor: discover_edge " << e;

    m_dumper.outputEdge(e, g, m_os);
}

void SyntacticAnalysisXmlLogger::DumpGraphVisitor::discover_vertex(LinguisticGraphVertex v,
                const LinguisticGraph& graph)
{
    m_dumper.outputVertex(v, graph, m_offsetBegin, m_syntacticData,
                          m_os, m_tokens, m_alreadyDumpedTokens);
}



} // SyntacticAnalysis
} // LinguisticProcessing
} // Lima
