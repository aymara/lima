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
/** @brief      Implementation of classes for disambiguation of syntagmatic chains paths.
  *
  * @file       ChainsDisambiguator.cpp
  * @author     Gael de Chalendar <Gael.de-Chalendar@cea.fr> 

  *             Copyright (c) 2004 by CEA
  * @version    $Id$
  */


#include "ChainsDisambiguator.h"

using namespace Lima::Common::MediaticData;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

namespace Lima
{
namespace LinguisticProcessing
{
namespace SyntacticAnalysis
{

#define MAXPATHS 15

ChainsDisambiguator::ChainsDisambiguator(SyntacticData* data,
    const LinguisticGraphVertex& s,
    const LinguisticGraphVertex& t,
    MediaId language,
    uint64_t depGraphMaxBranchingFactor) :
    m_hypsStack(),
    m_completePaths(),
    m_data(data),
    m_srcVertex(s),
    m_tgtVertex(t),
    m_language(language),
    m_depGraphMaxBranchingFactor(depGraphMaxBranchingFactor)
{
  m_microAccessor=&static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager().getPropertyAccessor("MICRO");
}

ChainsDisambiguator::ChainsDisambiguator(const ChainsDisambiguator& cd) :
    m_hypsStack(cd.m_hypsStack),
    m_completePaths(cd.m_completePaths),
    m_data(cd.m_data),
    m_srcVertex(cd.m_srcVertex),
    m_tgtVertex(cd.m_tgtVertex),
    m_language(cd.m_language),
    m_depGraphMaxBranchingFactor(cd.m_depGraphMaxBranchingFactor),
    m_microAccessor(cd.m_microAccessor)
{}

ChainsDisambiguator& ChainsDisambiguator::operator=(const ChainsDisambiguator& cd)
{
  m_hypsStack = cd.m_hypsStack;
  m_completePaths = cd.m_completePaths;
  m_data = cd.m_data;
  m_srcVertex = cd.m_srcVertex;
  m_tgtVertex = cd.m_tgtVertex;
  m_language = cd.m_language;
  m_depGraphMaxBranchingFactor = cd.m_depGraphMaxBranchingFactor;
  m_microAccessor = cd.m_microAccessor;
  return *this;
}

void ChainsDisambiguator::initPaths()
{
  const LinguisticGraph* graph = m_data->graph();
  LinguisticGraphVertex currentVertex = m_srcVertex;
  if (currentVertex == m_data->iterator()->firstVertex() || (currentVertex==m_data->iterator()->lastVertex()) )
  {
    uint64_t id = currentVertex;
    std::set< uint64_t> chainsToIgnore;
    Path initialPath(id, chainsToIgnore);
    m_hypsStack.push_front(initialPath);
    return;
  }
  CVertexDataPropertyMap dataMap = get(vertex_data, *graph);
  const MorphoSyntacticData* currentData = dataMap[currentVertex];
  LinguisticCode currentMicroCateg= currentData->firstValue(*m_microAccessor);
  CVertexChainIdPropertyMap chainsMap = get(vertex_chain_id, *graph);
  const std::set< ChainIdStruct >& currentVertexChains = chainsMap[currentVertex];
  uint64_t id = currentVertex;
  if (currentVertexChains.empty())
  {
    std::set< uint64_t> chainsToIgnore;
    Path initialPath(id, chainsToIgnore);
    initialPath.outChainsWordsNb()++;
    if (static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).isAPropositionIntroductor(currentMicroCateg))
      initialPath.idealConjVerbNb()++;
    m_hypsStack.push_front(initialPath);
  }
  else
  {
    unsigned char type = 1;
    std::set< ChainIdStruct >::const_iterator currentChainIt, currentChainIt_end;
    currentChainIt = currentVertexChains.begin();
    currentChainIt_end = currentVertexChains.end();
    for (; currentChainIt != currentChainIt_end ; currentChainIt++)
    {
      std::set< uint64_t> chainsToIgnore;
      std::set< ChainIdStruct >::const_iterator otherChainIt, otherChainIt_end;
      otherChainIt = currentVertexChains.begin();
      otherChainIt_end = currentVertexChains.end();
      for (; otherChainIt != otherChainIt_end ; otherChainIt++)
      {
        if (currentChainIt != otherChainIt)
        {
          chainsToIgnore.insert(otherChainIt->chainId());
        }
      }
      Path initialPath(id, chainsToIgnore, type, currentChainIt->chainId());
      if (static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).isAPropositionIntroductor(currentMicroCateg))
        initialPath.idealConjVerbNb()++;
      else if (static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).isAConjugatedVerb(currentMicroCateg))
        initialPath.conjVerbNb()++;
      initialPath.chainsNb()++;
      m_hypsStack.push_front(initialPath);
    }
  }
}

