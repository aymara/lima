/**
  *
  * @file        SyntacticAnalyzer-nochains.cpp
  * @author      Gael de Chalendar (Gael.de-Chalendar@cea.fr) \n
  *              Copyright (c) 2003 by CEA
  * @date        Created on Aug, 31 2004
  * @version     $Id$
  *
  */

#include "SyntacticAnalyzer-nochains.h"
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
using namespace boost;
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

SimpleFactory<MediaProcessUnit,SyntacticAnalyzerNoChains> syntacticAnalyzerNoChainsFactory(SYNTACTICANALYZERNOCHAINS_CLASSID);

SyntacticAnalyzerNoChains::SyntacticAnalyzerNoChains() :
    m_language(),
    m_chainMatrix(0),
    m_maxChainsNbByVertex(std::numeric_limits<uint64_t>::max())
{}

void SyntacticAnalyzerNoChains::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  SACLOGINIT;
  m_language=manager->getInitializationParameters().media;
  m_macroAccessor=&(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyAccessor("L_MACRO"));
  m_microAccessor=&(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyAccessor("L_MACRO_MICRO"));
  try
  {
    std::string chainMatrixId=unitConfiguration.getParamsValueAtKey("chainMatrix");
    m_chainMatrix=static_cast<SyntagmDefStruct*>(LinguisticResources::single().getResource(m_language,chainMatrixId));
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    LERROR << "no parameter 'chainMatrix' in SyntacticAnalyzerChains group for language " << (int) m_language << " !" << LENDL;
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
    LWARN << "no parameter 'maxChainsNbByVertex' in SyntacticAnalyzerChains group for language " << (int) m_language << " ! Using default: "<<DEFAULT_MAXCHAINSNBBYVERTEX<<"." << LENDL;
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
    LWARN << "no parameter 'maxChainLength' in SyntacticAnalyzerChains group for language " << (int) m_language << " ! Using default: "<<DEFAULT_MAXCHAINLENGTH<<"." << LENDL;
    m_maxChainLength = DEFAULT_MAXCHAINLENGTH;
  }
  try
  {
    std::string id=unitConfiguration.getParamsValueAtKey("ponctuCategory");
    m_ponctuCategory=static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyManager("L_MACRO").getPropertyValue(id);
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    LWARN << "No ponctu macrocategory defined ! use category L_PONCTU" << LENDL;
    m_ponctuCategory=static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyManager("L_MACRO").getPropertyValue("L_PONCTU");
  }

}

LimaStatusCode SyntacticAnalyzerNoChains::process(
  AnalysisContent& analysis) const
{
  Lima::TimeUtilsController timer("SyntacticAnalysis");
  SACLOGINIT;
  LINFO << "start syntactic analysis - nochains" << LENDL;
  // create syntacticData
  AnalysisGraph* anagraph=static_cast<AnalysisGraph*>(analysis.getData("PosGraph"));
  if (anagraph==0)
  {
    LERROR << "no PosGraph ! abort" << LENDL;
    return MISSING_DATA;
  }
  SegmentationData* sb=static_cast<SegmentationData*>(analysis.getData("SentenceBoundaries"));
  if (sb==0)
  {
    LERROR << "no sentence bounds ! abort" << LENDL;
    return MISSING_DATA;
  }
  if (sb->getGraphId() != "PosGraph") {
    LERROR << "SentenceBounds have been computed on " << sb->getGraphId() << " !" << LENDL;
    LERROR << "SyntacticAnalyzer-deps needs SentenceBounds on PosGraph" << LENDL;
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
    identifyChains(syntacticData,beginSentence,endSentence,chainId);
    beginSentence=endSentence;
  }

  LINFO << "end syntactic analysis - nochains" << LENDL;
  return SUCCESS_ID;
}


