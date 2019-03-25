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
/***************************************************************************
 *   Copyright (C) 2003 by  CEA                                            *
 *   author Olivier MESNARD olivier.mesnard@cea.fr                         *
 *                                                                         *
 *  Compact dictionnary based on finite state automata implemented with    *
 *  Boost Graph library.                                                   *
 *  Algorithm is described in article from Daciuk, Mihov, Watson & Watson: *
 *  "Incremental Construction of Minimal Acyclic Finite State Automata"    *
 ***************************************************************************/

// for using hex and dec manipulators
#include <iostream>

// From boost library
#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>

#include "common/LimaCommon.h"

#include "FsaAccessBuilder16.h"
#include "FsaAccessIOHandler.h"

using namespace Lima;

namespace Lima {
namespace Common {
namespace FsaAccess {


FsaAccessBuilder16::FsaAccessBuilder16( bool trie_direction_fwd )
: FsaAccess16<selected_graph_types16::builderGraphType>(trie_direction_fwd) ,
  m_packingStatus(BUILDER)
{
}

FsaAccessBuilder16::~FsaAccessBuilder16()
{
}

FsaAccessIOHandler<selected_graph_types16::builderGraphType>*
 FsaAccessBuilder16::getFsaAccessIOHandler() const {
  return new FsaAccessIOHandlerWithMapping<selected_graph_types16::builderGraphType>();
//  return new FsaAccessIOHandlerWithoutMapping<selected_graph_types16::builderGraphType>();
}

// Same code as FsaAccessBuilderRandom !!
// duplicated toi avoid multiple inheritance
void FsaAccessBuilder16::write( const std::string & filename  ){

#ifdef DEBUG_CD
  FSAAIOLOGINIT;
  LDEBUG <<  "FsaAccessBuilder16::write(" << filename.c_str() << ")";
#endif
  std::ofstream os(filename.c_str(), std::ios::out | std::ios::binary  );
  if( !os.good() ) {
    std::string mess = "FsaAccessBuilder16::write: Can't open file " + filename;
#ifdef DEBUG_CD
    LERROR;
#endif
    throw( FsaNotSaved( mess ) );
  }
//  os.seekp(HEADER_SIZE ,std::ios_base::beg );
  FsaAccessBuilder16::write( os );
  os.close();
#ifdef DEBUG_CD
  LDEBUG <<  "FsaAccessBuilder16::write(" << filename.c_str() << "): end";
#endif
}

// Same code as FsaAccessBuilderRandom !!
// duplicated toi avoid multiple inheritance
void FsaAccessBuilder16::write ( std::ostream &os ){
#ifdef DEBUG_CD
  FSAAIOLOGINIT;
  LDEBUG <<  "FsaAccessBuilder16::write(std::ostream)";
#endif
  
  FsaAccessOStreamWrapper ow(os);
  FsaAccessBuilder16::write(ow);
}

// Same code as FsaAccessBuilderRandom !!
// duplicated toi avoid multiple inheritance
void FsaAccessBuilder16::write ( FsaAccessDataOutputHandler& oh ){
#ifdef DEBUG_CD
  FSAAIOLOGINIT;
  LDEBUG <<  "FsaAccessBuilder16::write(std::ostream)";
#endif
  
  FsaAccessOutputHandlerWrapper ow(oh);
  FsaAccessBuilder16::write(ow);
}

// Same code as FsaAccessBuilderRandom !!
// duplicated toi avoid multiple inheritance
void FsaAccessBuilder16::write ( AbstractFsaAccessOStreamWrapper &ow ){
#ifdef DEBUG_CD
  FSAAIOLOGINIT;
  LDEBUG <<  "FsaAccessBuilder16::write()";
#endif

  FsaAccessHeader::setPackingStatus(m_packingStatus);
  
  boost::graph_traits<graphType>::vertices_size_type nbVerts =
    boost::num_vertices(m_graph);
  boost::graph_traits<graphType>::edges_size_type nbEdges =
    boost::num_edges(m_graph);
  
  FsaAccessHeader::setNbVertices(nbVerts);
  FsaAccessHeader::setNbEdges(nbEdges);

  FsaAccessHeader::write(ow);

 #ifdef DEBUG_CD
  LDEBUG <<  "FsaAccessBuilder16::write: call to writeBody";
#endif
 writeBody( ow );
 #ifdef DEBUG_CD
  LDEBUG <<  "FsaAccessBuilder16::write: end";
#endif
}

void FsaAccessBuilder16::pack(){
#ifdef DEBUG_CD
  FSAAIOLOGINIT;
  LDEBUG <<  "FsaAccessBuilder::pack()";
#endif
  if(m_packingStatus==BUILDER) {
    replaceOrRegister(m_rootVertex);
    m_packingStatus = BUILT;
  }
}

void FsaAccessBuilder16::addWord( const LimaString & newWord ){
#ifdef DEBUG_CD
  FSAALOGINIT;
  LDEBUG <<  "FsaAccessBuilder::addWord(" << newWord << ")";
#endif
  dicoVertex prefix_leaf = m_rootVertex;

  PrefixIterator* prefixIt = getPrefixIterator(newWord);
  getPrefix( prefix_leaf, prefixIt );
#ifdef DEBUG_CD
  LWARN <<  "FsaAccessBuilder::addWord: prefix =  " << prefixIt->getCurrentPrefix();
#endif

  if( !prefixIt->hasNextLetter() ) {
#ifdef DEBUG_CD
     LWARN <<  "FsaAccessBuilder::addWord: already in dictionary!!! ";
#endif
   return;
  }

  replaceOrRegister(prefix_leaf);
  addSuffix( prefix_leaf, prefixIt );
  delete prefixIt;
}

void FsaAccessBuilder16::replaceOrRegister( dicoVertex candidateState ) {
#ifdef DEBUG_CD
  FSAALOGINIT;
  LDEBUG <<  "FsaAccessBuilder::replaceOrRegister: (" << candidateState << ")";
#endif

  // check if leaf
  // degree_size_type nbChild = out_degree(dicoVertex, m_graph);
  // if( nbChild == 0)
  dico_degree_size nbChild = boost::out_degree(candidateState, m_graph);
  #ifdef DEBUG_CD
  LDEBUG <<  "FsaAccessBuilder::replaceOrRegister: out_degree = " << nbChild;
  #endif
  if( nbChild == 0) {
    return;
  }
  
  // get Last Child
  boost::graph_traits<graphType>::out_edge_iterator ei, edge_end;
  boost::tie(ei,edge_end) = boost::out_edges(candidateState,m_graph);

//   boost::graph_traits<graphType>::adjacency_iterator vi, v_end;
//   boost::tie(vi,v_end) = boost::adjacent_vertices(candidateState, m_graph);
  //dicoVertex lastChild = *(v_end - 1);
  for( dico_degree_size i = 0 ; i < nbChild - 1 ; i++ ) {
    ei++;
//     vi++;
  }
  dicoVertex lastChild = boost::target(*ei, m_graph);

  // recursive call
  #ifdef DEBUG_CD
  LDEBUG <<  "FsaAccessBuilder::replaceOrRegister: recursive call on" << lastChild;
  #endif
  replaceOrRegister( lastChild );
  
  std::pair<const dicoVertex,bool> equivalent = findEquivalentInRegister( lastChild );
  if( equivalent.second ) {
    #ifdef DEBUG_CD
    assert(equivalent.first != lastChild);
    LDEBUG <<  "FsaAccessBuilder::replaceOrRegister: merging " << equivalent.first << " and " << lastChild << " with " << candidateState << " parent";
    #endif
    merge( equivalent.first, lastChild, candidateState );
  }
  else {
    #ifdef DEBUG_CD
    LDEBUG <<  "FsaAccessBuilder::replaceOrRegister: set HEAD_OF_CLASS_16 to " << lastChild;
    #endif
    dicoGraph_traits16<graphType>::nconst_vname_map_type vname_map = boost::get(boost::vertex_name,m_graph);
    put(vname_map,lastChild, get(vname_map,lastChild)|HEAD_OF_CLASS_16);
  }

}

void FsaAccessBuilder16::merge( dicoVertex inRegister,
       dicoVertex tempState, dicoVertex parentState ) {
#ifdef DEBUG_CD
  FSAALOGINIT;
  LDEBUG <<  "FsaAccessBuilder16::merge( " << inRegister << ", "
                                          << tempState << ", "
                                          << parentState << ")";
#endif
  // find in transition parentState -> tempstate
  std::pair<dicoEdgeType, bool> trans = edge(parentState, tempState, m_graph);
#ifdef DEBUG_CD
  Q_ASSERT( trans.second );
#endif
  // forward this transition to inRegister
  if( trans.second ) {
#ifdef DEBUG_CD
    LDEBUG <<  "FsaAccessBuilder16::merge: add_edge(" << parentState << ", " << inRegister << ", " << ")";
 #endif
    
    std::pair<dicoEdgeType, bool> res = add_edge(parentState, inRegister, m_graph);
    if (!res.second)
    {
      FSAALOGINIT;
      LERROR << "FsaAccessBuilder16::merge failed to add edge to the graph";
    }
#ifdef DEBUG_CD
    Q_ASSERT( res.second );
#endif
  }

  // delete tempstate
  #ifdef DEBUG_CD
  LDEBUG <<  "FsaAccessBuilder16::merge: remove vertex" << tempState;
  #endif
  clear_vertex(tempState, m_graph);
  remove_vertex(tempState, m_graph);
}


} // namespace FsaAccess
} // namespace Commmon
} // namespace Lima