void ChainsDisambiguator::computePaths()
{
//   SADLOGINIT;
  const LinguisticGraph* graph = m_data->graph();
  CVertexDataPropertyMap dataMap = get(vertex_data, *graph);
  CVertexChainIdPropertyMap chainsMap = get(vertex_chain_id, *graph);
  while (!m_hypsStack.empty())
  {
    if (m_hypsStack.size() > MAXPATHS)
      m_hypsStack.pop_back();
    Path currentHyp = m_hypsStack.front();
    m_hypsStack.pop_front();
    if (currentHyp.key().id() == m_tgtVertex)
    {
      m_completePaths.insert(currentHyp);
    }
    else
    {
      Key& currentKey = currentHyp.key();
      LinguisticGraphVertex currentVertex = currentKey.id();
      const std::set< ChainIdStruct >& currentVertexChains = chainsMap[currentVertex];
//       LDEBUG << "New hyp on " << currentVertex << LENDL;
      const Elem& currentElem = *(currentHyp.elems().rbegin());
//       LDEBUG << "Current elem: " << currentElem << LENDL;
      LinguisticGraphOutEdgeIt it, it_end;
      boost::tie(it, it_end) = boost::out_edges(currentVertex, *graph);
      uint64_t branchNum = 0;
      for (; it != it_end; it++)
      {
        if (++branchNum >= m_depGraphMaxBranchingFactor)
        {
          SADLOGINIT;
          LWARN << "Breaking computePaths inner loop on "<<currentVertex<<" due to excessive branching factor."<<LENDL;
          break;
        }

        LinguisticGraphVertex nextVertex = target(*it, *graph);
//         LDEBUG << "Looking at next vertex: " << nextVertex << " (current is "<<currentVertex<<")" << LENDL;
        if (currentVertex==0 && nextVertex==1)
          continue;
        const std::set< ChainIdStruct >& nextVertexChains = chainsMap[nextVertex];
        LinguisticCode nextMicroCateg(0);
        const MorphoSyntacticData* nextData = dataMap[nextVertex];
        if (nextData == 0)
        {
          SADLOGINIT;
          LWARN << "vertex " << nextVertex << " has no data" << LENDL;
          // @TODO ensure that continuing with a 0 microcateg is a good solution
//           continue;
        }
        else
        {
          nextMicroCateg = nextData->firstValue(*m_microAccessor);
        }
//         LDEBUG << "vertex microcateg is " << nextMicroCateg << LENDL;
        if ( currentElem.type() == 0 ) // no chain on current vertex
        {
//           LDEBUG << "no chain on current vertex" << LENDL;
          // no chain on the new vertex
          if (nextVertexChains.empty())
          {
//             LDEBUG << "no chain on the new vertex" << LENDL;
            Path newHyp(currentHyp);
            Key newKey(nextVertex);
            Elem newElem(nextVertex);
            newHyp.elems().push_back(newElem);
            newHyp.key(newKey);
            newHyp.updateParams(nextMicroCateg, false, true, m_language);
            m_hypsStack.push_front(newHyp);
            if (m_hypsStack.size() > MAXPATHS)
              m_hypsStack.pop_back();
          }
          // one chain on the new vertex
          else if (nextVertexChains.size() == 1)
          {
//             LDEBUG << "one chain on the new vertex" << LENDL;
            Path newHyp(currentHyp);
            Key newKey(nextVertex);
            ChainIdStruct nextVertexChain = (*(nextVertexChains.begin()));
            uint64_t nextVertexChainId = nextVertexChain.chainId();
            Elem newElem(nextVertex);
            if ( ( nextVertexChain.elemType() == BEGIN ) ||
                 ( nextVertexChain.elemType() == UNIGRAM ) )
            {
              newElem = Elem(nextVertexChainId, 1);
              newElem.elems().push_back(nextVertex);
              newHyp.updateParams(nextMicroCateg, true, false, m_language);
            }
            else //END ou PART
            {
              newKey.chainsToIgnore().insert(( nextVertexChainId));
              newHyp.updateParams(nextMicroCateg, false, true, m_language);
            }
            newHyp.elems().push_back(newElem);
            newHyp.key(newKey);
            m_hypsStack.push_front(newHyp);
            if (m_hypsStack.size() > MAXPATHS)
              m_hypsStack.pop_back();
          }
          // several chains on the new vertex
          else
          {
//             LDEBUG << "several chains on the new vertex" << LENDL;
            startWithSeveralChainsOnNewVertex(nextVertex, nextVertexChains, currentHyp);
          }
        }
        else if ( currentKey.chainsToIgnore().empty() ) // a chain on current vertex and no chain to ignore
        {
//           LDEBUG << "a chain on current vertex and no chain to ignore" << LENDL;
          uint64_t currentElemChainId = currentElem.id();
          ChainIdStruct currentElemChainIdStruct;
          std::set< ChainIdStruct >::const_iterator currentVertexChainsIt, currentVertexChainsIt_end;
          currentVertexChainsIt = currentVertexChains.begin();
          currentVertexChainsIt_end = currentVertexChains.end();
          for (; currentVertexChainsIt != currentVertexChainsIt_end; currentVertexChainsIt++)
          {
            if ( (*currentVertexChainsIt).chainId() == currentElemChainId )
            {
              currentElemChainIdStruct = *currentVertexChainsIt;
              break;
            }
          }
          if (currentVertexChainsIt == currentVertexChainsIt_end)
            throw std::runtime_error("Current elem chain id not found in current elem chains.");

          if (nextVertexChains.empty()) // the current chain does not continue
          {
            // current chain was finished, no new chain
            if ( (currentElemChainIdStruct.elemType() == END) ||
                 (currentElemChainIdStruct.elemType() == UNIGRAM) )
            {
//               LDEBUG << "current chain was finished, no new chain" << LENDL;
              Path newHyp(currentHyp);
              Key newKey(nextVertex);
              Elem newElem(nextVertex);
              newHyp.key(newKey);
              newHyp.elems().push_back(newElem);
              newHyp.updateParams(nextMicroCateg, false, true, m_language);
              m_hypsStack.push_front(newHyp);
              if (m_hypsStack.size() > MAXPATHS)
                m_hypsStack.pop_back();
            }
            // current chain was not finished, we are on a divergent branch
            // current chain elem has to be removed and replaced by vertices elems
            // made from its elements
            else
            {
//               LDEBUG << "current chain was not finished, we are on a divergent branch" << LENDL;
//               LDEBUG << "current chain elem has to be removed and replaced by vertices elems" << LENDL;
//               LDEBUG << "made from its elements" << LENDL;
              Path newHyp(currentHyp);
              cancelCurrentChain(currentElem, newHyp);
              Key newKey(nextVertex);
              Elem newElem(nextVertex);
              newHyp.key(newKey);
              newHyp.elems().push_back(newElem);
              newHyp.updateParams(nextMicroCateg, false, true, m_language);
              m_hypsStack.push_front(newHyp);
              if (m_hypsStack.size() > MAXPATHS)
                m_hypsStack.pop_back();
            }
          }
          // one chain on the new vertex
          else if (nextVertexChains.size() == 1)
          {
//             LDEBUG << "one chain on the new vertex" << LENDL;
            // verify if we continue the same chain
            // or finish it and start a new one
            // or finish it and new vertex is a confluent on another chain

            // continue the same chain
            if ( (*(nextVertexChains.begin())).chainId() == currentElem.id() )
            {
//               LDEBUG << "continue the same chain" << LENDL;
              Path newHyp(currentHyp);
              Key newKey(nextVertex);
              Elem& newElem = *(newHyp.elems().rbegin());
              newElem.elems().push_back(nextVertex);
              newHyp.key(newKey);
              newHyp.updateParams(nextMicroCateg, false, false, m_language);
              m_hypsStack.push_front(newHyp);
              if (m_hypsStack.size() > MAXPATHS)
                m_hypsStack.pop_back();
            }
            else // 1. finish it or cancel it if it was not finished
              // 2. start a new chain or add to ignore if new vertex is a confluent on another chain (begin or continued)
            {
//               LDEBUG << "1. finish it or cancel it if it was not finished" << LENDL;
//               LDEBUG << "2. start a new chain or add to ignore if new vertex is a confluent on another chain (begin or continued)" << LENDL;
              Path newHyp(currentHyp);
              // current chain was not finished, we are on a divergent branch
              // current chain elem has to be removed and replaced by vertices elems
              // made from its elements
              if (!( (currentElemChainIdStruct.elemType() == END) ||
                     (currentElemChainIdStruct.elemType() == UNIGRAM) ))
              {
//                 LDEBUG << "current chain was not finished, we are on a divergent branch" << LENDL;
//                 LDEBUG << "current chain elem has to be removed and replaced by vertices elems" << LENDL;
                cancelCurrentChain(currentElem, newHyp);
              }
              // now continue as when there was no chain on current vertex
              Key newKey(nextVertex);
              ChainIdStruct nextVertexChain = (*(nextVertexChains.begin()));
              uint64_t nextVertexChainId = nextVertexChain.chainId();
              Elem newElem(nextVertex);
              if ( ( nextVertexChain.elemType() == BEGIN ) ||
                   ( nextVertexChain.elemType() == UNIGRAM ) )
              {
                newElem = Elem(nextVertexChainId, 1);
                newElem.elems().push_back(nextVertex);
                newHyp.updateParams(nextMicroCateg, true, false, m_language);
              }
              else //END ou PART
              {
                newKey.chainsToIgnore().insert(( nextVertexChainId));
                newHyp.updateParams(nextMicroCateg, false, true, m_language);
              }
              newHyp.elems().push_back(newElem);
              newHyp.key(newKey);
              m_hypsStack.push_front(newHyp);
              if (m_hypsStack.size() > MAXPATHS)
                m_hypsStack.pop_back();
            }
          }
          // several chains on the new vertex
          else
          {
//             LDEBUG << "several chains on the new vertex" << LENDL;
            // verify if we continue the same chain
            // or finish it and start a new one
            // or finish it and new vertex is a confluent on another chain

            // continue the same chain
            bool currentChainFound = false;
            std::set< uint64_t > nextVertexChainsIdsToIgnore;
            std::set< ChainIdStruct >::const_iterator nextVertexChainsIt, nextVertexChainsIt_end;
            nextVertexChainsIt = nextVertexChains.begin();
            nextVertexChainsIt_end = nextVertexChains.end();
            for(; nextVertexChainsIt != nextVertexChainsIt_end; nextVertexChainsIt++)
            {
              if ( (*nextVertexChainsIt).chainId() == currentElem.id() )
                currentChainFound = true;
              else
                nextVertexChainsIdsToIgnore.insert((*nextVertexChainsIt).chainId());
            }
            // continue the same chain
            if (currentChainFound)
            {
//               LDEBUG << "continue the same chain" << LENDL;
              Path newHyp(currentHyp);
              Key newKey(nextVertex, nextVertexChainsIdsToIgnore);
              Elem& newElem = *(newHyp.elems().rbegin());
              newElem.elems().push_back(nextVertex);
              newHyp.key(newKey);
              newHyp.updateParams(nextMicroCateg, false, false, m_language);
              m_hypsStack.push_front(newHyp);
              if (m_hypsStack.size() > MAXPATHS)
                m_hypsStack.pop_back();
            }
            else // 1. finish it or cancel it if it was not finished
              // 2. start a new chain or add to ignore if new vertex is a confluent on another chain (begin or continued)
            {
//               LDEBUG << "1. finish it or cancel it if it was not finished;" << LENDL;
//               LDEBUG << "2. start a new chain or add to ignore if new vertex is a confluent on another chain (begin or continued)" << LENDL;
              Path newHyp(currentHyp);
              // current chain was not finished, we are on a divergent branch
              // current chain elem has to be removed and replaced by vertices elems
              // made from its elements
              if (!( (currentElemChainIdStruct.elemType() == END) ||
                     (currentElemChainIdStruct.elemType() == UNIGRAM) ))
              {
//                 LDEBUG << "current chain was not finished, we are on a divergent branch" << LENDL;
//                 LDEBUG << "current chain elem has to be removed and replaced by vertices elems made from its elements" << LENDL;
                cancelCurrentChain(currentElem, newHyp);
              }
              // now continue as when there was no chain on current vertex
//               LDEBUG << "now continue as when there was no chain on current vertex" << LENDL;
              startWithSeveralChainsOnNewVertex(nextVertex, nextVertexChains, currentHyp);
            }
          }
        }
        else // a chain on current vertex and one (or more) chain(s) to ignore
        {
//           LDEBUG << "a chain on current vertex and one (or more) chain(s) to ignore" << LENDL;
          uint64_t currentElemChainId = currentElem.id();
          ChainIdStruct currentElemChainIdStruct;
          std::set< ChainIdStruct >::const_iterator currentVertexChainsIt, currentVertexChainsIt_end;
          currentVertexChainsIt = currentVertexChains.begin();
          currentVertexChainsIt_end = currentVertexChains.end();
          for (; currentVertexChainsIt != currentVertexChainsIt_end; currentVertexChainsIt++)
          {
            if ( (*currentVertexChainsIt).chainId() == currentElemChainId )
            {
              currentElemChainIdStruct = *currentVertexChainsIt;
              break;
            }
          }
          if (currentVertexChainsIt == currentVertexChainsIt_end)
            throw std::runtime_error("Current elem chain id not found in current elem chains.");

          if (nextVertexChains.empty()) // no chain on next vertex: the current chain does not continue
          {
//             LDEBUG << "no chain on next vertex: the current chain does not continue" << LENDL;
            // current chain was finished, continuing normaly
            if ( (currentElemChainIdStruct.elemType() == END) ||
                 (currentElemChainIdStruct.elemType() == UNIGRAM) )
            {
//               LDEBUG << "current chain was finished, continuing normaly" << LENDL;
              Path newHyp(currentHyp);
              Key newKey(nextVertex, currentKey.chainsToIgnore());
              Elem newElem(nextVertex);
              newHyp.key(newKey);
              newHyp.elems().push_back(newElem);
              newHyp.updateParams(nextMicroCateg, false, true, m_language);
              m_hypsStack.push_front(newHyp);
              if (m_hypsStack.size() > MAXPATHS)
                m_hypsStack.pop_back();
            }
            // current chain was not finished, we are on a divergent branch
            // current chain elem has to be removed and replaced by vertices elems
            // made from its elements
            else
            {
//               LDEBUG << "current chain was not finished, we are on a divergent branch ; current chain elem has to be removed and replaced by vertices elems made from its elements" << LENDL;
              Path newHyp(currentHyp);
              cancelCurrentChain(currentElem, newHyp);
              Key newKey(nextVertex, currentKey.chainsToIgnore());
              newKey.chainsToIgnore().insert(currentElemChainId);
              Elem newElem(nextVertex);
              newHyp.key(newKey);
              newHyp.elems().push_back(newElem);
              newHyp.updateParams(nextMicroCateg, false, true, m_language);
              m_hypsStack.push_front(newHyp);
              if (m_hypsStack.size() > MAXPATHS)
                m_hypsStack.pop_back();
            }
          }
          // one chain on the new vertex
          else if (nextVertexChains.size() == 1)
          {
//             LDEBUG << "one chain on the new vertex" << LENDL;
            // verify if we continue the same chain
            // or finish it and start a new one
            // or finish it and new vertex is a confluent on another chain

            // continue the same chain
            if ( (*(nextVertexChains.begin())).chainId() == currentElem.id() )
            {
//               LDEBUG << "continue the same chain" << LENDL;
              Path newHyp(currentHyp);
              Key newKey(nextVertex);
              Elem& newElem = *(newHyp.elems().rbegin());
              newElem.elems().push_back(nextVertex);
              newHyp.key(newKey);
              newHyp.updateParams(nextMicroCateg, false, false, m_language);
              m_hypsStack.push_front(newHyp);
              if (m_hypsStack.size() > MAXPATHS)
                m_hypsStack.pop_back();
            }
            else
              // 1. finish it or cancel it if it was not finished
              // 2. start a new chain or add to ignore if new vertex is a confluent on another chain (begin or continued)
              // 3. continue without chain if new chain have to be ignored
            {
//               LDEBUG << "does not continue the same chain" << LENDL;
              Path newHyp(currentHyp);
              // current chain was not finished, we are on a divergent branch
              // current chain elem has to be removed and replaced by vertices elems
              // made from its elements
              if (!( (currentElemChainIdStruct.elemType() == END) ||
                     (currentElemChainIdStruct.elemType() == UNIGRAM) ))
              {
//                 LDEBUG << "current chain was not finished, we are on a divergent branch" << LENDL;
                cancelCurrentChain(currentElem, newHyp);
              }
              else
              {
//                 LDEBUG << "current chain was finished" << LENDL;
              }

//               LDEBUG << "now continue as when there was no chain on current vertex" << LENDL;
              // now continue as when there was no chain on current vertex
              Key newKey(nextVertex, currentKey.chainsToIgnore());
              ChainIdStruct nextVertexChain = (*(nextVertexChains.begin()));
              uint64_t nextVertexChainId = nextVertexChain.chainId();
              Elem newElem(nextVertex);
              if ( ( ( nextVertexChain.elemType() == BEGIN ) ||
                     ( nextVertexChain.elemType() == UNIGRAM ) ) &&
                   ( newKey.chainsToIgnore().find(nextVertexChainId) == newKey.chainsToIgnore().end() ) )
              {
                newElem = Elem(nextVertexChainId, 1);
                newElem.elems().push_back(nextVertex);
                newHyp.updateParams(nextMicroCateg, true, false, m_language);
              }
              else //END or PART or chain to ignore
              {
                newKey.chainsToIgnore().insert(( nextVertexChainId));
                newHyp.updateParams(nextMicroCateg, false, true, m_language);
              }
              newHyp.elems().push_back(newElem);
              newHyp.key(newKey);
              m_hypsStack.push_front(newHyp);
              if (m_hypsStack.size() > MAXPATHS)
                m_hypsStack.pop_back();
            }
          }
          // several chains on the new vertex
          else
          {
//             LDEBUG << "several chains on the new vertex" << LENDL;
            // verify if we continue the same chain
            // or finish it and start a new one
            // or finish it and new vertex is a confluent on another chain

            bool currentChainFound = false;
            std::set< uint64_t > nextVertexChainsIdsToIgnore = currentKey.chainsToIgnore();
            std::set< ChainIdStruct >::const_iterator nextVertexChainsIt, nextVertexChainsIt_end;
            nextVertexChainsIt = nextVertexChains.begin();
            nextVertexChainsIt_end = nextVertexChains.end();
            for(; nextVertexChainsIt != nextVertexChainsIt_end; nextVertexChainsIt++)
            {
              if ( (*nextVertexChainsIt).chainId() == currentElem.id() )
                currentChainFound = true;
              else
                nextVertexChainsIdsToIgnore.insert((*nextVertexChainsIt).chainId());
            }
            // continue the same chain
            if (currentChainFound)
            {
//               LDEBUG << "continue the same chain" << LENDL;
              Path newHyp(currentHyp);
              Key newKey(nextVertex, nextVertexChainsIdsToIgnore);
              Elem& newElem = *(newHyp.elems().rbegin());
              newElem.elems().push_back(nextVertex);
              newHyp.key(newKey);
              newHyp.updateParams(nextMicroCateg, false, false, m_language);
              m_hypsStack.push_front(newHyp);
              if (m_hypsStack.size() > MAXPATHS)
                m_hypsStack.pop_back();
            }
            else
              // 1. finish it or cancel it if it was not finished
              // 2. start a new chain or add to ignore if new vertex is a confluent on another chain (begin or continued)
            {
//               LDEBUG << "1. finish it or cancel it if it was not finished ;" << LENDL;
//               LDEBUG << "2. start a new chain or add to ignore if new vertex is a confluent on another chain (begin or continued)" << LENDL;
              Path newHyp(currentHyp);
              // current chain was not finished, we are on a divergent branch
              // current chain elem has to be removed and replaced by vertices elems
              // made from its elements
              if (!( (currentElemChainIdStruct.elemType() == END) ||
                     (currentElemChainIdStruct.elemType() == UNIGRAM) ))
              {
                cancelCurrentChain(currentElem, newHyp);
              }
              // now continue as when there was no chain on current vertex
              startWithSeveralChainsOnNewVertex(nextVertex, nextVertexChains, newHyp);
            }
          }
        }
      }
    }
  }
}

