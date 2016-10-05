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
  * @file        SyntacticAnalyzer-chains.cpp
  * @author      Gael de Chalendar (Gael.de-Chalendar@cea.fr) 

  *              Copyright (c) 2003 by CEA
  * @date        Created on Aug, 31 2004
  * @version     $Id$
  *
  */

#include "SyntacticAnalyzer-chains.h"
#include "SyntagmaticMatrix.h"

#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/TextSegmentation/SegmentationData.h"
#include "common/time/timeUtilsController.h"
#include "common/LimaCommon.h"

#undef min
#undef max

using namespace std;
//using namespace boost;
using namespace Lima::Common::MediaticData;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

namespace Lima
{
namespace LinguisticProcessing
{
namespace SyntacticAnalysis
{

static const uint64_t DEFAULT_MAXCHAINSNBBYVERTEX = 30;
static const uint64_t DEFAULT_MAXCHAINLENGTH = 200;

SimpleFactory<MediaProcessUnit,SyntacticAnalyzerChains> syntacticAnalyzerChainsFactory(SYNTACTICANALYZERCHAINS_CLASSID);

SyntacticAnalyzerChains::SyntacticAnalyzerChains() :
    m_language(),
    m_chainMatrix(0),
    m_maxChainsNbByVertex(std::numeric_limits<uint64_t>::max())
{}

void SyntacticAnalyzerChains::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  SACLOGINIT;
  m_language=manager->getInitializationParameters().media;
  m_macroAccessor=&(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyAccessor("MACRO"));
  try
  {
    std::string chainMatrixId=unitConfiguration.getParamsValueAtKey("chainMatrix");
    m_chainMatrix=static_cast<SyntagmDefStruct*>(LinguisticResources::single().getResource(m_language,chainMatrixId));
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    LERROR << "no parameter 'chainMatrix' in SyntacticAnalyzerChains group for language " << (int) m_language << " !";
    throw InvalidConfiguration();
  }
  try
  {
    std::string maxChainsNbByVertexS=unitConfiguration.getParamsValueAtKey("maxChainsNbByVertex");
    std::istringstream iss(maxChainsNbByVertexS);
    iss >> m_maxChainsNbByVertex;
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    LWARN << "no parameter 'maxChainsNbByVertex' in SyntacticAnalyzerChains group for language " << (int) m_language << " ! Using default: "<<DEFAULT_MAXCHAINSNBBYVERTEX<<".";
    m_maxChainsNbByVertex = DEFAULT_MAXCHAINSNBBYVERTEX;
  }
  try
  {
    std::string maxChainLengthS=unitConfiguration.getParamsValueAtKey("maxChainLength");
    std::istringstream iss(maxChainLengthS);
    iss >> m_maxChainLength;
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    LWARN << "no parameter 'maxChainLength' in SyntacticAnalyzerChains group for language " << (int) m_language << " ! Using default: "<<DEFAULT_MAXCHAINLENGTH<<".";
    m_maxChainLength = DEFAULT_MAXCHAINLENGTH;
  }
  try
  {
    std::string id=unitConfiguration.getParamsValueAtKey("ponctuCategory");
    m_ponctuCategory=static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyManager("MACRO").getPropertyValue(id);
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    LWARN << "No ponctu macrocategory defined ! use category PONCTU";
    m_ponctuCategory=static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyManager("MACRO").getPropertyValue("PONCTU");
  }

}

LimaStatusCode SyntacticAnalyzerChains::process(
  AnalysisContent& analysis) const
{
#ifdef ANTINNO_SPECIFIC
  auto const& stopAnalyze = analysis.stopAnalyze();
#endif
  Lima::TimeUtilsController timer("SyntacticAnalysis");
  SACLOGINIT;
  LINFO << "start syntactic analysis - chains";
  // create syntacticData
  AnalysisGraph* anagraph=static_cast<AnalysisGraph*>(analysis.getData("PosGraph"));
  if (anagraph==0)
  {
    LERROR << "no PosGraph ! abort";
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

  SyntacticData* syntacticData=dynamic_cast<SyntacticData*>(analysis.getData("SyntacticData"));
  if (syntacticData==0)
  {
    syntacticData=new SyntacticData(anagraph,m_chainMatrix);
    analysis.setData("SyntacticData",syntacticData);
  }
  else if (syntacticData->matrices() == 0)
  {
    syntacticData->matrices(m_chainMatrix);
  }
  syntacticData->setupDependencyGraph();

  uint64_t chainId = m_firstChainId;
  std::list<LinguisticCode> ponctuMacroFilter;
  ponctuMacroFilter.push_back(m_ponctuCategory);

//  bool l2r = true;
  // ??OME2 for (SegmentationData::const_iterator boundItr=sb->begin();
  //     boundItr!=sb->end();
  for (std::vector<Segment>::const_iterator boundItr=(sb->getSegments()).begin();
       boundItr!=(sb->getSegments()).end();
       boundItr++)
  {
    LinguisticGraphVertex beginSentence=boundItr->getFirstVertex();
    LinguisticGraphVertex endSentence=boundItr->getLastVertex();
//     LDEBUG << "analyze sentence from vertex " << beginSentence << " to vertex " << endSentence;
    LinguisticGraphVertex current, next;
    current = beginSentence; next = current;

#ifdef ANTINNO_SPECIFIC
    if (stopAnalyze)
	  {
		  LERROR << "Analyze too long. Stopped in SyntacticAnalyzerChains";
		  return TIME_OVERFLOW;
	  }
#endif
    while (next != endSentence)
    {
#ifdef ANTINNO_SPECIFIC
		  if (stopAnalyze)
		  {
			  LERROR << "Analyze too long. Stopped in SyntacticAnalyzerChains";
			  return TIME_OVERFLOW;
		  }
#endif
//       LDEBUG << "nextChainsBreak";
      next = anagraph->nextMainPathVertex(current,*m_macroAccessor,ponctuMacroFilter,endSentence);

#ifdef ANTINNO_SPECIFIC
		  if (stopAnalyze)
		  {
			  LERROR << "Analyze too long. Stopped in SyntacticAnalyzerChains";
			  return TIME_OVERFLOW;
		  }
#endif
//       LDEBUG << "analyze chain from " << current << " to " << next;
//       LDEBUG << "identify chains";
      identifyChains(syntacticData,current,next,chainId
#ifdef ANTINNO_SPECIFIC
        , stopAnalyze
#endif
        );
#ifdef ANTINNO_SPECIFIC
		  if (stopAnalyze)
		  {
			  LERROR << "Analyze too long. Stopped in SyntacticAnalyzerChains";
			  return TIME_OVERFLOW;
		  }
#endif
      current = next;
    }
    beginSentence=endSentence;
  }

  LINFO << "end syntactic analysis - chains";
  return SUCCESS_ID;
}


void SyntacticAnalyzerChains::identifyChains(SyntacticData* data,
                                       const LinguisticGraphVertex& start,
                                       const LinguisticGraphVertex& stop,
                                       uint64_t& startChainId,
#ifdef ANTINNO_SPECIFIC
                                       StopAnalyze const& stopAnalyze
#endif
                                       ) const
{
//   SACLOGINIT;
//   LDEBUG << "Searching chains from/to (morph): " << start << "/" << stop;
  if (start == stop)
    return;
  VertexChainIdPropertyMap vertexChainIdMap = get( vertex_chain_id, *(data->graph()) );
  std::set< std::string > alreadyReported;
  LinguisticGraphVertex first = data->iterator()-> firstVertex();
  LinguisticGraphVertex last = data->iterator()-> lastVertex();
  VertexDataPropertyMap dataMap = get(vertex_data, (*data->iterator()->getGraph() ) );
//   VertexTokenPropertyMap tokenMap =get(vertex_token, (*data->iterator()->getGraph() ) );

  std::vector< ChainStackTuple > pile;
  //  std::stack< LinguisticGraphVertex > pileSons;
  Common::MediaticData::ChainsType currentType = Common::MediaticData::NO_CHAIN_TYPE;
//   std::stack< std::pair< std::deque< ChainStackTuple >, std::stack< LinguisticGraphVertex > > > tank;
  std::vector< std::vector< ChainStackTuple > > tank;
  std::set< LinguisticGraphVertex > alreadyFinished;
  std::vector<LinguisticGraphVertex> nextVxs;
//   LDEBUG << "Initializing nextVxs with " << start;
  nextVxs.push_back(start);


  while (! ( tank.empty() && nextVxs.empty()) )
  {
#ifdef ANTINNO_SPECIFIC
		if (stopAnalyze)
		{
      SACLOGINIT;
			LERROR << "Analyze too long. Stopped in SyntacticAnalyzerChains";
			return;
		}
#endif
    //     LDEBUG << "LOOP";
    if (pile.size() >= m_maxChainLength)
    {
#ifdef DEBUG_LP
      SACLOGINIT;
      LNOTICE << "Chain reached its max size or is too long.";
#endif

//       LDEBUG << "Trying to find a chain end in the too long stack";
      LinguisticGraphVertex lastChainVx = unstackUptoChainEnd(data, pile, currentType);
      if (lastChainVx != first) {
//         LDEBUG << "Chain end is " << lastChainVx << ". Reporting the chain in the graph.";
        std::string newChainString = stringChain(data,pile, currentType, alreadyFinished,startChainId,lastChainVx);
        alreadyReported.insert(newChainString);
        reportChainInGraph(data,pile, currentType, alreadyFinished,startChainId,lastChainVx);
        LinguisticGraphOutEdgeIt it, it_end;
        boost::tie(it, it_end) = out_edges(lastChainVx, *(data->graph()));
//         LDEBUG << "Initializing for the sons of " << lastChainVx;
        for (; it != it_end; it++)
        {
#ifdef ANTINNO_SPECIFIC
					if (stopAnalyze)
					{
#ifdef DEBUG_LP
            SACLOGINIT
						LERROR << "Analyze too long. Stopped in SyntacticAnalyzerChains";
#endif
						return;
					}
#endif
//           LDEBUG << "Looking at an out edge of the chain's last vertex : " << *it;
          LinguisticGraphVertex nextVx = target(*it, *(data->graph()));
          if (alreadyFinished.find(nextVx) == alreadyFinished.end())
          {
//             LDEBUG << "Adding " << nextVx << " to nextVxs";
            nextVxs.push_back(nextVx);
          }
        }
      } 
      else {
//         LDEBUG << "NoChainEndInStack";
      }
      if ( ! tank.empty() )
      {
//         LDEBUG << "Using a new stack after chain too long";
//         boost::tie(pile, pileSons) = tank.back();
        pile = tank.back();
        tank.pop_back();
      }
    }
    else if (tank.empty())
    {
//       LDEBUG << "tank is empty";
      LinguisticGraphVertex nextVx = nextVxs.back();
      nextVxs.pop_back();
      while (alreadyFinished.find(nextVx) != alreadyFinished.end())
      {
#ifdef ANTINNO_SPECIFIC
				if (stopAnalyze)
				{
					SACLOGINIT;
					LERROR << "Analyze too long. Stopped in SyntacticAnalyzerChains";
					return;
				}
#endif
        if (nextVxs.empty())
        {
//           LDEBUG << "Nothing more to work on: returning";
          return;
        }
//         LDEBUG << "Ignoring next vertex " << nextVx << " because it is already finished.";
        nextVx = nextVxs.back();
        nextVxs.pop_back();
        while ((vertexChainIdMap[nextVx].size() >= m_maxChainsNbByVertex) )
        {
          SACLOGINIT;
          LNOTICE << "Vertex ignored (" << nextVx << ") because there is too much chains on it.";
//           LDEBUG << "Ignoring next vertex " << nextVx << " because there is too much chains on it.";
          if (nextVxs.empty())
          {
//             LDEBUG << "Nothing more to work on: returning";
            return;
          }
          nextVx = nextVxs.back();
          nextVxs.pop_back();
        }
      }
//       LDEBUG << "next vertex is " << nextVx;
      bool canFinish = false;
      pile.clear();
//       pileSons = std::stack< LinguisticGraphVertex >();
      if ( (nextVx != first) && (nextVx != last) &&
           ( data->matrices()->canNominalChainBeginBy(dataMap[nextVx]) ) )
      {
//         LDEBUG << "next vertex is a nominal chain beginning";
        canFinish = (data->matrices()-> canNominalChainEndBy(dataMap[nextVx]));
        pile.push_back(boost::make_tuple(nextVx, canFinish, std::vector< LinguisticGraphVertex >()));
        currentType = NOMINAL;
      }
      else if ( (nextVx != first) && (nextVx != last) &&
                ( data->matrices()-> canVerbalChainBeginBy(dataMap[nextVx]) ) )
      {
//         LDEBUG << "next vertex is a verbal chain beginning";
        canFinish = ( data->matrices()-> canVerbalChainEndBy(dataMap[nextVx]));
        pile.push_back(boost::make_tuple(nextVx, canFinish, std::vector< LinguisticGraphVertex >()));
        currentType = VERBAL;
      }
      else
      {
//         LDEBUG << "next vertex " << nextVx << " is not a chain beginning";
        currentType = NO_CHAIN_TYPE;
//         LDEBUG << "Adding nextVx " << nextVx << " to alreadyFinished";
//         alreadyFinished.insert(nextVx);
      }

      if (nextVx != stop)
      {
        std::vector< LinguisticGraphVertex > sons;
        LinguisticGraphOutEdgeIt it, it_end;
        boost::tie(it, it_end) = out_edges(nextVx, *(data->graph()));
        for (; it != it_end; it++)
        {
#ifdef ANTINNO_SPECIFIC
					if (stopAnalyze)
					{
						SACLOGINIT;
						LERROR << "Analyze too long. Stopped in SyntacticAnalyzerChains";
						return;
					}
#endif
//           LDEBUG << "Looking at the next vertex out edge: " << *it;
          LinguisticGraphVertex nextNext = target(*it, *(data->graph()));
          if (nextNext != last)
          {
            if ( ( alreadyFinished.find(nextNext) == alreadyFinished.end()) && (currentType != NO_CHAIN_TYPE) )
            {
//               LDEBUG << "Adding " << nextNext << " to sons of " << nextVx;
              sons.push_back(nextNext);
            }
            else
            {
//               LDEBUG << "Adding " << nextNext << " to nextVxs";
              nextVxs.push_back(nextNext);
              // The addition of the line below seems to solve a loop problem
              // whithout producing regressions in TVA tests.
              alreadyFinished.insert(nextVx);
            }
          }
        }
        if (!sons.empty() && !pile.empty())
        {
//           LDEBUG << nextVx << " has sons: pushing them to the tank";
//           tank.push_back(std::make_pair(pile, sons));
          pile.back().get<2>() = sons;
          tank.push_back(pile);
        }
      }
    }
    else
    {
      LinguisticGraphVertex father = pile.back().get<0>();
      LinguisticGraphVertex currentSon = pile.back().get<2>().back();
//       LDEBUG << "Father and current son are: " << father << " / " << currentSon;
      pile.back().get<2>().pop_back();
      if ( (currentType == NO_CHAIN_TYPE) && (pile.empty()) )
      {
        if ( data->matrices()->canNominalChainBeginBy(dataMap[currentSon]))
          currentType = NOMINAL;
        else if ( data->matrices()->canVerbalChainBeginBy(dataMap[currentSon]))
          currentType = VERBAL;
      }

      if ( currentType != NO_CHAIN_TYPE )
      {
//         LDEBUG << "Current type is " << currentType;
        // ------------->
        // endroit ou mettre le bloc deplace
        // <-------------
        if ( (currentSon != last) &&
             ( data->matrices()-> belongsToMatrix(
                   dataMap[father],
                   dataMap[currentSon],
                   currentType ) ) )
        {
//           LDEBUG << father << " -> " << currentSon << " is in the matrix";
          bool canFinish = ( data->matrices()->canChainEndBy(dataMap[currentSon], currentType));
          // bloc ci-dessous a deplacer plus haut pour explorer
          // toutes les chaines. Pb: rend le parcours tres tres lourd.
          // ------------->
          if (!pile.empty() && !pile.back().get<2>().empty())
          {
//             LDEBUG << father << " has remaining sons: pushing them to the tank";
//             tank.push_back(std::make_pair(pile, pileSons));
            tank.push_back(pile);
          }
          // <-------------
//           LDEBUG << "Pushing " << currentSon << "(" << canFinish << ")";
          pile.push_back(boost::make_tuple(currentSon, canFinish, std::vector< LinguisticGraphVertex >()));
          if (currentSon != stop)
          {
            std::vector< LinguisticGraphVertex >& sons = pile.back().get<2>();
            LinguisticGraphOutEdgeIt it, it_end;
            boost::tie(it, it_end) = out_edges(currentSon, *(data->graph()));
            for (; it != it_end; it++)
            {
//               LDEBUG << "Edge is " << *it;
//               LDEBUG << "Adding " << target(*it, *(data->graph())) << " to sons of " << currentSon;
              sons.push_back(target(*it, *(data->graph())));
            }
          }
          else
          {
//             LDEBUG << "Stop reached";
            if (canFinish)
            {
//               LDEBUG << "currentSon " << currentSon << " is a possible end. Reporting the chain in the graph.";
              std::string newChainString = stringChain(data, pile, currentType, alreadyFinished,startChainId,currentSon);
              alreadyReported.insert(newChainString);
              reportChainInGraph(data, pile, currentType, alreadyFinished,startChainId, currentSon);
            }
            else
            {
//               LDEBUG << "currentSon " << currentSon << " is not a possible end.";
//               LDEBUG << "Trying to find a chain end in the stack";
              LinguisticGraphVertex lastChainVx = unstackUptoChainEnd(data, pile, currentType);
              if (lastChainVx!=first) {
//                 LDEBUG << "Chain end is " << lastChainVx << ". Reporting the chain in the graph.";
                std::string newChainString = stringChain(data, pile, currentType, alreadyFinished,startChainId,lastChainVx);
                alreadyReported.insert(newChainString);
                reportChainInGraph(data, pile, currentType, alreadyFinished,startChainId,lastChainVx);
                LinguisticGraphOutEdgeIt it, it_end;
                boost::tie(it, it_end) = out_edges(lastChainVx, *(data->graph()));
//                 LDEBUG << "Initializing for the sons of " << lastChainVx;
                for (; it != it_end; it++)
                {
//                   LDEBUG << "Looking at an out edge of the chain's last vertex : " << *it;
                  LinguisticGraphVertex nextVx = target(*it, *(data->graph()));
                  if (alreadyFinished.find(nextVx) == alreadyFinished.end())
                  {
//                     LDEBUG << "Adding " << nextVx << " to nextVxs";
                    nextVxs.push_back(nextVx);
                  }
                }
              } 
//               else 
//               {
//                 LDEBUG << "NoChainEndInStackException catched";
//               }
            }
          }
        }
        else
        {
#ifdef ANTINNO_SPECIFIC
					if (stopAnalyze)
					{
						SACLOGINIT;
						LERROR << "Analyze too long. Stopped in SyntacticAnalyzerChains";
						return;
					}
#endif
//           LDEBUG << father << " -> " << currentSon << " NOT in the matrix";
          LinguisticGraphVertex lastChainVx = unstackUptoChainEnd(data, pile, currentType);
          if (lastChainVx!=first) 
          {
            std::string newChainString = stringChain(data, pile, currentType, alreadyFinished,startChainId,lastChainVx);
            if (alreadyReported.find(newChainString) == alreadyReported.end())
            {
//               LDEBUG << "Reporting chain: " << newChainString;
              alreadyReported.insert(newChainString);
              reportChainInGraph(data, pile, currentType, alreadyFinished,startChainId,lastChainVx);
              LinguisticGraphOutEdgeIt it, it_end;
              boost::tie(it, it_end) = out_edges(lastChainVx, *(data->graph()));
//               LDEBUG << "Initializing for the sons of " << lastChainVx << " after unstacking";
              for (; it != it_end; it++)
              {
#ifdef ANTINNO_SPECIFIC
								if (stopAnalyze)
								{
									SACLOGINIT;
									LERROR << "Analyze too long. Stopped in SyntacticAnalyzerChains";
									return;
								}
#endif
//                 LDEBUG << "Looking at an out edge of the chain's last vertex : " << *it;
                LinguisticGraphVertex nextVx = target(*it, *(data->graph()));
                if (alreadyFinished.find(nextVx) == alreadyFinished.end())
                {
//                   LDEBUG << "Adding " << nextVx << " to nextVxs";
                  nextVxs.push_back(nextVx);
                }
              }
            }
//             else
//             {
//               LDEBUG << "This chain (" << newChainString << ") has already been found. Nothing to do.";
//             }
          } 
          else 
          {
//             LDEBUG << "No end of chain found in pile";
            if (alreadyFinished.find(currentSon) == alreadyFinished.end())
            {
              if ( parentsFinished(data, father, alreadyFinished ) )
              {
//                 LDEBUG << "Adding father " << father << " to alreadyFinished";
                alreadyFinished.insert(father);
              }
              if (currentSon != last)
              {
//                 LDEBUG << "Adding " << currentSon << " to nextVxs";
                nextVxs.push_back(currentSon);
              }
              else
              {
//                 LDEBUG << "Adding current son " << currentSon << " to alreadyFinished";
                alreadyFinished.insert(currentSon);
              }
            }
          }
        }
      }

      if ( (pile.empty() || pile.back().get<2>().empty()) && (! tank.empty()) )
      {
//         LDEBUG << "Using a new stack";
//         boost::tie(pile, pileSons) = tank.back();
        pile = tank.back();
        tank.pop_back();
      }
    }
  }
//   LDEBUG << "<========= chains search finished";
}

void SyntacticAnalyzerChains::reportChainInGraph(
    SyntacticData* data,
    const std::vector< ChainStackTuple >& pile,
    Common::MediaticData::ChainsType type,
    std::set< LinguisticGraphVertex >& alreadyFinished,
                                                  uint64_t& chainId,
                                                  const LinguisticGraphVertex& stop) const
{
//     SACLOGINIT;
//     LDEBUG << "SyntacticAnalyzerChains::reportChainInGraph";

    ChainIdStruct property = ChainIdStruct(type, chainId);

    VertexChainIdPropertyMap vertexChainIdMap = get( vertex_chain_id, *(data->graph()) );

    std::vector< ChainStackTuple >::const_iterator it, it_end;
    it = pile.begin(); it_end = pile.end();
    for (; it != it_end; it++)
    {
      LinguisticGraphVertex current = (*it).get<0>();
      if ((vertexChainIdMap[current].size() >= m_maxChainsNbByVertex) )
      {
        SACLOGINIT;
        LNOTICE << "Too much chains on " << current << " ; cannot add a new one.";
        return;
      }
    }


    std::vector< ChainStackTuple >::const_iterator it_beg, it_last;
    it = pile.begin(); it_beg = pile.begin();
    it_end = pile.end(); it_last = --(pile.end());
    std::ostringstream oss;
    for (; it != it_end; it++)
    {
      LinguisticGraphVertex current = (*it).get<0>();
      if (it == it_beg)
      {
        if (it_beg == it_last)
          property = ChainIdStruct(type, chainId, LinguisticAnalysisStructure::UNIGRAM);
        else
          property = ChainIdStruct(type, chainId, LinguisticAnalysisStructure::BEGIN);
      }
      else if (it == it_last)
      {
        property = ChainIdStruct(type, chainId, LinguisticAnalysisStructure::END);
      }
      else
      {
        property = ChainIdStruct(type, chainId, LinguisticAnalysisStructure::PART);
      }
      oss << current;
      if (current != data->iterator()->firstVertex() && current != data->iterator()->lastVertex()
          && (vertexChainIdMap[current].size() < m_maxChainsNbByVertex) )
      {
//         LDEBUG << "executing: vertexChainIdMap[" << current << "].insert(" << property << ")";
        vertexChainIdMap[current].insert(property);

        if (pile.size() > 1)
        {
          std::vector< ChainStackTuple >::const_iterator it2, it2_end;
          it2 = pile.begin(); it2_end = pile.end();
          bool ok = false;
          for (; it2 != it2_end; it2++)
          {
            LinguisticGraphVertex other = (*it2).get<0>();
            if (other != current)
            {
              LinguisticGraphEdge e; bool found;
              boost::tie (e, found) = edge(current, other, *(data->graph()));
              if (found)
              {
                ok = true;
                break;
              }
              else
              {
                boost::tie(e, found) = edge(other, current, *(data->graph()));
                if (found)
                {
                  ok = true;
                  break;
                }
              }
            }
          }
          if (!ok)
          {
            SACLOGINIT;
            LWARN << "An edge should exist for " << current << " !";
          }
        }
      }
      else if (vertexChainIdMap[current].size() >= m_maxChainsNbByVertex)
      {
        SACLOGINIT;
        LNOTICE << "Too much chains on " << current << " ; cannot add a new one.";
      }
      if (current == stop)
        break;
      else
        oss << " ";
      if (current != data->iterator()->firstVertex() && current != data->iterator()->lastVertex()
          && (vertexChainIdMap[current].size() < m_maxChainsNbByVertex) )
        if (parentsFinished(data, current, alreadyFinished))
      {
/*        LDEBUG << "Parents of " << current << " are finished ; so it too.";
        alreadyFinished.insert(current);*/
      }
    }
//     LDEBUG << "Chain " << chainId << " is : " << (type==NOMINAL?"nominal":"verbal") << " " << oss.str();
    chainId++;
  }

bool SyntacticAnalyzerChains::parentsFinished(
    const SyntacticData* data,
    const LinguisticGraphVertex& v,
    const std::set< LinguisticGraphVertex >& alreadyFinished) const
{
/*
  Critical function : comment logging messages
*/
//    SACLOGINIT;
//    LDEBUG << "SyntacticAnalyzerChains::parentsFinished";

    LinguisticGraphInEdgeIt it, it_end;
    boost::tie(it, it_end) = in_edges(v, *(data->graph()));
    for (; it != it_end; it++)
    {
      if (alreadyFinished.find(source(*it, *(data->graph()))) == alreadyFinished.end())
        return false;
    }
    return true;
}

std::string SyntacticAnalyzerChains::stringChain(
    const SyntacticData* data,
    const std::vector< ChainStackTuple >& pile,
    Common::MediaticData::ChainsType type,
    std::set< LinguisticGraphVertex >& alreadyFinished,
    uint64_t chainId,
    const LinguisticGraphVertex& stop) const
{
/*
  Critical Function : comment logging messages
*/
//  SACLOGINIT;
  ChainIdStruct property = ChainIdStruct(type, chainId);

  std::vector< ChainStackTuple >::const_iterator it, it_beg, it_end, it_last;
  it = pile.begin(); it_beg = pile.begin();
  it_end = pile.end(); it_last = --(pile.end());
  std::ostringstream oss;
  for (; it != it_end; it++)
  {
    if (it == it_beg)
    {
      if (it_beg == it_last)
        property = ChainIdStruct(type, chainId, LinguisticAnalysisStructure::UNIGRAM);
      else
        property = ChainIdStruct(type, chainId, LinguisticAnalysisStructure::BEGIN);
    }
    else if (it == it_last)
    {
      property = ChainIdStruct(type, chainId, LinguisticAnalysisStructure::END);
    }
    else
    {
      property = ChainIdStruct(type, chainId, LinguisticAnalysisStructure::PART);
    }
    oss << (*it).get<0>();
    LinguisticGraphVertex current = (*it).get<0>();
    if (current == stop)
      break;
    else
      oss << " ";
    if (current != data->iterator()->firstVertex() && current != data->iterator()->lastVertex())
    {
      if (pile.size() > 1)
      {
        std::vector< ChainStackTuple >::const_iterator it2, it2_end;
        // @todo replace by lookup only previous and next vertex in pile
        it2 = pile.begin(); it2_end = pile.end();
        bool ok = false;
        for (; it2 != it2_end; it2++)
        {
          LinguisticGraphVertex other = (*it2).get<0>();
          if (other != current)
          {
            LinguisticGraphEdge e; bool found;
            boost::tie (e, found) = edge(current, other, *(data->graph()));
            if (found)
            {
              ok = true;
              break;
            }
            else
            {
              boost::tie (e, found) = edge(other, current, *(data->graph()));
              if (found)
              {
                ok = true;
                break;
              }
            }
          }
        }
        if (!ok)
        {
          SALOGINIT;
          LWARN << "An edge should exist for " << current << " !";
        }
      }
    }
    if ( parentsFinished(data, current, alreadyFinished) )
    {
//      LDEBUG << "Adding current " << current << " to alreadyFinished";
      alreadyFinished.insert(current);
    }
  }
//  LDEBUG << "In stringChain, chain " << chainId << " is : " << (type==NOMINAL?"nominal":"verbal") << " " << oss.str();
  return oss.str();
}

LinguisticGraphVertex SyntacticAnalyzerChains::unstackUptoChainEnd(
        const SyntacticData* data,
        std::vector< ChainStackTuple >& pile,
        Common::MediaticData::ChainsType type
  ) const
{
/*
  Critical function : commeng logging messages
*/
//  SACLOGINIT;
//  LDEBUG << "unstackUptoChainEnd " << (type==NOMINAL?"nominal":(type==VERBAL?"verbal":"none"));
  CVertexDataPropertyMap dataMap = get( vertex_data, (*data->iterator()->getGraph()) );

  std::vector< ChainStackTuple >::const_reverse_iterator rit, rit_end;
  rit = pile.rbegin(); rit_end = pile.rend();
  for (; rit != rit_end; rit++)
  {
    if ( data->matrices()->canChainEndBy(dataMap[(*rit).get<0>()], type))
      break;
//    LDEBUG << "chain cannot finish by " << (*rit).get<0>();
  }

  if (rit != rit_end)
  {
    LinguisticGraphVertex newChainEnd = (*rit).get<0>();
//    LDEBUG << "Chain end found in pile: " << newChainEnd;
    return (newChainEnd);
  }
  else
  {
//    LDEBUG << "No chain end found in pile !";
    return data->iterator()->firstVertex();
  }
}

} // closing namespace SyntacticAnalysis
} // closing namespace LinguisticProcessing
} // closing namespace Lima
