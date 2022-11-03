// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/** @brief       A graph that stores the relations of syntactic dependency
  *              between the elements of a DependencyGraph
  *
  * @file        DependencyGraph.h
  * @author      Gael de Chalendar <Gael.de-Chalendar@cea.fr> 

  *              Copyright (c) 2003 by CEA
  * @date        Created on Sep, 30 2003
  * @version     $Id$
  *
  */

#ifndef LIMA_SYNTACTICANALYSIS_DEPENDENCYGRAPH_H
#define LIMA_SYNTACTICANALYSIS_DEPENDENCYGRAPH_H

#include "SyntacticAnalysisExport.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/ChainIdStruct.h"
#include "linguisticProcessing/common/linguisticData/languageData.h" // for SyntacticRelationId

#include <vector>
#include <stdexcept>
#include <boost/graph/adjacency_list.hpp>
#include "common/misc/depth_first_searchnowarn.hpp"

//========== Vertex properties

/** @brief Default color property to use in various algorithms */
typedef boost::property<boost::vertex_color_t, boost::default_color_type> VertexDependencyGraphColorProperty;

/** @brief Final declaration of the vertices properties */
typedef VertexDependencyGraphColorProperty DepVertexProperties;


//========== Edges properties


/** @brief Property to identify the chain of the tokens graph concerned by the relation */
// enum edge_depchain_id_t { edge_depchain_id = 5000 };
// typedef boost::property< edge_depchain_id_t, Lima::LinguisticProcessing::LinguisticAnalysisStructure::ChainIdStruct > DepChainIdProperty;
// namespace boost {BOOST_INSTALL_PROPERTY(edge, depchain_id);}

/** @brief Property to identify the type of syntactic relations in the graph */
typedef Lima::Common::MediaticData::SyntacticRelationId DepRelTypeProp;
enum edge_deprel_type_t { edge_deprel_type = 5001 };
typedef boost::property< edge_deprel_type_t, DepRelTypeProp > DepRelTypeProperty;
namespace boost {BOOST_INSTALL_PROPERTY(edge, deprel_type);}


/** @brief Final declaration of the edges properties */
typedef DepRelTypeProperty DepEdgeProperties;


// ============ Graph class

/** @brief The dependency graph class. It uses vecS for edges and vertices in order to have quick
  * acces to specific indices in the vertices and edges lists */
typedef boost::adjacency_list< boost::vecS, boost::vecS, boost::bidirectionalS,
        DepVertexProperties, DepEdgeProperties > DependencyGraph;

/** @brief typedefs to simplify the acces to various graphs elements */
///@{
typedef DependencyGraph::edge_descriptor DependencyGraphEdge;
typedef DependencyGraph::edge_iterator DependencyGraphEdgeIt;
typedef DependencyGraph::out_edge_iterator DependencyGraphOutEdgeIt;
typedef DependencyGraph::in_edge_iterator DependencyGraphInEdgeIt;
typedef DependencyGraph::vertex_descriptor DependencyGraphVertex;
typedef DependencyGraph::vertex_iterator DependencyGraphVertexIt;
typedef DependencyGraph::adjacency_iterator DependencyGraphAdjacencyIt;
typedef boost::property_map<DependencyGraph, edge_deprel_type_t>::type EdgeDepRelTypePropertyMap;
typedef boost::property_map<DependencyGraph, edge_deprel_type_t>::const_type CEdgeDepRelTypePropertyMap;
// typedef boost::property_map<DependencyGraph, edge_depchain_id_t>::type EdgeDepChainIdPropertyMap;
// typedef boost::property_map<DependencyGraph, edge_depchain_id_t>::const_type CEdgeDepChainIdPropertyMap;
typedef boost::property_map<DependencyGraph, boost::vertex_color_t >::type
    VertexColorDepGraphPropertyMap;
///@}

/** @brief Comparison operator to order pairs of dependency graph vertices.
 *
 * This order is completely arbitrary.
 */
inline bool operator<(const std::pair<DependencyGraphVertex, DependencyGraphVertex>& e1,
        const std::pair<DependencyGraphVertex, DependencyGraphVertex>& e2)
{
    return ( (e1.first < e2.first) || (e1.first == e2.first && e1.second < e2.second) );
}

namespace Lima {
namespace LinguisticProcessing {
namespace SyntacticAnalysis {

#define SAPLOGINIT  LOGINIT("LP::SyntacticAnalysis::Deps")

} // closing namespace SyntacticAnalysis
} // closing namespace LinguisticProcessing
} // closing namespace Lima


#endif