/** Do what is necessary when no chain is continued on the next vertex
  * and there is several chains ont it */
void ChainsDisambiguator::startWithSeveralChainsOnNewVertex(
  LinguisticGraphVertex nextVertex,
  const std::set< ChainIdStruct >& nextVertexChains,
    const Path& currentHyp)
{
//   SADLOGINIT;
//   LDEBUG << "startWithSeveralChainsOnNewVertex: " << nextVertex << LENDL;
  const Key& currentKey = currentHyp.key();
  const LinguisticGraph* graph = m_data->graph();
  CVertexDataPropertyMap dataMap = get(vertex_data, *graph);
  const MorphoSyntacticData* nextData = dataMap[nextVertex];
  LinguisticCode nextMicroCateg(0);
  if (nextData != 0 && !nextData->empty())
  {
    nextMicroCateg = m_microAccessor->readValue(nextData->begin()->properties);
  }
  uint64_t nextVertexChainId = std::numeric_limits<uint64_t>::max();
  std::set< ChainIdStruct >::const_iterator nextVertexChainsIt, nextVertexChainsIt_end;
  nextVertexChainsIt = nextVertexChains.begin();
  nextVertexChainsIt_end = nextVertexChains.end();
  std::set<uint64_t> newChainsToIgnore;
  std::set<uint64_t> nextVertexChainsIds;
  std::set<uint64_t> nextVertexBeginChainsIds;
  for (; nextVertexChainsIt != nextVertexChainsIt_end; nextVertexChainsIt++)
  {
    if ( ( ( (*nextVertexChainsIt).elemType() == BEGIN ) ||
           ( (*nextVertexChainsIt).elemType() == UNIGRAM ) ) &&
         ( currentKey.chainsToIgnore().find(nextVertexChainId) == currentKey.chainsToIgnore().end() ) )
      nextVertexBeginChainsIds.insert((*nextVertexChainsIt).chainId());
    nextVertexChainsIds.insert((*nextVertexChainsIt).chainId());
  }
  if (nextVertexBeginChainsIds.empty())  //no chain begin found
  {
//     LDEBUG << "no chain begin found" << nextVertex << LENDL;
    Key newKey(nextVertex, currentKey.chainsToIgnore());
    Path newHyp(currentHyp);
    newChainsToIgnore = nextVertexChainsIds;
    newKey.chainsToIgnore().insert(newChainsToIgnore.begin(), newChainsToIgnore.end());
    Elem newElem(nextVertex);
    newHyp.elems().push_back(newElem);
    newHyp.key(newKey);
    newHyp.updateParams(nextMicroCateg, false, true, m_language);
    m_hypsStack.push_front(newHyp);
    if (m_hypsStack.size() > MAXPATHS)
      m_hypsStack.pop_back();
  }
  else //chain(s) begin(s) found
  {
//     LDEBUG << "chain(s) begin(s) found" << nextVertex << LENDL;
    std::set<uint64_t>::const_iterator nextVertexBeginChainsIdsIt, nextVertexBeginChainsIdsIt_end;
    nextVertexBeginChainsIdsIt = nextVertexBeginChainsIds.begin();
    nextVertexBeginChainsIdsIt_end = nextVertexBeginChainsIds.end();
    for (; nextVertexBeginChainsIdsIt != nextVertexBeginChainsIdsIt_end; nextVertexBeginChainsIdsIt++)
    {
      std::set<uint64_t> nextVertexChainsIdsToIgnore = nextVertexChainsIds;
      nextVertexChainsIdsToIgnore.erase(*nextVertexBeginChainsIdsIt);

      Key newKey(nextVertex, currentKey.chainsToIgnore());
      newKey.chainsToIgnore().insert(nextVertexChainsIdsToIgnore.begin(), nextVertexChainsIdsToIgnore.end());
      Path newHyp(currentHyp);
      Elem newElem(*nextVertexBeginChainsIdsIt, 1);
      newElem.elems().push_back(nextVertex);
      newHyp.updateParams(nextMicroCateg, true, false, m_language);
      newHyp.elems().push_back(newElem);
      newHyp.key(newKey);
      m_hypsStack.push_front(newHyp);
      if (m_hypsStack.size() > MAXPATHS)
        m_hypsStack.pop_back();
    }
  }
//   LDEBUG << "END startWithSeveralChainsOnNewVertex: " << nextVertex << LENDL;
}

