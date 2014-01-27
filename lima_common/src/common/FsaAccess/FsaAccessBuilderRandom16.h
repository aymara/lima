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
                          FsaAccessBuilder16.h  -  description
                             -------------------
    begin                : mar mai 27 2003
    copyright            : (C) 2003 by Olivier Mesnard
    email                : olivier.mesnard@cea.fr
 ***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2003 by  CEA                                            *
 *   author Olivier MESNARD olivier.mesnard@cea.fr                         *
 *                                                                         *
 *  Compact dictionnary based on finite state automata implemented with    *
 *  Boost Graph library.                                                   *
 *  Algorithm is described in article from Daciuk, Mihov, Watson & Watson: *
 *  "Incremental Construction of Minimal Acyclic Finite State Automata"    *
 ***************************************************************************/
#ifndef FSA_ACCESS_BUILDER_RANDOM16_H
#define FSA_ACCESS_BUILDER_RANDOM16_H

#include <vector>
#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/adjacency_list.hpp>

#include "FsaAccessReader16.h"
#include "common/misc/AbstractRwAccessByString.h"

namespace Lima {
namespace Common {
namespace FsaAccess {

class LIMA_FSAACCESS_EXPORT FsaAccessBuilderRandom16: public AbstractModifierOnAccessByString,
                                public FsaAccessReader16<selected_graph_types16::builderGraphType> {
  // type of graph
  typedef selected_graph_types16::builderGraphType graphType;

  typedef boost::graph_traits<graphType>::vertex_descriptor dicoVertex;
  typedef boost::graph_traits<graphType>::edge_descriptor dicoEdge;
  typedef boost::graph_traits<graphType>::degree_size_type dico_degree_size;

  public:
    FsaAccessBuilderRandom16( bool trie_direction_fwd = true );
    virtual ~FsaAccessBuilderRandom16();
    void addRandomWord( const Lima::LimaString & newWord );
    
    // For IO
    void write( const std::string & filename  );
    void write( std::ostream &os );
    void write( FsaAccessDataOutputHandler &oh );
    FsaAccessIOHandler<graphType>* getFsaAccessIOHandler() const;
   
  protected:
    void write ( AbstractFsaAccessOStreamWrapper &ow );
  private:
    void replaceOrRegister( dicoVertex candidateState,
      PrefixIterator* prefixIt );
    void merge( dicoVertex inRegister, dicoVertex tempState,
       dicoVertex parentState, const ForwardPrefixIterator& textIt);
    void suppressEdge(
      const boost::graph_traits<graphType>::vertex_descriptor from,
      const boost::graph_traits<graphType>::vertex_descriptor to,
      const char32_t currentChar,
      const Lima::LimaChar* const word_content,
      const int32_t wordOffset );
    void cloneVertex(
      const boost::graph_traits<graphType>::vertex_descriptor oldTo,
      const boost::graph_traits<graphType>::vertex_descriptor newTo );
    void replaceEdge( const boost::graph_traits<graphType>::vertex_descriptor from,
      const boost::graph_traits<graphType>::vertex_descriptor to,
      const char32_t currentChar,
      const int32_t wordOffset );
    int updateHash( dicoVertex from, 
        PrefixIterator* prefixIt );
    bool scanAndCloneConfluentStates( dicoVertex from, 
      PrefixIterator* prefixIt,
      dicoVertex& lastState );
    bool cloneConfluentStates(
      char32_t currentChar,
      int32_t wordOffset,
      boost::graph_traits<selected_graph_types16::builderGraphType>::vertex_descriptor& toOldPath,
      PrefixIterator* prefixIt,
      boost::graph_traits<selected_graph_types16::builderGraphType>::vertex_descriptor fromNewPath,
      boost::graph_traits<selected_graph_types16::builderGraphType>::vertex_descriptor& toNewPath );
    void addEdge(
      const boost::graph_traits<selected_graph_types16::builderGraphType>::vertex_descriptor from,
      const boost::graph_traits<selected_graph_types16::builderGraphType>::vertex_descriptor to,
      const char32_t currentChar,
      const Lima::LimaChar* const word_content,
      const int32_t wordOffset );
      void initConversion( const boost::graph_traits<graphType>::vertex_descriptor from ) const;
    void addSuffix( dicoVertex from, PrefixIterator* prefixIt );
  private:
    dict_packing_type m_packingStatus;
};


} // namespace compactDict
} // namespace Common
} // namespace Lima

#endif   //FSA_ACCESS_BUILDER_RANDOM16_H
