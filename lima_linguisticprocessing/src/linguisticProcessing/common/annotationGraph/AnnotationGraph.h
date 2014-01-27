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
/** @brief       A graph that stores any data (annotations) referencing
  *              primarily nodes of a text anlaysis
  *
  * @file        AnnotationGraph.h
  * @author      Gael de Chalendar <Gael.de-Chalendar@cea.fr> 

  *              Copyright (c) 2004 by CEA
  * @date        Created on Nov, 8 2004
  * @version     $Id$
  *
  */

#ifndef LIMA_ANNOTATIONGRAPHS_ANNOTATIONGRAPH_H
#define LIMA_ANNOTATIONGRAPHS_ANNOTATIONGRAPH_H

#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "linguisticProcessing/common/annotationGraph/GenericAnnotation.h"
#include "common/Data/LimaString.h"

#include <vector>
#include <stdexcept>
#include <boost/graph/adjacency_list.hpp>
#include "common/misc/depth_first_searchnowarn.hpp"

//========== Vertex properties

/** Default color property to use in various algorithms */
typedef boost::property<boost::vertex_color_t, boost::default_color_type> AGVertexColorProperty;

/** Property to annotate vertices with integer values */
typedef std::map< uint64_t, uint64_t > AGIannotProp;
enum vertex_iannot_t { vertex_iannot };
typedef boost::property< vertex_iannot_t, AGIannotProp, AGVertexColorProperty > AGVIannotProperty;
namespace boost {BOOST_INSTALL_PROPERTY(vertex, iannot);}

/** Property to annotate vertices with string values */
typedef std::map< uint64_t, Lima::LimaString > AGSannotProp;
enum vertex_sannot_t { vertex_sannot };
typedef boost::property< vertex_sannot_t, AGSannotProp, AGVIannotProperty> AGVSannotProperty;
namespace boost {BOOST_INSTALL_PROPERTY(vertex, sannot);}

/** Property to annotate vertices with generic values */
typedef std::map< uint64_t, Lima::Common::AnnotationGraphs::GenericAnnotation > AGGannotProp;
enum vertex_gannot_t { vertex_gannot };
typedef boost::property< vertex_gannot_t, AGGannotProp, AGVSannotProperty> AGVGannotProperty;
namespace boost {BOOST_INSTALL_PROPERTY(vertex, gannot);}

/** Final declaration of the vertices properties */
typedef AGVGannotProperty AGVertexProperties;


//========== Edges properties


/** Property to annotate edges with integer values */
typedef std::map< uint64_t, uint64_t > AGIannotProp;
enum edge_iannot_t { edge_iannot };
typedef boost::property< edge_iannot_t, AGIannotProp > AGEIannotProperty;
namespace boost {BOOST_INSTALL_PROPERTY(edge, iannot);}

/** Property to annotate edges with string values */
typedef std::map< uint64_t, Lima::LimaString > AGSannotProp;
enum edge_sannot_t { edge_sannot };
typedef boost::property< edge_sannot_t, AGSannotProp, AGEIannotProperty> AGESannotProperty;
namespace boost {BOOST_INSTALL_PROPERTY(edge, sannot);}

/** Property to annotate edges with generic values */
typedef std::map< uint64_t, Lima::Common::AnnotationGraphs::GenericAnnotation > AGGannotProp;
enum edge_gannot_t { edge_gannot };
typedef boost::property< edge_gannot_t, AGGannotProp, AGESannotProperty> AGEGannotProperty;
namespace boost {BOOST_INSTALL_PROPERTY(edge, gannot);}


/** Final declaration of the edges properties */
typedef AGEGannotProperty AGEdgeProperties;


// ============ Graph class

/** The graph class. It uses vecS for edges and vertices in order to have quick
  * acces to specific indices in the vertices and edges lists */
typedef boost::adjacency_list< boost::vecS, boost::vecS, boost::bidirectionalS,
        AGVertexProperties, AGEdgeProperties > AnnotationGraph;


/** @defgroup AnnotationGraphElementsAccess typedefs to simplify the acces to various graphs elements
  @{ */
typedef AnnotationGraph::edge_descriptor AnnotationGraphEdge;
typedef AnnotationGraph::edge_iterator AnnotationGraphEdgeIt;
typedef AnnotationGraph::out_edge_iterator AnnotationGraphOutEdgeIt;
typedef AnnotationGraph::in_edge_iterator AnnotationGraphInEdgeIt;
typedef AnnotationGraph::vertex_descriptor AnnotationGraphVertex;
typedef AnnotationGraph::vertex_iterator AnnotationGraphVertexIt;
typedef AnnotationGraph::adjacency_iterator AnnotationGraphAdjacencyIt;
typedef boost::property_map<AnnotationGraph, edge_iannot_t>::type EdgeAGIannotPropertyMap;
typedef boost::property_map<AnnotationGraph, edge_iannot_t>::const_type CEdgeAGIannotPropertyMap;
typedef boost::property_map<AnnotationGraph, edge_sannot_t>::type EdgeAGSannotPropertyMap;
typedef boost::property_map<AnnotationGraph, edge_sannot_t>::const_type CEdgeAGSannotPropertyMap;
typedef boost::property_map<AnnotationGraph, edge_gannot_t>::type EdgeAGGannotPropertyMap;
typedef boost::property_map<AnnotationGraph, edge_gannot_t>::const_type CEdgeAGGannotPropertyMap;
typedef boost::property_map<AnnotationGraph, edge_iannot_t>::type EdgeAGIannotPropertyMap;

typedef boost::property_map<AnnotationGraph, vertex_iannot_t>::type VertexAGIannotPropertyMap;
typedef boost::property_map<AnnotationGraph, vertex_iannot_t>::const_type CVertexAGIannotPropertyMap;
typedef boost::property_map<AnnotationGraph, vertex_sannot_t>::type VertexAGSannotPropertyMap;
typedef boost::property_map<AnnotationGraph, vertex_sannot_t>::const_type CVertexAGSannotPropertyMap;
typedef boost::property_map<AnnotationGraph, vertex_gannot_t>::type VertexAGGannotPropertyMap;
typedef boost::property_map<AnnotationGraph, vertex_gannot_t>::const_type CVertexAGGannotPropertyMap;

typedef boost::property_map<AnnotationGraph, boost::vertex_color_t >::type AGVertexColorPropertyMap;
/** @} */

namespace Lima {
namespace Common {
namespace AnnotationGraphs {

  LIMA_ANNOTATIONGRAPH_EXPORT QDebug& operator<< (QDebug& os, const AnnotationGraphEdge& e);
  
} // closing namespace AnnotationGraphs
} // closing namespace Common
} // closing namespace Lima


#endif // LIMA_ANNOTATIONGRAPHS_ANNOTATIONGRAPH_H