/** simplify the graph to keep only the selected chains path */
void ChainsDisambiguator::applyDisambiguisation()
{
//   SADLOGINIT;
//   LDEBUG << "ChainsDisambiguator::applyDisambiguisation on " << m_completePaths.size() << " complete paths" << LENDL;
  if (m_completePaths.empty()) return;
  std::set< Path > updatedPaths;
  std::multiset< Path >::const_iterator pathsIt, pathsIt_end;
  pathsIt = m_completePaths.begin();
  pathsIt_end = m_completePaths.end();
  for (; pathsIt != pathsIt_end ; pathsIt++)
  {
    Path path = *pathsIt;
    uint64_t nb = computeDepsNb(path);
    path.depsNb(nb);
//     LDEBUG << path << LENDL;
    updatedPaths.insert(path);
  }
  const Path& selectedPath = *(updatedPaths.begin());
//   LDEBUG << "Selected path: " << selectedPath << LENDL;
  std::set< LinguisticGraphVertex > selectedVertices;
  std::list< Elem >::const_iterator it, it_end;
  it = selectedPath.elems().begin();
  it_end = selectedPath.elems().end();
  for (; it != it_end; it++)
  {
    const Elem& elem  = *it;
    if (elem.type() == 0)
      selectedVertices.insert(elem.id());
    else
    {
      std::vector< uint64_t >::const_iterator elit, elit_end;
      elit = elem.elems().begin();
      elit_end = elem.elems().end();
      for (; elit != elit_end; elit++)
      {
        selectedVertices.insert(*elit);
      }
    }
  }
  LinguisticGraph& graph = *(m_data->graph());
  DependencyGraph& depGraph = *(m_data->dependencyGraph());
  std::list< LinguisticGraphVertex > verticesToExplore;
  std::set< LinguisticGraphVertex > scheduledVertices;
  verticesToExplore.push_back(m_srcVertex);
  scheduledVertices.insert(m_srcVertex);

  // clearing unselected vertices
  while (! verticesToExplore.empty() )
  {
    LinguisticGraphVertex currentVertex = verticesToExplore.front();
    verticesToExplore.pop_front();
    LinguisticGraphOutEdgeIt it, it_end;
    boost::tie(it, it_end) = boost::out_edges(currentVertex, graph);
    for (; it != it_end; it++)
    {
      LinguisticGraphVertex nextVertex = target(*it, graph);
      if ( (nextVertex != m_tgtVertex) && (nextVertex != 1) &&
           (scheduledVertices.find(nextVertex) == scheduledVertices.end() ) )
      {
        scheduledVertices.insert(nextVertex);
        verticesToExplore.push_back(nextVertex);
      }
    }
    if (selectedVertices.find(currentVertex) == selectedVertices.end())
    {
//      LDEBUG << "Clearing " << currentVertex << "(dep="<<m_data->depVertexForTokenVertex(currentVertex)<<")"<<LENDL;
      boost::clear_vertex(m_data->depVertexForTokenVertex(currentVertex), depGraph );
      boost::clear_vertex(currentVertex, graph);
      //boost::remove_vertex(currentVertex, graph);
    }
  }

  //removing unselected chains from selected vertices
  VertexChainIdPropertyMap chainsMap = get(vertex_chain_id, graph);
//   EdgeDepChainIdPropertyMap edcipm = get(edge_depchain_id,depGraph);

  it = selectedPath.elems().begin();
  it_end = selectedPath.elems().end();
  for (; it != it_end; it++)
  {
    const Elem& elem  = *it;
    if (elem.type() == 0)
    {
      LinguisticGraphVertex currentVertex = elem.id();
      chainsMap[currentVertex] = std::set< ChainIdStruct >();
//       DependencyGraphVertex currentDepVertex = m_data->depVertexForTokenVertex(currentVertex);
//       DependencyGraphOutEdgeIt dit, dit_end;
//       boost::tie(dit, dit_end) = boost::out_edges(currentDepVertex, depGraph);
//       std::list< LinguisticGraphEdge > edgesToRemove;
//       for (; dit != dit_end; dit++)
//       {
//         DependencyGraphEdge edge = *dit;
//         if ( edcipm[edge].chainId() != UINT_MAX )
//           edgesToRemove.push_back(edge);
//       }
//       std::list< LinguisticGraphEdge >::iterator itr, itr_end;
//       itr = edgesToRemove.begin(); itr_end = edgesToRemove.end();
//       for (; itr != itr_end; itr++)
//       {
//         remove_edge(*itr, depGraph);
//       }
    }
    else
    {
      uint64_t chainId = elem.id();
      LinguisticGraphVertex currentVertex = *(elem.elems().begin());
      std::set< ChainIdStruct >& currentVertexChains = chainsMap[currentVertex];
      ChainIdStruct cisToKeep;
      std::set< ChainIdStruct >::const_iterator currentVertexChainsIt, currentVertexChainsIt_end;
      currentVertexChainsIt = currentVertexChains.begin();
      currentVertexChainsIt_end = currentVertexChains.end();
      for (; currentVertexChainsIt != currentVertexChainsIt_end; currentVertexChainsIt++)
      {
        if ( (*currentVertexChainsIt).chainId() == chainId)
        {
          cisToKeep = *currentVertexChainsIt;
          break;
        }
      }
      if (cisToKeep.chainType() == Common::MediaticData::NO_CHAIN_TYPE)
        throw std::runtime_error("Selected chain not found on current vertex");
      std::vector< uint64_t >::const_iterator elit, elit_end;
      elit = elem.elems().begin();
      elit_end = elem.elems().end();
      for (; elit != elit_end; elit++)
      {
        std::set< ChainIdStruct > newCurrentVertexChains;
        ChainIdStruct cisToKeepCopy = cisToKeep;
        if (elem.elems().size() == 1)
          cisToKeepCopy.elemType(UNIGRAM);
        else if (elit == elem.elems().begin())
          cisToKeepCopy.elemType(BEGIN);
        else if ( (elit+1) == elem.elems().end())
          cisToKeepCopy.elemType(END);
        else
          cisToKeepCopy.elemType(PART);
        newCurrentVertexChains.insert(cisToKeepCopy);
        chainsMap[*elit] = newCurrentVertexChains;

        DependencyGraphVertex currentDepVertex = m_data->depVertexForTokenVertex(*elit);
        DependencyGraphOutEdgeIt dit, dit_end;
        boost::tie(dit, dit_end) = boost::out_edges(currentDepVertex, depGraph);
        std::list< LinguisticGraphEdge > edgesToRemove;
        while (dit != dit_end)
        {
//           DependencyGraphOutEdgeIt dit_copy = dit;
          dit++;
//           DependencyGraphEdge edge = *dit_copy;
/*          if ( ( edcipm[edge].chainId() != UINT_MAX ) &&
               ( edcipm[edge].chainId() != chainId ) )*/
//             edgesToRemove.push_back(edge);
        }
        std::list< LinguisticGraphEdge >::iterator itr, itr_end;
        itr = edgesToRemove.begin(); itr_end = edgesToRemove.end();
        for (; itr != itr_end; itr++)
        {
          remove_edge(*itr, depGraph);
        }
      }
    }
  }
}

