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
  * @brief        A graph structure for linguistic analysis
  *
  * @file         LinguisticGraph.h
  * @author       Gael de Chalendar (Gael.de-Chalendar@cea.fr)

  * @author       Benoit Mathieu
  *
  */

#ifndef LIMA_LINGUISTICPROCESSING_LINGUISTICANALYSISSTRUCTURE_LINGUISTICGRAPH_H
#define LIMA_LINGUISTICPROCESSING_LINGUISTICANALYSISSTRUCTURE_LINGUISTICGRAPH_H

#include "Token.h"
#include "MorphoSyntacticData.h"
#include "ChainIdStruct.h"


#include <vector>
#include <boost/graph/adjacency_list.hpp>
#include "common/misc/depth_first_searchnowarn.hpp"
#include <boost/graph/breadth_first_search.hpp>
#include <boost/serialization/strong_typedef.hpp>


//========== defines
// the graphs are always constructed such as first vertex is numbered 0
// and last vertex is numbered 1
//========== Vertex properties
/** This vertex proprerty represents the linguistic object pointed by the node with
  * position and length. Its orthographic alternatives (Token*) will be
  * distributed through several vertices each having the same FullToken* for
  * VertexLingProp */

typedef Lima::LinguisticProcessing::LinguisticAnalysisStructure::Token* VertexTokenProp;
enum vertex_token_t { vertex_token = 1000 };
typedef boost::property< vertex_token_t, VertexTokenProp > VertexTokenProperty;
namespace boost {BOOST_INSTALL_PROPERTY(vertex, token);}

/** This vertex proprerty represents the linguitic object pointed by the node
  * It points to only one word defined as a lemma and its category */

typedef Lima::LinguisticProcessing::LinguisticAnalysisStructure::MorphoSyntacticData* VertexDataProp;
enum vertex_data_t { vertex_data = 1001 };
typedef boost::property< vertex_data_t, VertexDataProp, VertexTokenProperty > VertexDataProperty;
namespace boost {BOOST_INSTALL_PROPERTY(vertex, data);}


typedef boost::property<boost::vertex_color_t, boost::default_color_type, VertexDataProperty> VertexLinguisticGraphColorProperty;

/** Property to identify the chains in the graph. Each vertex can belong to
  * several chains, so the property is a set of objects
  * composed of a chain number in the path and a chain type
  */
typedef std::set< Lima::LinguisticProcessing::LinguisticAnalysisStructure::ChainIdStruct > VertexChainIdProp;
enum vertex_chain_id_t { vertex_chain_id = 1005 };
typedef boost::property< vertex_chain_id_t, VertexChainIdProp, VertexLinguisticGraphColorProperty > VertexChainIdProperty;
namespace boost {BOOST_INSTALL_PROPERTY(vertex, chain_id);}

/** Final declaration of the vertices properties */
typedef VertexChainIdProperty LinguisticVertexProperties;


//========== Edges properties
/** Property to identify the chains in the graph. Works like the PathIdProperty
  * but each edge can belong to several chains in various paths, so the property
  * is a set of objects composed of a path number, a chain number in the path
  * and a chain type */
// typedef std::set< Lima::LinguisticProcessing::LinguisticAnalysisStructure::ChainIdStruct > ChainIdProp;
//
// enum edge_chain_id_t { edge_chain_id = 2002 };
// typedef boost::property< edge_chain_id_t, ChainIdProp > ChainIdProperty;
// namespace boost {BOOST_INSTALL_PROPERTY(edge, chain_id);}

/** Final declaration of the edges properties */
// typedef ChainIdProperty LinguisticEdgeProperties;

// ============ Graph class

/**
 * @brief The boost graph based linguistic graph class.
 * It represents the (possible) successions of tokens. Vertices are tokens (either simple, named
 * entities, idiom, etc.) and edges reflect text order.
 * It uses vecS for edges and vertices in order to have quick
 * acces to specific indices in the vertices and edges lists
 */
typedef boost::adjacency_list< boost::vecS, boost::vecS, boost::bidirectionalS,
LinguisticVertexProperties/*,LinguisticEdgeProperties*/> LinguisticGraph;

/** typedefs to simplify the acces to various graphs elements */
typedef boost::graph_traits< LinguisticGraph >::edge_descriptor LinguisticGraphEdge;
typedef LinguisticGraph::edge_iterator LinguisticGraphEdgeIt;
typedef LinguisticGraph::out_edge_iterator LinguisticGraphOutEdgeIt;
typedef LinguisticGraph::in_edge_iterator LinguisticGraphInEdgeIt;
typedef LinguisticGraph::vertex_descriptor LinguisticGraphVertex;
//BOOST_STRONG_TYPEDEF(LinguisticGraph::vertex_descriptor, LinguisticGraphVertex);
typedef LinguisticGraph::vertex_iterator LinguisticGraphVertexIt;
typedef LinguisticGraph::adjacency_iterator LinguisticGraphAdjacencyIt;
typedef boost::property_map<LinguisticGraph, vertex_token_t>::type VertexTokenPropertyMap;
typedef boost::property_map<LinguisticGraph, vertex_token_t>::const_type CVertexTokenPropertyMap;
typedef boost::property_map<LinguisticGraph, vertex_data_t>::type VertexDataPropertyMap;
typedef boost::property_map<LinguisticGraph, vertex_data_t>::const_type CVertexDataPropertyMap;
typedef boost::property_map<LinguisticGraph, vertex_chain_id_t>::type VertexChainIdPropertyMap;
typedef boost::property_map<LinguisticGraph, vertex_chain_id_t>::const_type CVertexChainIdPropertyMap;

typedef std::set<LinguisticGraphVertex> SetOfLinguisticGraphVertices;

// typedef boost::property_map<LinguisticGraph, edge_chain_id_t>::type EdgeChainIdPropertyMap;
typedef boost::property_map<LinguisticGraph, boost::vertex_color_t >::type VertexColorLinguisticGraphPropertyMap;

LIMA_LINGUISTICANALYSISSTRUCTURE_EXPORT QDebug& operator<< (QDebug& qd, LinguisticGraphEdge& edge);

#endif // LIMA_GRAPH_PHOENIXGRAPH_H
