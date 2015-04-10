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
 * @file       recognizerData.cpp
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Tue Jan 25 2005
 * @version    $Id$
 * copyright   Copyright (C) 2005-2012 by CEA LIST
 *
 ***********************************************************************/

#include "recognizerData.h"
#include <queue>

using namespace Lima::LinguisticProcessing::Automaton;
using namespace std;

namespace Lima
{
namespace LinguisticProcessing
{
namespace ApplyRecognizer
{

//***********************************************************************
// constructors
//***********************************************************************
RecognizerData::RecognizerData():
    AnalysisData(),
    m_verticesToRemove(),
    m_resultData(0),
    m_currentSentence(0),
    m_nextVertices(),
    m_entityFeatures()
{}

RecognizerData::RecognizerData(const RecognizerData& d):
    AnalysisData(d),
    m_verticesToRemove(d.m_verticesToRemove),
    m_resultData(d.m_resultData),
    m_currentSentence(d.m_currentSentence),
    m_nextVertices(d.m_nextVertices),
    m_entityFeatures(d.m_entityFeatures)
{}

//***********************************************************************
// destructor
//***********************************************************************
RecognizerData::~RecognizerData()
{
}

//***********************************************************************
// assignment operator
//***********************************************************************
RecognizerData& RecognizerData::operator = (const RecognizerData& d)
{
  if (this != &d)
  {
    AnalysisData::operator=(d);
    m_verticesToRemove=d.m_verticesToRemove;
    m_resultData=d.m_resultData;
    m_currentSentence=d.m_currentSentence;
    m_nextVertices = d.m_nextVertices;
    m_entityFeatures = d.m_entityFeatures;
  }
  return *this;
}

//***********************************************************************
// member functions
//***********************************************************************
void RecognizerData::
nextSentence() {
  m_currentSentence++;
  if (m_resultData!=0) {
    m_resultData->push_back(std::vector< Automaton::RecognizerMatch >());
  }
}

bool RecognizerData::
matchOnRemovedVertices(const RecognizerMatch& result) const
{
  for (RecognizerMatch::const_iterator m(result.begin());
       m!=result.end(); m++)
  {
    if ((*m).isKept())
    {
      if (m_verticesToRemove.find((*m).getVertex()) !=
          m_verticesToRemove.end())
      {
        return true;
      }
    }
  }
  return false;
}

void RecognizerData::storeVerticesToRemove(const RecognizerMatch& result,
    LinguisticGraph* graph)
{
  //   APPRLOGINIT;
  //   LDEBUG << "RecognizerData: storing vertices to remove";

  for (RecognizerMatch::const_iterator match(result.begin());
       match!=result.end(); match++)
  {
    if ((*match).isKept())
    {
      //       LDEBUG << "  storing "<< (*match).getVertex() << " to be removed";
      // store this vertex to remove
      m_verticesToRemove.insert((*match).getVertex());

      // check previous vertices
      std::queue<LinguisticGraphVertex> verticesToCheck;
      verticesToCheck.push((*match).getVertex());
      while (! verticesToCheck.empty())
      {
        // check previous vertices to see if they have only this vertex
        // as target : if it is the case, remove them also
        LinguisticGraphInEdgeIt it_begin,it_end;
        boost::tie(it_begin,it_end)=in_edges(verticesToCheck.front(),*graph);
        for (LinguisticGraphInEdgeIt it(it_begin); it!=it_end; it++)
        {
          LinguisticGraphVertex previousVertex=source(*it,*graph);
          //           LDEBUG << "  checking if "<< previousVertex << " should be removed also";
          bool vertexToRemove(false);
          if (out_degree(previousVertex,*graph)==1)
          {
            vertexToRemove=true;
          }
          else
          {
            // test if all following vertices are already to be removed
            LinguisticGraphOutEdgeIt it_out_begin,it_out_end;
            boost::tie(it_out_begin,it_out_end)=out_edges(previousVertex,*graph);
            vertexToRemove=true;
            for (LinguisticGraphOutEdgeIt it_out(it_out_begin); it_out!=it_out_end; it_out++)
            {
              if (m_verticesToRemove.find(target(*it_out,*graph))==
                  m_verticesToRemove.end())
              {
                vertexToRemove=false;
                break;
              }
            }
          }
          if (vertexToRemove &&
              m_verticesToRemove.find(previousVertex)==
              m_verticesToRemove.end())
          {
            //             LDEBUG << "    yes";
            m_verticesToRemove.insert(previousVertex);
            verticesToCheck.push(previousVertex);
          }
          //           else
          //           {
          //             LDEBUG << "    no";
          //           }
        }
        verticesToCheck.pop();
      }

      // check next vertices
      verticesToCheck.push((*match).getVertex());
      while (! verticesToCheck.empty())
      {
        // check next vertices to see if they have only this vertex
        // as source : if it is the case, remove them also
        LinguisticGraphOutEdgeIt it_begin,it_end;
        boost::tie(it_begin,it_end)=out_edges(verticesToCheck.front(),*graph);
        for (LinguisticGraphOutEdgeIt it(it_begin); it!=it_end; it++)
        {
          LinguisticGraphVertex nextVertex=target(*it,*graph);
          bool vertexToRemove(false);
          if (in_degree(nextVertex,*graph)==1)
          {
            vertexToRemove=true;
          }
          else
          {// test if all preceding vertices are already to be removed
            vertexToRemove=true;
            LinguisticGraphInEdgeIt it_in_begin,it_in_end;
            boost::tie(it_in_begin,it_in_end)=in_edges(nextVertex,*graph);
            for (LinguisticGraphInEdgeIt it_in(it_in_begin); it_in!=it_in_end; it_in++)
            {
              if (m_verticesToRemove.find(source(*it_in,*graph))== m_verticesToRemove.end())
              {
                vertexToRemove=false;
                break;
              }
            }
          }
          if (vertexToRemove &&
              m_verticesToRemove.find(nextVertex)==
              m_verticesToRemove.end())
          {
            m_verticesToRemove.insert(nextVertex);
            verticesToCheck.push(nextVertex);
          }
        }
        verticesToCheck.pop();
      }
    }
  }
}

void RecognizerData::removeVertices(AnalysisContent& analysis) const
{
  LinguisticAnalysisStructure::AnalysisGraph* anagraph=
    static_cast<LinguisticAnalysisStructure::AnalysisGraph*>(analysis.getData(m_resultData->getGraphId()));

  // remove vertices and edges in reverse order, so that
  // it does not affect the reordering of vertex numbers in
  // the graph
  APPRLOGINIT;
  LDEBUG << "RecognizerData: removing vertices";
  LinguisticGraph& g=*(anagraph->getGraph());
  for (set<LinguisticGraphVertex>::const_reverse_iterator
       it=m_verticesToRemove.rbegin();
       it!=m_verticesToRemove.rend(); it++)
  {
    LDEBUG << "  clearing vertex " << *it;
    clear_vertex(*it,g);
    // remove FullToken;
    //Data::FullToken* token=get(vertex_ling,g,*it);
    //     LDEBUG << "Idiomatic alternatives: removing vertex " << *it
    //            << "(" << *token << ")";
    //delete token;
  }

}

void RecognizerData::addResult(const Automaton::RecognizerMatch& result)
{
  if (m_resultData==0) {
  APPRLOGINIT;
  LERROR << "RecognizerData: cannot add result: missing data";
    return;
  }
  m_resultData->insert(result,m_currentSentence);
}

void RecognizerData::removeEdges(AnalysisContent& analysis)
{
  APPRLOGINIT;
  LDEBUG << "RecognizerData: removing edges to remove";
  LinguisticAnalysisStructure::AnalysisGraph* anagraph=
    static_cast<LinguisticAnalysisStructure::AnalysisGraph*>(analysis.getData(m_resultData->getGraphId()));
  LinguisticGraph& g=*(anagraph->getGraph());
  std::set< std::pair<LinguisticGraphVertex, LinguisticGraphVertex> >::const_iterator it, it_end;
  it = m_edgesToRemove.begin(); it_end = m_edgesToRemove.end();
  for (; it != it_end; it++)
  {
    LDEBUG << "RecognizerData::removeEdges removing edge " << (*it).first << " - " << (*it).second;
    boost::remove_edge((*it).first,(*it).second, g);
    clearUnreachableVertices(analysis, (*it).first);
    clearUnreachableVertices(analysis, (*it).second);
  }
  m_edgesToRemove.clear();
}

void RecognizerData::setEdgeToBeRemoved(AnalysisContent& analysis, LinguisticGraphEdge e)
{
  //   APPRLOGINIT;
  //   LDEBUG << "RecognizerData: setting edge "<<e<<" to be removed";
  LinguisticAnalysisStructure::AnalysisGraph* anagraph=
    static_cast<LinguisticAnalysisStructure::AnalysisGraph*>(analysis.getData(m_resultData->getGraphId()));
  LinguisticGraph& g=*(anagraph->getGraph());

  std::pair<LinguisticGraphVertex, LinguisticGraphVertex> p = std::make_pair(source(e,g),target(e,g));
  m_edgesToRemove.insert(p);
}

bool RecognizerData::isEdgeToBeRemoved(LinguisticGraphVertex s, LinguisticGraphVertex t) const
{
  return (m_edgesToRemove.find(std::make_pair(s,t)) != m_edgesToRemove.end());
}


void RecognizerData::clearUnreachableVertices(
  AnalysisContent& analysis,
  LinguisticGraphVertex from,
  LinguisticGraphVertex to,
  std::set< std::pair<LinguisticGraphVertex, LinguisticGraphVertex > >& storedEdges)
{
  APPRLOGINIT;
  LDEBUG << "RecognizerData: clearing unreachable vertices from " << from << " and to " << to;
  std::deque< std::deque< LinguisticGraphVertex > > paths;
  std::deque< LinguisticGraphVertex > current;
  std::set< std::pair<LinguisticGraphVertex, LinguisticGraphVertex > > validated;

  LinguisticAnalysisStructure::AnalysisGraph* anagraph=
    static_cast<LinguisticAnalysisStructure::AnalysisGraph*>(analysis.getData(m_resultData->getGraphId()));
  LinguisticGraph& g=*(anagraph->getGraph());

  current.push_back(from);
  paths.push_back(current);
  while (!paths.empty())
  {
    current = paths.front();
    paths.pop_front();
    if (current.empty()) continue;
    LinguisticGraphOutEdgeIt it_out,it_out_end;
    boost::tie(it_out,it_out_end)=out_edges(current.back(),g);
    if (it_out == it_out_end)
    { // current back vertex will not be reachable, remove edges from current
      // path iff not in storedEdges and not last vertex
      LinguisticGraphVertex tgt = current.back();
      if (tgt == 1) continue;
      current.pop_back();
      while (!current.empty())
      {
        LinguisticGraphVertex src = current.back();
        current.pop_back();
        std::pair< LinguisticGraphVertex, LinguisticGraphVertex > p = std::make_pair(src,tgt);
        if (storedEdges.find(p) == storedEdges.end())
        {
          LDEBUG << "RecognizerData::clearUnreachableVertices removing edge " << src << " -> " << tgt;
          remove_edge(edge(src,tgt,g).first,g);
        }
        tgt = src;
      }
    }
    else
    {
      for (; it_out != it_out_end; it_out++)
      {
        std::pair< LinguisticGraphVertex, LinguisticGraphVertex > p = std::make_pair(source(*it_out,g),source(*it_out,g));
        if ( (target(*it_out,g) == to)
             || (validated.find(p) != validated.end()) )
        {
          validated.insert(p);
          LinguisticGraphVertex tgt = current.back();
          current.pop_back();
          while (!current.empty())
          {
            LinguisticGraphVertex src = current.back();
            current.pop_back();
            validated.insert(std::make_pair(src,tgt));
            tgt = src;
          }
        }
        else
        {
          std::deque< LinguisticGraphVertex > newpath = current;
          newpath.push_back(target(*it_out,g));
          paths.push_front(newpath);
        }
      }
    }
  }
}


void RecognizerData::clearUnreachableVertices(
  AnalysisContent& analysis,
  LinguisticGraphVertex from)
{
  APPRLOGINIT;
  LDEBUG << "RecognizerData: clearing unreachable vertices from " << from;

  LinguisticAnalysisStructure::AnalysisGraph* anagraph=
    static_cast<LinguisticAnalysisStructure::AnalysisGraph*>(analysis.getData(m_resultData->getGraphId()));
  LinguisticGraph& g=*(anagraph->getGraph());

  std::queue<LinguisticGraphVertex> verticesToCheck;
  verticesToCheck.push( from );
  while (! verticesToCheck.empty() )
  {
    LDEBUG << "    vertices to check size = " << verticesToCheck.size();
    LinguisticGraphVertex v = verticesToCheck.front();
    verticesToCheck.pop();
    bool toClear = false;
    LDEBUG << "  out degree of " << v << " is " << out_degree(v, g);
    if (out_degree(v, g) == 0 && v != anagraph->lastVertex())
    {
      toClear = true;
      LinguisticGraphInEdgeIt it,it_end;
      boost::tie(it,it_end)=in_edges(v,g);
      for (; it!=it_end; it++)
      {
          verticesToCheck.push(source(*it,g));
      }
    }
    LDEBUG << "  in degree of " << v << " is " << in_degree(v, g);
    if (in_degree(v, g) == 0 && v != anagraph->firstVertex())
    {
      toClear = true;
      LinguisticGraphOutEdgeIt it,it_end;
      boost::tie(it,it_end)=out_edges(v,g);
      for (; it!=it_end; it++)
      {
          verticesToCheck.push(target(*it,g));
      }
    }
    if (toClear)
    {
      LDEBUG << "  clearing vertex " << v;
      clear_vertex(v,g);
    }
  }
}


void RecognizerData::setResultData(RecognizerResultData* data)
{
  m_resultData=data;
}

void RecognizerData::deleteResultData() 
{
  if (m_resultData != 0) {
    delete m_resultData;
  }
}

//**********************************************************************
// use also this AnalysisData to store Entity Features

void RecognizerData::clearEntityFeatures()
{
  m_entityFeatures.clear();
}

//**********************************************************************
// Data to store the results

RecognizerResultData::
RecognizerResultData(const std::string& sourceGraph):
    AnalysisData(),
    std::vector<std::vector< Automaton::RecognizerMatch > >(),
    m_graphId(sourceGraph)
{
  // empty result contains one empty vector
  // (for first sentence or whole text)
  push_back(std::vector< Automaton::RecognizerMatch >());
}

RecognizerResultData::
RecognizerResultData(const RecognizerResultData& d):
    AnalysisData(d),
    std::vector<std::vector< Automaton::RecognizerMatch > >(d),
    m_graphId(d.m_graphId)
{}

RecognizerResultData::~RecognizerResultData()
{}

RecognizerResultData& RecognizerResultData::
operator = (const RecognizerResultData& d)
{
  if (this != &d)
  {
    AnalysisData::operator=(d);
    std::vector<std::vector< Automaton::RecognizerMatch > >::operator=(d);
    m_graphId=d.m_graphId;
  }
  return *this;
}

void RecognizerResultData::
insert(const RecognizerMatch& m,
       const uint64_t sentenceId)
{
  if (sentenceId>= size()) {
    APPRLOGINIT;
    LERROR << "RecognizerResultData: try to access data oustide of vector (sentenceId=" << sentenceId << ",size=" << size() << ")";
    return;
  }
  (*this)[sentenceId].push_back(m);
}


} // end namespace
} // end namespace
} // end namespace