uint64_t ChainsDisambiguator::computeDepsNb(const Path& path)
{
  LIMA_UNUSED(path);
  uint64_t nb = 0;
//   const DependencyGraph& depGraph = *(m_data->dependencyGraph());
//   CEdgeDepChainIdPropertyMap edcipm = get(edge_depchain_id,depGraph);
//   std::list< Elem >::const_iterator ite, ite_end;
//   ite = path.elems().begin(); ite_end = path.elems().end();
//   for (; ite != ite_end; ite++)
//   {
//     const Elem& el = *ite;
//     if (el.type() == 0)
//     {
//       DependencyGraphVertex currentDepVertex = m_data->depVertexForTokenVertex(el.id());
//       DependencyGraphOutEdgeIt dit, dit_end;
//       boost::tie(dit, dit_end) = boost::out_edges(currentDepVertex, depGraph);
//       for (; dit != dit_end; dit++)
//       {
//         const DependencyGraphEdge& edge = *dit;
//         if ( edcipm[edge].chainId() == UINT_MAX )
//           nb++;
//       }
//     }
//     else
//     {
//       std::vector< uint64_t >::const_iterator itee, itee_end;
//       itee = el.elems().begin(); itee_end = el.elems().end();
//       for (;  itee != itee_end; itee++)
//       {
//         DependencyGraphVertex currentDepVertex = m_data->depVertexForTokenVertex(*itee);
//         DependencyGraphOutEdgeIt dit, dit_end;
//         boost::tie(dit, dit_end) = boost::out_edges(currentDepVertex, depGraph);
//         for (; dit != dit_end; dit++)
//         {
//           const DependencyGraphEdge& edge = *dit;
//           if ( ( edcipm[edge].chainId() == UINT_MAX ) ||
//                ( edcipm[edge].chainId() == el.id() ) )
//             nb++;
//         }
//       }
//     }
//   }
  return nb;
}