void SyntacticAnalyzerNoChains::identifyChains(SyntacticData* data,
                                       const LinguisticGraphVertex& start,
                                       const LinguisticGraphVertex& stop,
                                       uint64_t& startChainId) const
{
//   SACLOGINIT;
//   LDEBUG << "Searching chains from/to (morph): " << start << "/" << stop << LENDL;
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
//   LDEBUG << "Initializing nextVxs with " << start << LENDL;
  nextVxs.push_back(start);


  while (! ( tank.empty() && nextVxs.empty()) )
  {
//     LDEBUG << "LOOP" << LENDL;
    if (pile.size() >= m_maxChainLength)
    {
      SACLOGINIT;
      LNOTICE << "Chain reached its max size or is too long." << LENDL;
//       LDEBUG << "Trying to find a chain end in the too long stack" << LENDL;
      LinguisticGraphVertex lastChainVx = unstackUptoChainEnd(data, pile, currentType);
      if (lastChainVx != first) {
//         LDEBUG << "Chain end is " << lastChainVx << ". Reporting the chain in the graph." << LENDL;
        std::string newChainString = stringChain(data,pile, currentType, alreadyFinished,startChainId,lastChainVx);
        alreadyReported.insert(newChainString);
        reportChainInGraph(data,pile, currentType, alreadyFinished,startChainId,lastChainVx);
        LinguisticGraphOutEdgeIt it, it_end;
        boost::tie(it, it_end) = out_edges(lastChainVx, *(data->graph()));
//         LDEBUG << "Initializing for the sons of " << lastChainVx << LENDL;
        for (; it != it_end; it++)
        {
//           LDEBUG << "Looking at an out edge of the chain's last vertex : " << *it << LENDL;
          LinguisticGraphVertex nextVx = target(*it, *(data->graph()));
          if (alreadyFinished.find(nextVx) == alreadyFinished.end())
          {
//             LDEBUG << "Adding " << nextVx << " to nextVxs" << LENDL;
            nextVxs.push_back(nextVx);
          }
        }
      } 
      else {
//         LDEBUG << "NoChainEndInStack" << LENDL;
      }
      if ( ! tank.empty() )
      {
//         LDEBUG << "Using a new stack after chain too long" << LENDL;
//         boost::tie(pile, pileSons) = tank.back();
        pile = tank.back();
        tank.pop_back();
      }
    }
    else if (tank.empty())
    {
//       LDEBUG << "tank is empty" << LENDL;
      LinguisticGraphVertex nextVx = nextVxs.back();
      nextVxs.pop_back();
      while (alreadyFinished.find(nextVx) != alreadyFinished.end())
      {
        if (nextVxs.empty())
        {
//           LDEBUG << "Nothing more to work on: returning" << LENDL;
          return;
        }
//         LDEBUG << "Ignoring next vertex " << nextVx << " because it is already finished." << LENDL;
        nextVx = nextVxs.back();
        nextVxs.pop_back();
        while ((vertexChainIdMap[nextVx].size() >= m_maxChainsNbByVertex) )
        {
          SACLOGINIT;
          LNOTICE << "Vertex ignored (" << nextVx << ") because there is too much chains on it." << LENDL;
//           LDEBUG << "Ignoring next vertex " << nextVx << " because there is too much chains on it." << LENDL;
          if (nextVxs.empty())
          {
//             LDEBUG << "Nothing more to work on: returning" << LENDL;
            return;
          }
          nextVx = nextVxs.back();
          nextVxs.pop_back();
        }
      }
//       LDEBUG << "next vertex is " << nextVx << LENDL;
      bool canFinish = false;
      pile.clear();
//       pileSons = std::stack< LinguisticGraphVertex >();
      if ( (nextVx != first) && (nextVx != last) )
      {
//         LDEBUG << "next vertex is a nominal chain beginning" << LENDL;
        canFinish = true;
        pile.push_back(boost::make_tuple(nextVx, canFinish, std::vector< LinguisticGraphVertex >()));
        currentType = NOMINAL;
      }
      else
      {
//         LDEBUG << "next vertex " << nextVx << " is not a chain beginning" << LENDL;
        currentType = NO_CHAIN_TYPE;
//         LDEBUG << "Adding nextVx " << nextVx << " to alreadyFinished" << LENDL;
//         alreadyFinished.insert(nextVx);
      }

      if (nextVx != stop)
      {
        std::vector< LinguisticGraphVertex > sons;
        LinguisticGraphOutEdgeIt it, it_end;
        boost::tie(it, it_end) = out_edges(nextVx, *(data->graph()));
        for (; it != it_end; it++)
        {
//           LDEBUG << "Looking at the next vertex out edge: " << *it << LENDL;
          LinguisticGraphVertex nextNext = target(*it, *(data->graph()));
          if (nextNext != last)
          {
            if ( ( alreadyFinished.find(nextNext) == alreadyFinished.end()) && (currentType != NO_CHAIN_TYPE) )
            {
//               LDEBUG << "Adding " << nextNext << " to sons of " << nextVx << LENDL;
              sons.push_back(nextNext);
            }
            else
            {
//               LDEBUG << "Adding " << nextNext << " to nextVxs" << LENDL;
              nextVxs.push_back(nextNext);
              // The addition of the line below seems to solve a loop problem
              // whithout producing regressions in TVA tests.
              alreadyFinished.insert(nextVx);
            }
          }
        }
        if (!sons.empty() && !pile.empty())
        {
//           LDEBUG << nextVx << " has sons: pushing them to the tank" << LENDL;
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
//       LDEBUG << "Father and current son are: " << father << " / " << currentSon << LENDL;
      pile.back().get<2>().pop_back();
      if ( (currentType == NO_CHAIN_TYPE) && (pile.empty()) )
      {
        currentType = NOMINAL;
      }

      if ( currentType != NO_CHAIN_TYPE )
      {
//         LDEBUG << "Current type is " << currentType << LENDL;
        // ------------->
        // endroit ou mettre le bloc deplace
        // <-------------
        if ( currentSon != last )
        {
//           LDEBUG << father << " -> " << currentSon << " is in the matrix" << LENDL;
          bool canFinish = true;
          // bloc ci-dessous a deplacer plus haut pour explorer
          // toutes les chaines. Pb: rend le parcours tres tres lourd.
          // ------------->
          if (!pile.empty() && !pile.back().get<2>().empty())
          {
//             LDEBUG << father << " has remaining sons: pushing them to the tank" << LENDL;
//             tank.push_back(std::make_pair(pile, pileSons));
            tank.push_back(pile);
          }
          // <-------------
//           LDEBUG << "Pushing " << currentSon << "(" << canFinish << ")" << LENDL;
          pile.push_back(boost::make_tuple(currentSon, canFinish, std::vector< LinguisticGraphVertex >()));
          if (currentSon != stop)
          {
            std::vector< LinguisticGraphVertex >& sons = pile.back().get<2>();
            LinguisticGraphOutEdgeIt it, it_end;
            boost::tie(it, it_end) = out_edges(currentSon, *(data->graph()));
            for (; it != it_end; it++)
            {
//               LDEBUG << "Edge is " << *it << LENDL;
//               LDEBUG << "Adding " << target(*it, *(data->graph())) << " to sons of " << currentSon << LENDL;
              sons.push_back(target(*it, *(data->graph())));
            }
          }
          else
          {
//             LDEBUG << "Stop reached" << LENDL;
            if (canFinish)
            {
//               LDEBUG << "currentSon " << currentSon << " is a possible end. Reporting the chain in the graph." << LENDL;
              std::string newChainString = stringChain(data, pile, currentType, alreadyFinished,startChainId,currentSon);
              alreadyReported.insert(newChainString);
              reportChainInGraph(data, pile, currentType, alreadyFinished,startChainId, currentSon);
            }
            else
            {
//               LDEBUG << "currentSon " << currentSon << " is not a possible end." << LENDL;
//               LDEBUG << "Trying to find a chain end in the stack" << LENDL;
              LinguisticGraphVertex lastChainVx = unstackUptoChainEnd(data, pile, currentType);
              if (lastChainVx!=first) {
//                 LDEBUG << "Chain end is " << lastChainVx << ". Reporting the chain in the graph." << LENDL;
                std::string newChainString = stringChain(data, pile, currentType, alreadyFinished,startChainId,lastChainVx);
                alreadyReported.insert(newChainString);
                reportChainInGraph(data, pile, currentType, alreadyFinished,startChainId,lastChainVx);
                LinguisticGraphOutEdgeIt it, it_end;
                boost::tie(it, it_end) = out_edges(lastChainVx, *(data->graph()));
//                 LDEBUG << "Initializing for the sons of " << lastChainVx << LENDL;
                for (; it != it_end; it++)
                {
//                   LDEBUG << "Looking at an out edge of the chain's last vertex : " << *it << LENDL;
                  LinguisticGraphVertex nextVx = target(*it, *(data->graph()));
                  if (alreadyFinished.find(nextVx) == alreadyFinished.end())
                  {
//                     LDEBUG << "Adding " << nextVx << " to nextVxs" << LENDL;
                    nextVxs.push_back(nextVx);
                  }
                }
              } 
//               else 
//               {
//                 LDEBUG << "NoChainEndInStackException catched" << LENDL;
//               }
            }
          }
        }
        else
        {
//           LDEBUG << father << " -> " << currentSon << " NOT in the matrix" << LENDL;
          LinguisticGraphVertex lastChainVx = unstackUptoChainEnd(data, pile, currentType);
          if (lastChainVx!=first) 
          {
            std::string newChainString = stringChain(data, pile, currentType, alreadyFinished,startChainId,lastChainVx);
            if (alreadyReported.find(newChainString) == alreadyReported.end())
            {
//               LDEBUG << "Reporting chain: " << newChainString << LENDL;
              alreadyReported.insert(newChainString);
              reportChainInGraph(data, pile, currentType, alreadyFinished,startChainId,lastChainVx);
              LinguisticGraphOutEdgeIt it, it_end;
              boost::tie(it, it_end) = out_edges(lastChainVx, *(data->graph()));
//               LDEBUG << "Initializing for the sons of " << lastChainVx << " after unstacking" << LENDL;
              for (; it != it_end; it++)
              {
//                 LDEBUG << "Looking at an out edge of the chain's last vertex : " << *it << LENDL;
                LinguisticGraphVertex nextVx = target(*it, *(data->graph()));
                if (alreadyFinished.find(nextVx) == alreadyFinished.end())
                {
//                   LDEBUG << "Adding " << nextVx << " to nextVxs" << LENDL;
                  nextVxs.push_back(nextVx);
                }
              }
            }
//             else
//             {
//               LDEBUG << "This chain (" << newChainString << ") has already been found. Nothing to do." << LENDL;
//             }
          } 
          else 
          {
//             LDEBUG << "No end of chain found in pile" << LENDL;
            if (alreadyFinished.find(currentSon) == alreadyFinished.end())
            {
              if ( parentsFinished(data, father, alreadyFinished ) )
              {
//                 LDEBUG << "Adding father " << father << " to alreadyFinished" << LENDL;
                alreadyFinished.insert(father);
              }
              if (currentSon != last)
              {
//                 LDEBUG << "Adding " << currentSon << " to nextVxs" << LENDL;
                nextVxs.push_back(currentSon);
              }
              else
              {
//                 LDEBUG << "Adding current son " << currentSon << " to alreadyFinished" << LENDL;
                alreadyFinished.insert(currentSon);
              }
            }
          }
        }
      }

      if ( (pile.empty() || pile.back().get<2>().empty()) && (! tank.empty()) )
      {
//         LDEBUG << "Using a new stack" << LENDL;
//         boost::tie(pile, pileSons) = tank.back();
        pile = tank.back();
        tank.pop_back();
      }
    }
  }
//   LDEBUG << "<========= chains search finished" << LENDL;
}

void SyntacticAnalyzerNoChains::reportChainInGraph(
    SyntacticData* data,
    const std::vector< ChainStackTuple >& pile,
    Common::MediaticData::ChainsType type,
    std::set< LinguisticGraphVertex >& alreadyFinished,
                                                  uint64_t& chainId,
                                                  const LinguisticGraphVertex& stop) const
{
//     SACLOGINIT;
//     LDEBUG << "SyntacticAnalyzerNoChains::reportChainInGraph" << LENDL;

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
        LNOTICE << "Too much chains on " << current << " ; cannot add a new one." << LENDL;
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
//         LDEBUG << "executing: vertexChainIdMap[" << current << "].insert(" << property << ")" << LENDL;
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
            LWARN << "An edge should exist for " << current << " !" << LENDL;
          }
        }
      }
      else if (vertexChainIdMap[current].size() >= m_maxChainsNbByVertex)
      {
        SACLOGINIT;
        LNOTICE << "Too much chains on " << current << " ; cannot add a new one." << LENDL;
      }
      if (current == stop)
        break;
      else
        oss << " ";
      if (current != data->iterator()->firstVertex() && current != data->iterator()->lastVertex()
          && (vertexChainIdMap[current].size() < m_maxChainsNbByVertex) )
        if (parentsFinished(data, current, alreadyFinished))
      {
/*        LDEBUG << "Parents of " << current << " are finished ; so it too." << LENDL;
        alreadyFinished.insert(current);*/
      }
    }
