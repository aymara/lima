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
  * @file        SyntacticAnalyzer-disamb.cpp
  * @author      Gael de Chalendar (Gael.de-Chalendar@cea.fr) 

  *              Copyright (c) 2003 by CEA
  * @date        Created on Aug, 31 2004
  * @version     $Id$
  *
  */

#include "SyntacticAnalyzer-disamb.h"

#include "ChainsDisambiguator.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "linguisticProcessing/core/TextSegmentation/SegmentationData.h"
#include "common/time/timeUtilsController.h"

using namespace Lima::Common::MediaticData;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

namespace Lima
{
namespace LinguisticProcessing
{
namespace SyntacticAnalysis
{

static const uint64_t DEFAULT_DEPGRAPHMAXBRANCHINGFACTOR = 30;

SimpleFactory<MediaProcessUnit,SyntacticAnalyzerDisamb> syntacticAnalyzerDisambFactory(SYNTACTICANALYZERDISAMB_CLASSID);

SyntacticAnalyzerDisamb::SyntacticAnalyzerDisamb() :
    m_language()
{}

void SyntacticAnalyzerDisamb::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  SADLOGINIT;
  m_language=manager->getInitializationParameters().media;
  try
  {
    std::string depGraphMaxBranchingFactorS=unitConfiguration.getParamsValueAtKey("depGraphMaxBranchingFactor");
    std::istringstream iss(depGraphMaxBranchingFactorS);
    iss >> m_depGraphMaxBranchingFactor;
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    LWARN << "no parameter 'depGraphMaxBranchingFactor' in syntacticAnalyzerDisamb group for language " << (int) m_language << " ! Using default value << " << DEFAULT_DEPGRAPHMAXBRANCHINGFACTOR << ".";
  }
}

LimaStatusCode SyntacticAnalyzerDisamb::process(
  AnalysisContent& analysis) const
{
  Lima::TimeUtilsController timer("SyntacticAnalysis");
  SADLOGINIT;
  LINFO << "start syntactic analysis - disambiguation";
  // create syntacticData
  AnalysisGraph* anagraph=static_cast<AnalysisGraph*>(analysis.getData("PosGraph"));
  if (anagraph==0)
  {
    LERROR << "no AnalysisGraph ! abort";
    return MISSING_DATA;
  }
  SegmentationData* sb=static_cast<SegmentationData*>(analysis.getData("SentenceBoundaries"));
  if (sb==0)
  {
    LERROR << "no sentence bounds ! abort";
    return MISSING_DATA;
  }
  if (sb->getGraphId() != "PosGraph") {
    LERROR << "SentenceBounds have been computed on " << sb->getGraphId() << " !";
    LERROR << "SyntacticAnalyzer-deps needs SentenceBounds on PosGraph";
    return INVALID_CONFIGURATION;
  }
  
  if (analysis.getData("SyntacticData")==0)
  {
    SyntacticData* syntacticData=new SyntacticData(anagraph,0);
    syntacticData->setupDependencyGraph();
    analysis.setData("SyntacticData",syntacticData);
  }


//  bool l2r = true;
  // ??OME2 for (SegmentationData::const_iterator boundItr=sb->begin();
  //     boundItr!=sb->end();
  for (std::vector<Segment>::const_iterator boundItr=(sb->getSegments()).begin();
       boundItr!=(sb->getSegments()).end();
       boundItr++)
  {
    LinguisticGraphVertex beginSentence=boundItr->getFirstVertex();
    LinguisticGraphVertex endSentence=boundItr->getLastVertex();
    LDEBUG << "analyze sentence from vertex " << beginSentence << " to vertex " << endSentence;
    ChainsDisambiguator cd(dynamic_cast<SyntacticData*>(analysis.getData("SyntacticData")), 
        beginSentence, endSentence, m_language, m_depGraphMaxBranchingFactor);
    cd.initPaths();
    cd.computePaths();
    cd.applyDisambiguisation();
/*    LinguisticGraphVertex current, next;
    current = beginSentence; next = current;
    while (next != endSentence)
    {
      next = nextChainsDisambBreakFrom(current, *anagraph, endSentence);
      LDEBUG << "Disambiguate chains between " << current << " and " << next;
      ChainsDisambiguator cd(dynamic_cast<SyntacticData*>(analysis.getData("SyntacticData")), 
          current, next);
      cd.initPaths();
      cd.computePaths();
      cd.applyDisambiguisation();
      current = next;
    }*/
  }
  
  LINFO << "end syntactic analysis - disambiguation";
  return SUCCESS_ID;
}

/**
  * @brief Finds the next vertex in the agraph after the input vertex that:
  *        1. is on a subgraph with no paralel branch (after the closing of a diamond); <br>
  *        2. is a chain end only (no begin nor part but unigram OK) or is not
  *           in a chain
  *        3. has passed a diamond in the graph
  *        This method should be called <b>after</b> the disambiguation such
  *        that the VertexWordProperty is filled.
  * @param v @b IN <I>LinguisticGraphVertex\&</I>
  *        The vertex from which to start the search
  * @param graph @b IN <I>AnalysisGraph\&</I>
  *        The graph containing @ref v
  * @param nextSentenceBreak @b IN <I>LinguisticGraphVertex\&</I>
  *        The vertex on which to (unconditionaly) stop
  * @return <I>LinguisticGraphVertex</I>
  *         The searched vertex or the last one if there is no such vertex
  */

LinguisticGraphVertex SyntacticAnalyzerDisamb::nextChainsDisambBreakFrom(
      const LinguisticGraphVertex& v,
      const AnalysisGraph& anagraph,
      const LinguisticGraphVertex& nextSentenceBreak) const
{

  const LinguisticGraph& graph = *(anagraph.getGraph());
  
  LinguisticGraphVertex current = v;
  while (boost::out_degree(current, graph) == 1)
  {
//     LDEBUG << "On " << current; 
    if (current == anagraph.lastVertex() || current == nextSentenceBreak) return current;
    current = boost::target(*(boost::out_edges(current, graph).first), graph);
  }
//   LDEBUG << "Entering loop on " << current;
  while(true)
  {
    std::list< LinguisticGraphVertex > fifo;
    std::set< LinguisticGraphVertex > infifo;
    std::set< LinguisticGraphVertex > finished;
    fifo.push_back(current);
    LinguisticGraphInEdgeIt init, init_end;
    boost::tie(init, init_end) = boost::in_edges(current, graph);
    for (; init != init_end; init++)
    {
      finished.insert(source(*init,graph));
    }
    bool first = true;
    while (!fifo.empty())
    {
      current = fifo.front();
      fifo.pop_front();
//       LDEBUG << "On " << current; 
      bool curfinished = true;
      if (finished.find(current) == finished.end())
      {
        LinguisticGraphInEdgeIt init, init_end;
        boost::tie(init, init_end) = boost::in_edges(current, graph);
        while (curfinished && init != init_end)
        {
          if ( finished.find(source(*init,graph)) == finished.end() )
            curfinished = false;
          init++;
        }
        if (curfinished) finished.insert(current);
      }
      if (fifo.empty() && curfinished && !first) break;
      else
      {
        first = false;
        LinguisticGraphOutEdgeIt it, it_end;
        boost::tie(it, it_end) = boost::out_edges(current, graph);
        for (; it != it_end; it++)
        {
          if (infifo.find(target(*it, graph)) == infifo.end())
          {
            fifo.push_back(target(*it, graph));
            infifo.insert(target(*it, graph));
          }
        }
//        fifo.push_back(current);
      }
      if (current == anagraph.lastVertex() || current==nextSentenceBreak)
      {
        if (current != nextSentenceBreak)
        {
          SADLOGINIT;
          LERROR << "In nextChainsBreakFrom: went beyond next sentence break " << nextSentenceBreak;
          LERROR << "   returning graph's last vertex " << current;
        }
//         LDEBUG << "Next chains break is: " << current;
        return current;
      }
    }
//     LDEBUG << "Testing end only on " << current;
    CVertexChainIdPropertyMap chainsMap = boost::get( vertex_chain_id, graph );
    const std::set< Lima::LinguisticProcessing::LinguisticAnalysisStructure::ChainIdStruct >& chains = chainsMap[current];
    if (chains.empty())
      return current;
    std::set< Lima::LinguisticProcessing::LinguisticAnalysisStructure::ChainIdStruct >::const_iterator itc, itc_end;
    itc = chains.begin(); itc_end = chains.end();
    bool endonly  = true;
    while (endonly && itc != itc_end)
    {
      if ( ( (*itc).elemType() == BEGIN ) || ( ( (*itc).elemType() == PART ) ) )
        endonly = false;
      itc++;
    }
    if (endonly) return current;
  }
}


} // closing namespace SyntacticAnalysis
} // closing namespace LinguisticProcessing
} // closing namespace Lima