/** elem is volontarily given by copy */
void ChainsDisambiguator::cancelCurrentChain(Elem elem, Path& hyp)
{
  hyp.elems().pop_back();
  std::vector<uint64_t>::const_iterator elemElemsIt, elemElemsIt_end;
  elemElemsIt = elem.elems().begin();
  elemElemsIt_end = elem.elems().end();
  for (; elemElemsIt != elemElemsIt_end; elemElemsIt++)
  {
    Elem newCurrentElem(*elemElemsIt);
    hyp.elems().push_back(newCurrentElem);
    hyp.outChainsWordsNb(hyp.outChainsWordsNb()+1);
    hyp.chainsNb(hyp.chainsNb()-1);
  }
}

Elem& Elem::operator=(const Elem& elem)
{
  m_id = elem.m_id;
  m_type = elem.m_type;
  m_elems = elem.m_elems;
  return *this;
}

Key& Key::operator=(const Key& key)
{
  m_id = key.m_id;
  m_chainsToIgnore = key.m_chainsToIgnore;
  return *this;
}

Path::Path(
  uint64_t id,
  const std::set< uint64_t>& chainsToIgnore,
    unsigned char type,
    uint64_t chainId) :
      m_key(id, chainsToIgnore),
      m_elems(),
      m_idealConjVerbNb(1),
      m_conjVerbNb(0),
      m_outChainsWordsNb(0),
      m_chainsNb(0),
      m_depsNb(0)
{
  if (type == 0)
    m_elems.push_back(Elem(id,type));
  else if ( (type != 0) && (chainId != std::numeric_limits<uint64_t>::max()) )
  {
    Elem elem(chainId,type);
    elem.elems().push_back(id);
    m_elems.push_back(elem);
  }
  else if (type == 1)
    throw std::runtime_error("Chains disambiguation: Chain id not given while creating path beginning by a chain.");
  else
    throw std::runtime_error("Chains disambiguation: Unsupported elem type while creating a Path.");
}