//     LDEBUG << "Chain " << chainId << " is : " << (type==NOMINAL?"nominal":"verbal") << " " << oss.str() << LENDL;
    chainId++;
  }

bool SyntacticAnalyzerNoChains::parentsFinished(
    const SyntacticData* data,
    const LinguisticGraphVertex& v,
    const std::set< LinguisticGraphVertex >& alreadyFinished) const
{
/*
  Critical function : comment logging messages
*/
//    SACLOGINIT;
//    LDEBUG << "SyntacticAnalyzerNoChains::parentsFinished" << LENDL;

    LinguisticGraphInEdgeIt it, it_end;
    boost::tie(it, it_end) = in_edges(v, *(data->graph()));
    for (; it != it_end; it++)
    {
      if (alreadyFinished.find(source(*it, *(data->graph()))) == alreadyFinished.end())
        return false;
    }
    return true;
}

std::string SyntacticAnalyzerNoChains::stringChain(
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
          LWARN << "An edge should exist for " << current << " !" << LENDL;
        }
      }
    }
    if ( parentsFinished(data, current, alreadyFinished) )
    {
//      LDEBUG << "Adding current " << current << " to alreadyFinished" << LENDL;
      alreadyFinished.insert(current);
    }
  }
//  LDEBUG << "In stringChain, chain " << chainId << " is : " << (type==NOMINAL?"nominal":"verbal") << " " << oss.str() << LENDL;
  return oss.str();
}

