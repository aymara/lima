// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include <iostream>
#include <vector>
#include <string>
#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>
  
/*  
  // Declare a property (vertex_text) of type LimaString
  // to store more efficiently edge label
  typedef boost::property< vertex_text_t, LimaString >
   dicoVertexTextProperty;
  // Declare a property (vertex_name) of type unsigned char to store
  // vertex quality :
  // (qualifer & 1) == 1: final, (qualifier & 2) == 2: head of class,
  // (qualifer & 4) == 4: confluent
  typedef boost::property< boost::vertex_name_t, VERTEX_PROPERTY_16, dicoVertexTextProperty>
   dicoVertexStatusProperty;
  // Declare a property (vertex_count) of type std::vector<int> to store
  // count of sub automata (to compute hash)
  typedef boost::property< vertex_count_t, std::vector<int>, dicoVertexStatusProperty>
   dicoVertexCountProperty;
*/   
  
  typedef boost::adjacency_list<boost::vecS,
                                boost::listS,
                                boost::bidirectionalS
                                > GraphType;
  
  typedef boost::graph_traits<GraphType>::vertex_descriptor DicoVertexType;
  typedef boost::graph_traits<GraphType>::edge_descriptor DicoEdgeType;
  typedef boost::graph_traits<GraphType>::degree_size_type DicoDegreeSizeType;

int main(int argc, char *argv[])
{
   GraphType m_graph(5);
   
   std::vector<DicoVertexType> verts;
   verts.push_back(add_vertex(m_graph));
   verts.push_back(add_vertex(m_graph));
   verts.push_back(add_vertex(m_graph));
   verts.push_back(add_vertex(m_graph));
   verts.push_back(add_vertex(m_graph));
   verts.push_back(add_vertex(m_graph));
   verts.push_back(add_vertex(m_graph));
   verts.push_back(add_vertex(m_graph));
   
   std::cerr << "construction graphe" << std::endl;
   add_edge(verts[0], verts[1], m_graph);
   add_edge(verts[1], verts[2], m_graph);
   add_edge(verts[2], verts[3], m_graph);
   add_edge(verts[3], verts[4], m_graph);
   add_edge(verts[0], verts[5], m_graph);
   add_edge(verts[5], verts[2], m_graph);
   add_edge(verts[2], verts[6], m_graph);
   add_edge(verts[6], verts[4], m_graph);
   add_edge(verts[5], verts[7], m_graph);
   add_edge(verts[7], verts[4], m_graph);

   // suppression de (5,2)
   std::cerr << "suppression de edge(5,2)" << std::endl;
   DicoVertexType from = verts[5];
   boost::graph_traits<GraphType>::out_edge_iterator edge_start, edge_end;
   boost::tie(edge_start,edge_end) = boost::out_edges(from,m_graph);
   int32_t edgeOffset = 0;
   DicoDegreeSizeType outd0 = boost::out_degree(from, m_graph);
   assert( outd0 == 2 );
   for( boost::graph_traits<GraphType>::out_edge_iterator ei = edge_start ;
        ei != edge_end ; ei++, edgeOffset++ ) {
     assert( ei == (ei+edgeOffset));
     DicoEdgeType edge = *(ei+edgeOffset);
     DicoVertexType to = target(edge, m_graph);
     if( to == verts[2]) {
       remove_edge( edge, m_graph );
       break;
     }
   }
   assert( edgeOffset < 2 );
   DicoDegreeSizeType outd = boost::out_degree(from, m_graph);
   assert( outd == 1 );
   
   std::cerr << "duplication de vert[2]" << std::endl;
   // creation de v8
   DicoVertexType vertex8 = add_vertex(m_graph);
   add_edge(verts[7], verts[4], m_graph);
   
   // lien 5 -> 8
   add_edge(verts[5], vertex8, m_graph);
   
   // duplication de 2 vers 8
   DicoVertexType ref = verts[2];
   boost::tie(edge_start,edge_end) = boost::out_edges(ref,m_graph);
   std::list<DicoVertexType> memTargets;
   for( ; edge_start != edge_end ; edge_start++ ) {
     memTargets.push_back(target(*edge_start, m_graph));
   }
   for( std::list<DicoVertexType>::const_iterator it = memTargets.begin() ;
     it != memTargets.end()  ; it++ ) {
     add_edge(vertex8, *it, m_graph);
   }
   DicoDegreeSizeType outd8 = boost::out_degree(vertex8, m_graph);
   assert(outd8 == boost::out_degree(ref, m_graph));
   
   std::cerr << "fin!" << std::endl;
              
}