Path::Path(const Path& path) :
    m_key(path.m_key),
    m_elems(path.m_elems),
    m_idealConjVerbNb(path.m_idealConjVerbNb),
    m_conjVerbNb(path.m_conjVerbNb),
    m_outChainsWordsNb(path.m_outChainsWordsNb),
    m_chainsNb(path.m_chainsNb),
    m_depsNb(path.m_depsNb)
{}

Path& Path::operator=(const Path& path)
{
  m_key = path.m_key;
  m_elems = path.m_elems;
  m_idealConjVerbNb = path.m_idealConjVerbNb;
  m_conjVerbNb = path.m_conjVerbNb;
  m_outChainsWordsNb = path.m_outChainsWordsNb;
  m_chainsNb = path.m_chainsNb;
  m_depsNb = path.m_depsNb;
  return *this;
}

bool Path::operator<(const Path& path) const
{
  if ( ( ( m_idealConjVerbNb == m_conjVerbNb ) && ( path.m_idealConjVerbNb == path.m_conjVerbNb ) ) ||
       ( ( m_idealConjVerbNb != m_conjVerbNb ) && ( path.m_idealConjVerbNb != path.m_conjVerbNb ) ) )
  {
    if ( m_outChainsWordsNb < path.m_outChainsWordsNb)
      return true;
    else if ( m_outChainsWordsNb > path.m_outChainsWordsNb)
      return false;
    else
    {
      if ( m_chainsNb < path.m_chainsNb)
        return true;
      else if ( m_chainsNb > path.m_chainsNb)
        return false;
      else
        return ( m_depsNb > path.m_depsNb );
    }
  }
  else
    return ( m_idealConjVerbNb == m_conjVerbNb );
}

