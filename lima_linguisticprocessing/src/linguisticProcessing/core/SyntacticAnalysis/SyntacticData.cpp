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
/** @brief       Data used for the syntactic analyzis of texts
  *
  * @file        SyntacticData.cpp
  * @author      Gael de Chalendar (Gael.de-Chalendar@cea.fr) 

  *              Copyright (c) 2003 by CEA
  * @date        Created on Oct, 1 2003
  * @version     $Id$
  *
  */

#include "SyntacticData.h"
#include "SyntagmaticMatrix.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/Token.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticData.h"

using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

namespace Lima {
namespace LinguisticProcessing {
namespace SyntacticAnalysis {


SyntacticData::SyntacticData(
        AnalysisGraph* anagraph,
        const SyntagmDefStruct* matrices) :
    m_anagraph(anagraph),
    m_graph(anagraph->getGraph()),
    m_depGraph(0),
    m_matrices(matrices),
    m_mapping(),
    m_relations(),
    m_ownedTokens(),
    m_ownedMorphData(),
    m_relationStoredForSelectionalConstraint(0,0,0)
{
//     SALOGINIT;
//     LDEBUG << "######## new SyntacticData !!!!" << LENDL;
}

SyntacticData::~SyntacticData()
{
    delete m_depGraph;
    m_depGraph = 0;

  std::vector<Token*>::iterator itTokens, itTokens_end;
  itTokens = m_ownedTokens.begin(); itTokens_end = m_ownedTokens.end();
  for (; itTokens != itTokens_end; itTokens++)
  {
    delete *itTokens;
  }

  std::vector<MorphoSyntacticData*>::iterator itMorphData, itMorphData_end;
  itMorphData = m_ownedMorphData.begin(); itMorphData_end = m_ownedMorphData.end();
  for (; itMorphData != itMorphData_end; itMorphData++)
  {
    delete *itMorphData;
  }
}

/**
  * @brief This method is used to setup the search of the dependency relations
  *        when the PoS tagging is done and thus the number of vertices will not
  *        change anymore in the morphosyntactic graph. It creates a dependency
  *        graph with the same number of nodes than in the morphosyntactic graph
  *        and a mapping expressing the bijection between the nodes of both
  *        graphs.
  */
void SyntacticData::setupDependencyGraph()
{
//     SALOGINIT;
//     LDEBUG << "Calling setupDependencyGraph" << LENDL;
    if (m_depGraph != 0)
        delete m_depGraph;
    m_depGraph = new DependencyGraph(num_vertices(*(m_anagraph->getGraph())));
    m_mapping.first.clear();
    m_mapping.second.clear();

    LinguisticGraphVertexIt itg, itg_end;
    boost::tie(itg, itg_end) = vertices(*m_graph);
    DependencyGraphVertexIt itd, itd_end;
    boost::tie(itd, itd_end) = vertices(*m_depGraph);

    while (itg != itg_end)
    {
        m_mapping.first.insert(std::make_pair(*itg, *itd));
        m_mapping.second.insert(std::make_pair(*itd, *itg));
        itg++; itd++;
    }
}


/**
  * @brief This method is used to setup the search of the dependency relations
  *        when the PoS tagging is done and thus the number of vertices will not
  *        change anymore in the morphosyntactic graph. It creates a dependency
  *        graph with the same number of nodes than in the morphosyntactic graph
  *        and a mapping expressing the bijection between the nodes of both
  *        graphs.
  */
std::pair<LinguisticGraphVertex, DependencyGraphVertex> SyntacticData::addVertex()
{
//     SALOGINIT;
//     LDEBUG << "Calling SyntacticData::addVertex" << LENDL;

    LinguisticGraphVertex morph = add_vertex(*m_graph);
    DependencyGraphVertex synt = add_vertex(*m_depGraph);
    m_mapping.first.insert(std::make_pair(morph, synt));
    m_mapping.second.insert(std::make_pair(synt, morph));
    return std::make_pair(morph, synt);
}


DependencyGraphVertex SyntacticData::depVertexForTokenVertex(const LinguisticGraphVertex& v) const
{
    if (m_mapping.first.find(v) == m_mapping.first.end())
    {
      std::ostringstream oss;
      oss << "SyntacticData::depVertexForTokenVertex: searched dependency vertex " << v << " not found";
      throw LimaException(oss.str());
    }
    return (*(m_mapping.first.find(v))).second;
}

LinguisticGraphVertex SyntacticData::tokenVertexForDepVertex(const DependencyGraphVertex& v) const
{
  //SALOGINIT;
  //LDEBUG << this << " SyntacticData::m_mapping " << &m_mapping << " sizes : (" << m_mapping.first.size() << ","
  //          << m_mapping.second.size() << ")" << LENDL;
    if (m_mapping.second.find(v) == m_mapping.second.end())
        throw LimaException("SyntacticData::tokenVertexForDepVertex: searched morphosyntactic vertex not found");
    return (*(m_mapping.second.find(v))).second;
}

/**
  * @brief Computes the number of dependency relations in the graph between
  * the given source and target morphosyntactic vertices
  */
uint64_t SyntacticData::depsBetween(
    const LinguisticGraphVertex& srcVertex,
    const LinguisticGraphVertex& tgtVertex)
{
  uint64_t nb = 0;
  std::list< LinguisticGraphVertex > verticesToLookAt;
  verticesToLookAt.push_back(srcVertex);
  while (!verticesToLookAt.empty())
  {
    const LinguisticGraphVertex& currentVertex = verticesToLookAt.front();
    verticesToLookAt.pop_front();
    DependencyGraphVertex depVertex = depVertexForTokenVertex(currentVertex);
    nb += out_degree(depVertex, *m_depGraph);

    DependencyGraphOutEdgeIt it, it_end;
    for (boost::tie(it, it_end) = out_edges(depVertex, *m_depGraph); it != it_end; it++)
    {
      DependencyGraphVertex outDepVertex = target(*it, *m_depGraph);
      LinguisticGraphVertex outVertex = tokenVertexForDepVertex(outDepVertex);
      if ( outVertex != m_anagraph->lastVertex() && outVertex != tgtVertex)
      {
        verticesToLookAt.push_back(outVertex);
      }
    }
  }
  return nb;
}


std::vector<LinguisticGraphVertex>
SyntacticData::findRelated(const LinguisticGraphVertex v,
                           const Common::MediaticData::SyntacticRelationId& relation)
{
  std::vector<LinguisticGraphVertex> related;
  EdgeDepRelTypePropertyMap depRelMap = get(edge_deprel_type, *m_depGraph);

  //look at in_edges and out_edges
  DependencyGraphOutEdgeIt outIt, outIt_end;
  boost::tie(outIt, outIt_end) = out_edges(v, *m_depGraph);
  for (; outIt != outIt_end; outIt++) {
    if (depRelMap[*outIt] == relation) {
      related.push_back(target(*outIt,*m_depGraph));
    }
  }

  DependencyGraphInEdgeIt inIt, inIt_end;
  boost::tie(inIt, inIt_end) = in_edges(v, *m_depGraph);
  for (; inIt != inIt_end; inIt++) {
    if (depRelMap[*inIt] == relation) {
      related.push_back(source(*inIt,*m_depGraph));
    }
  }

  return related;
}

// the same but with a set of possible relations
std::vector<LinguisticGraphVertex>
SyntacticData::findRelated(const LinguisticGraphVertex v,
                           const std::set<Common::MediaticData::SyntacticRelationId>& relations)
{
  std::vector<LinguisticGraphVertex> related;
  EdgeDepRelTypePropertyMap depRelMap = get(edge_deprel_type, *m_depGraph);

  //look at in_edges and out_edges
  DependencyGraphOutEdgeIt outIt, outIt_end;
  boost::tie(outIt, outIt_end) = out_edges(v, *m_depGraph);
  for (; outIt != outIt_end; outIt++) {
    if (relations.find(depRelMap[*outIt]) != relations.end()) {
      related.push_back(target(*outIt,*m_depGraph));
    }
  }

  DependencyGraphInEdgeIt inIt, inIt_end;
  boost::tie(inIt, inIt_end) = in_edges(v, *m_depGraph);
  for (; inIt != inIt_end; inIt++) {
    if (relations.find(depRelMap[*inIt]) != relations.end()) {
      related.push_back(source(*inIt,*m_depGraph));
    }
  }

  return related;
}

//**********************************************************************
// add a relation that is not attached to a chain (relation between two
// chains)
void SyntacticData::addRelationNoChain(const Common::MediaticData::SyntacticRelationId& relType,
                   const LinguisticGraphVertex& src,
                   const LinguisticGraphVertex& dest) {
  SALOGINIT;
  LDEBUG << "Adding relation " << relType << " between "
         << src << " and " << dest << " for no chain" << LENDL;

  ChainIdStruct noChain;

  DependencyGraphVertex v1 = depVertexForTokenVertex(src);
  DependencyGraphVertex v2 = depVertexForTokenVertex(dest);
  DependencyGraphEdge edge;
  bool success;
  boost::tie(edge, success) = add_edge(v1, v2, *m_depGraph);
  if (success)
  {
//     put(edge_depchain_id, *m_depGraph, edge, noChain);
    SALOGINIT;
    LDEBUG << "Storing relation "<<v1<<","<<v2<<"," << relType << LENDL;
    put(edge_deprel_type, *m_depGraph, edge, relType);
  }
  else {
    SALOGINIT;
    LERROR << "Was not able to add relation (" << relType << ") between "
           << src << " and " << dest << " for no chain" << LENDL;
    throw std::runtime_error("Was not able to add relation");
  }
}


void SyntacticData::modifyRelationNoChain(const Common::MediaticData::SyntacticRelationId& relType,
                   const LinguisticGraphVertex& src,
                   const LinguisticGraphVertex& dest,
                   const Common::MediaticData::SyntacticRelationId& newType) {
//   SALOGINIT;
//   LDEBUG << "modify relation " << relType << " between "
//          << src << " and " << dest << " for no chain" << LENDL;

  ChainIdStruct noChain;
  DependencyGraphVertex v1 = depVertexForTokenVertex(src);
  DependencyGraphVertex v2 = depVertexForTokenVertex(dest);
  DependencyGraphOutEdgeIt outItr,outItrEnd;
  boost::tie(outItr, outItrEnd) = out_edges(v1, *m_depGraph);
  for (; outItr != outItrEnd; outItrEnd++)
  {
    if(target(*outItr, *m_depGraph) != v2)
      continue;

    if (get(edge_deprel_type,*m_depGraph,*outItr) == relType)
    {
      put(edge_deprel_type, *m_depGraph, *outItr, newType);
      return;
    }
  }

  SALOGINIT;
  LERROR << "Was not able to modify relation (" << relType << ") between "
         << src << " and " << dest << " to " << newType << LENDL;
  throw std::runtime_error("Was not able to modify relation");
}


bool SyntacticData::removeDependency(const LinguisticGraphVertex& v1,
    const LinguisticGraphVertex& v2,
    const Common::MediaticData::SyntacticRelationId relationType)
{
  SALOGINIT;
  LDEBUG << "SyntacticData::removeDependency " << v1 << ", " << v2 << ", " << relationType << LENDL;
  EdgeDepRelTypePropertyMap relTypeMap = get(edge_deprel_type, *m_depGraph);
  
  DependencyGraphVertex dv1 = depVertexForTokenVertex(v1);
  DependencyGraphVertex dv2 = depVertexForTokenVertex(v2);
  LDEBUG << "SyntacticData::removeDependency dep vertices are " << dv1 << ", " << dv2 << LENDL;
  DependencyGraphOutEdgeIt outIt, outIt_end;
  boost::tie (outIt, outIt_end) = boost::out_edges(dv1, *m_depGraph);
  for (; outIt != outIt_end; outIt++)
  {
    if (target(*outIt, *m_depGraph) == dv2 && relTypeMap[*outIt] == relationType)
    {
      LDEBUG << "SyntacticData::removeDependency dependency found. Removing it." << LENDL;
      boost::remove_edge(*outIt, *m_depGraph);
      return true;
    }
  }
  LDEBUG << "SyntacticData::removeDependency dependency NOT found." << LENDL;
  return false;
}

} // closing namespace SyntacticAnalysis
} // closing namespace LinguisticProcessing
} // closing namespace Lima