LinguisticGraphVertex SyntacticAnalyzerNoChains::unstackUptoChainEnd(
        const SyntacticData* data,
        std::vector< ChainStackTuple >& pile,
        Common::MediaticData::ChainsType type
  ) const
{
/*
  Critical function : commeng logging messages
*/
//  SACLOGINIT;
//  LDEBUG << "unstackUptoChainEnd " << (type==NOMINAL?"nominal":(type==VERBAL?"verbal":"none")) << LENDL;
  CVertexDataPropertyMap dataMap = get( vertex_data, (*data->iterator()->getGraph()) );

  std::vector< ChainStackTuple >::const_reverse_iterator rit, rit_end;
  rit = pile.rbegin(); rit_end = pile.rend();
  for (; rit != rit_end; rit++)
  {
    if ( data->matrices()->canChainEndBy(dataMap[(*rit).get<0>()], type))
      break;
//    LDEBUG << "chain cannot finish by " << (*rit).get<0>() << LENDL;
  }

  if (rit != rit_end)
  {
    LinguisticGraphVertex newChainEnd = (*rit).get<0>();
//    LDEBUG << "Chain end found in pile: " << newChainEnd << LENDL;
    return (newChainEnd);
  }
  else
  {
//    LDEBUG << "No chain end found in pile !" << LENDL;
    return data->iterator()->firstVertex();
  }
}

} // closing namespace SyntacticAnalysis
} // closing namespace LinguisticProcessing
} // closing namespace Lima