void Path::updateParams(
  const LinguisticCode& microCategory,
  bool incrChainsNb,
  bool incrOutChainsWordsNb,
  MediaId language)
{
  if (static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).isAConjugatedVerb(microCategory))
    m_conjVerbNb++;
  else if (static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).isAPropositionIntroductor(microCategory))
    m_idealConjVerbNb++;

  if (incrChainsNb) m_chainsNb++;
  if (incrOutChainsWordsNb) m_outChainsWordsNb++;
}

std::ostream& operator<<(std::ostream &os, const Key& k)
{
  os << k.id() << " ( ";
  std::set< uint64_t >::const_iterator it, it_end;
  it = k.chainsToIgnore().begin(); it_end = k.chainsToIgnore().end();
  for (; it != it_end; it++)
  {
    os << *it << " ";
  }
  os << ")";
  return os;
}

std::ostream& operator<<(std::ostream &os, const Elem& e)
{
  if (e.type() != 0)
  {
    os << "[" << e.id() << ": ";
    std::vector< uint64_t >::const_iterator it, it_end;
    it = e.elems().begin(); it_end = e.elems().end();
    for (; it != it_end; it++)
    {
      os << *it << " ";
    }
    os << "]";
  }
  else
    os << e.id();
  return os;
}

std::ostream& operator<<(std::ostream &os, const Path& p)
{
  os << p.key() << " | ";
  std::list< Elem >::const_iterator it, it_end;
  it = p.elems().begin(); it_end = p.elems().end();
  for (; it != it_end; it++)
  {
    os << *it << " ";
  }
  os << " | " << p.idealConjVerbNb() << "/" << p.conjVerbNb() << " "
  << p.outChainsWordsNb() << " " << p.chainsNb() << " " << p.depsNb() << std::endl;
  return os;
}
} // closing namespace SyntacticAnalysis
} // closing namespace LinguisticProcessing
} // closing namespace Lima
