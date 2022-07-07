// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                              *
 *                                                                         *
 ***************************************************************************/
#include "AnalysisGraph.h"

#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "linguisticProcessing/client/LinguisticProcessingException.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "common/MediaticData/mediaticData.h"
#include "common/Data/strwstrtools.h"

#include <queue>

using namespace Lima::Common::MediaticData;
using namespace Lima::Common::AnnotationGraphs;

namespace Lima {

namespace LinguisticProcessing {

namespace LinguisticAnalysisStructure {

//***********************************************************************
// constructors
//***********************************************************************
AnalysisGraph::AnalysisGraph(const std::string& graphId,MediaId language,bool deleteTokenWhenDestroyed,bool deleteDataWhenDestroyed):
    m_graph(0),
    m_firstVertex(0),
    m_lastVertex(0),
    m_deleteTokenWhenDestroyed(deleteTokenWhenDestroyed),
    m_deleteDataWhenDestroyed(deleteDataWhenDestroyed),
    m_language(language),
    m_graphId(graphId)
{
  createGraph();
}

AnalysisGraph::AnalysisGraph(const std::string& graphId,MediaId language,bool deleteTokenWhenDestroyed,bool deleteDataWhenDestroyed,const AnalysisGraph& anagraph) :
  m_graph(new LinguisticGraph(*(anagraph.m_graph))),
  m_firstVertex(anagraph.m_firstVertex),
  m_lastVertex(anagraph.m_lastVertex),
  m_deleteTokenWhenDestroyed(deleteTokenWhenDestroyed),
  m_deleteDataWhenDestroyed(deleteDataWhenDestroyed),
  m_language(language),
  m_graphId(graphId)
{}


AnalysisGraph::AnalysisGraph(const AnalysisGraph& g):
    AnalysisData(),
    m_graph(g.m_graph),
    m_firstVertex(g.m_firstVertex),
    m_lastVertex(g.m_lastVertex),
    m_deleteTokenWhenDestroyed(g.m_deleteTokenWhenDestroyed),
    m_deleteDataWhenDestroyed(g.m_deleteDataWhenDestroyed),
    m_language(g.m_language)
{}

//***********************************************************************
// destructor
//***********************************************************************
AnalysisGraph::~AnalysisGraph()
{
  deleteGraph();
}

//***********************************************************************
// assignment operator
//***********************************************************************
AnalysisGraph& AnalysisGraph::operator = (const AnalysisGraph& g)
{
  if (this != &g)
  {
    m_graph=g.m_graph;
    m_firstVertex=g.m_firstVertex;
    m_lastVertex=g.m_lastVertex;
    m_language=g.m_language;
  }
  return *this;
}

//***********************************************************************
// member functions
//***********************************************************************
void AnalysisGraph::createGraph()
{

  if (Common::MediaticData::MediaticData::single().releaseStringsPool())
    Common::MediaticData::MediaticData::changeable().stringsPool(m_language).registerUser(this);

  if (m_graph == 0)
  {
    m_graph = new LinguisticGraph();
  }

  if (num_vertices(*m_graph) != 0)
  {
    throw LinguisticProcessingException("AnalysisGraph::createGraph() - graph is not empy");
  }

  VertexTokenPropertyMap tokenMap = get( vertex_token, *m_graph );
  VertexDataPropertyMap dataMap = get( vertex_data, *m_graph );

  // add first vertex
  LinguisticGraphVertex vertex1 = add_vertex(*m_graph);
  tokenMap[vertex1] = 0;
  dataMap[vertex1] = 0;
  m_firstVertex = vertex1;

  // add last vertex
  LinguisticGraphVertex vertex2 = add_vertex(*m_graph);
  tokenMap[vertex2] = 0;
  dataMap[vertex2] = 0;
  m_lastVertex = vertex2;

  // add edge between first and last vertex
  bool b;
  LinguisticGraphEdge beginEndEdge;
  boost::tie(beginEndEdge, b) = add_edge(vertex1, vertex2, *m_graph);
  if (!b)
  {
    throw LinguisticProcessingException("AnalysisGraph::createGraph - could not bind beginEndEdge");
  }
}

void AnalysisGraph::deleteGraph()
{
#ifdef DEBUG_LP
  LASLOGINIT;
  LDEBUG << "deleteGraph";
#endif
  if (m_graph == 0) return;

  if (m_deleteTokenWhenDestroyed)
  {
    LinguisticGraphVertexIt it, it_end;
    VertexTokenPropertyMap tokenMap = get( vertex_token, *m_graph );

    std::set<Token*> deletedToken;

    boost::tie(it, it_end) = vertices(*m_graph);
    for (; it != it_end; it++)
    {
      Token* ft = tokenMap[*it];
      if ( ft != 0 && (deletedToken.find(ft) == deletedToken.end()) &&
          ( (*it) != firstVertex() ) && ( (*it) != lastVertex() ) )
      {
        delete ft;
        deletedToken.insert(ft);
      }
      tokenMap[*it] = 0;
    }
  }
  if (m_deleteDataWhenDestroyed)
  {
    LinguisticGraphVertexIt it, it_end;
    VertexDataPropertyMap dataMap = get( vertex_data, *m_graph );

    std::set<MorphoSyntacticData*> deletedData;

    boost::tie(it, it_end) = vertices(*m_graph);
    for (; it != it_end; it++)
    {
      MorphoSyntacticData* data = dataMap[*it];
      if ( data != 0 && (deletedData.find(data) == deletedData.end()) &&
          ( (*it) != firstVertex() ) && ( (*it) != lastVertex() ) )
      {
        delete data;
        deletedData.insert(data);
      }
      dataMap[*it] = 0;
    }
  }

  delete m_graph;
  m_graph = 0;

  if (Common::MediaticData::MediaticData::single().releaseStringsPool())
    Common::MediaticData::MediaticData::changeable().stringsPool(m_language).unregisterUser(this);

}

LinguisticGraphVertex AnalysisGraph::nextMainPathVertex(
  LinguisticGraphVertex start,
  const Common::PropertyCode::PropertyAccessor& microAccessor,
  const std::list<LinguisticCode> microFilters,
  LinguisticGraphVertex end)
{
#ifdef DEBUG_LP
  LASLOGINIT;
#endif
  /*
   * Algorithm: we're using a Breadth First Search and keep track of the
   * "thickness" of the lattice, and only stop if both condition apply:
   *  1/ the thickness is 1, meaning that every path goes through this node
   *  2/ the node is in microFilters (eg. a full stop in english)
   *     OR the node has t_sentence_break tokenization status
   */
  std::set<LinguisticGraphVertex> visited;
  LinguisticGraphOutEdgeIt outItr,outItrEnd;
  LinguisticGraphInEdgeIt inItr,inItrEnd;

  std::queue<LinguisticGraphVertex,std::list<LinguisticGraphVertex> > toVisit;

  // initialize
  size_t accumulator=out_degree(start,*m_graph);
  boost::tie (outItr,outItrEnd) = out_edges(start,*m_graph);
  for (;outItr!=outItrEnd;outItr++)
  {
    toVisit.push(target(*outItr,*m_graph));
  }

  VertexTokenPropertyMap tokenMap = get( vertex_token, *m_graph );
  // search
  while (!toVisit.empty())
  {
    LinguisticGraphVertex current=toVisit.front();
    toVisit.pop();
    visited.insert(current);
    if (current==end)
    {
      return end;
    }
    Token* ft = tokenMap[current];

    accumulator-=in_degree(current,*m_graph);
    if (accumulator==0)
    {
      // check unique category only if accumulator is 0
      MorphoSyntacticData* msd=get(vertex_data,*m_graph,current);
      if (msd!=0 && msd->hasUniqueMicro(microAccessor,microFilters))
      {
#ifdef DEBUG_LP
        LDEBUG << "AnalysisGraph::nextMainPathVertex micro, return" << current;
#endif
        return current;
      }
      if (ft && ft->status().getStatus() == T_SENTENCE_BRK)
      {
#ifdef DEBUG_LP
        LDEBUG << "AnalysisGraph::nextMainPathVertex sentence break, return" << current;
#endif
        return current;
      }
    }
    accumulator+=out_degree(current,*m_graph);

    boost::tie (outItr,outItrEnd) = out_edges(current,*m_graph);
    if (outItr==outItrEnd)
    {
      LASLOGINIT;
      LERROR << "no next vertex in graph whereas current vertex is not last vertex !!";
      throw std::runtime_error("no next vertex in graph whereas current vertex is not last vertex !!");
    }
    for (;outItr!=outItrEnd;outItr++)
    {
      // Must Check if all predecessors have already been visited
      LinguisticGraphVertex next=target(*outItr,*m_graph);
      boost::tie(inItr,inItrEnd) = in_edges(next,*m_graph);
      bool visitable=true;
      for (;inItr!=inItrEnd;inItr++)
      {
        if (visited.find(source(*inItr,*m_graph))==visited.end())
        {
          visitable=false;
          break;
        }
      }
      if (visitable)
      {
        toVisit.push(next);
      }
    }
  }

  // reach this end if file doesn't end with a specified categ.
  return end;
}


/**
  * @brief Finds the next vertex after the input vertex that:
  *        1. is on a subgraph with no parallel branch ; <br>
  *        2. is a punctuation (macro category is "ponctu forte")
  *        This method should be called <b>after</b> the disambiguation such
  *        that the VertexWordProperty is filled.
  * @param v @b IN <I>LinguisticGraphVertex\&</I>
  *        The vertex from which to start the search
  * @param g @b IN <I>LinguisticGraph\&</I>
  *        The graph containing @ref v
  * @return <I>LinguisticGraphVertex</I>
  *         The searched vertex or the last one if there is no such vertex
  */

LinguisticGraphVertex AnalysisGraph::nextChainsBreakFrom(
  const LinguisticGraphVertex& v,
  const Common::PropertyCode::PropertyAccessor& macroAccessor,
  const LinguisticCode& ponctu,
  const Common::PropertyCode::PropertyAccessor& microAccessor,
  LinguisticGraphVertex& nextSentenceBreak)
{
#ifdef DEBUG_LP
  LASLOGINIT;
#endif

  LinguisticGraphVertex current = v;
  size_t accumulator=out_degree(current,*m_graph);
  while (true)
  {
    LinguisticGraphOutEdgeIt it, it_end;
    boost::tie (it, it_end) = out_edges(current, *m_graph);

    if (it == it_end)
      return m_lastVertex;
    if ( (source((*it), *m_graph) == m_firstVertex) && (target((*it), *m_graph) == m_lastVertex) )
    {
      it++;
      if (it == it_end)
        return m_lastVertex;
    }

    LinguisticGraphVertex next = target((*it), *m_graph);
    if (next == m_lastVertex || next==nextSentenceBreak)
    {
      if (next != nextSentenceBreak)
      {
        LASLOGINIT;
        LERROR << "In nextChainsBreakFrom: went beyond next sentence break " << nextSentenceBreak;
        LERROR << "   returning graph's last vertex " << next;
      }
#ifdef DEBUG_LP
      LDEBUG << "Next chains break is: " << next;
#endif
      return next;
    }
    accumulator-=in_degree(next,*m_graph);
    VertexDataPropertyMap dataMap = get( vertex_data, *m_graph );
    MorphoSyntacticData* msd =dataMap[next];
    if ( (accumulator == 0) && (msd->countValues(microAccessor) == 1) )
    {
      LinguisticCode macro = NONE_1;

      /*if (!tok->morphoSyntacticData().isEmpty())
      {
        std::pair< std::list< WordForm >::const_iterator, std::list< WordForm >::const_iterator > basePair = tok->morphoSyntacticData().base();
        std::pair< std::list< WordForm >::const_iterator, std::list< WordForm >::const_iterator > hyphensPair = tok->morphoSyntacticData().hyphens();
        std::pair< WordFormProperties::const_iterator, WordFormProperties::const_iterator > defaultsPair = tok->morphoSyntacticData().properties();
        if (basePair.first != basePair.second)
        {
          macro = macroAccessor.readValue(*((*(basePair.first)).properties().first));
        }
        else if (hyphensPair.first !=hyphensPair.second)
        {
          macro = macroAccessor.readValue(*((*(hyphensPair.first)).properties().first));
        }
        else if (defaultsPair.first != defaultsPair.second)
        {
          macro = macroAccessor.readValue(*(defaultsPair.first));
        }
        else
          macro = NONE_1;
      }*/
      /* �valider : le code ci-dessus est remplac�par :  */
      if (msd->begin() != msd->end()) {
        macro=macroAccessor.readValue(msd->begin()->properties);
      }

      /*      else if (hyphensPair.first != hyphensPair.second)
            {
              const Token& alt = (*((tok-> getOrthographicAlternatives()).begin()));
              macro = ((*(alt.morphoSyntacticData().base().first)).properties().first)->code(MACRO);
            }
      else
        macro = NONE_1;*/

      if ( macro == ponctu)
        return next;
    }
    accumulator+=out_degree(next,*m_graph);
    if (next == current)
    {
      LASLOGINIT;
      LERROR << "In nextChainsBreakFrom: cannot go beyond " << current;
      return m_lastVertex;
    }
    current = next;
  }
}


void AnalysisGraph::populateAnnotationGraph(
    AnnotationData* annotData,
    const std::string& src)
{
  LinguisticGraphVertexIt it, it_end;

  boost::tie(it, it_end) = vertices(*m_graph);
  for (; it != it_end; it++)
  {
    if (annotData->matches(src, *it, "annot").empty())
    {
      AnnotationGraphVertex agv =  annotData->createAnnotationVertex();
      annotData->addMatching(src, *it, "annot", agv);
      annotData->annotate(agv, Common::Misc::utf8stdstring2limastring(src), static_cast< uint64_t >(*it));
    }
  }

}


}

}

}
