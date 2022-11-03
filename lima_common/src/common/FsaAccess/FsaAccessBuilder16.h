// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
#ifndef COMPACT_DICT_FSA_DICT_BUILDER16_HPP
#define COMPACT_DICT_FSA_DICT_BUILDER16_HPP

#include <vector>
#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/adjacency_list.hpp>

#include "common/FsaAccess/FsaAccess16.h"

namespace Lima {
namespace Common {
namespace FsaAccess {

class LIMA_FSAACCESS_EXPORT FsaAccessBuilder16: public FsaAccess16<selected_graph_types16::builderGraphType> {
  // type of graph
  typedef selected_graph_types16::builderGraphType graphType;

  typedef boost::graph_traits<graphType>::vertex_descriptor dicoVertex;
  typedef boost::graph_traits<graphType>::edge_descriptor dicoEdge;
  typedef boost::graph_traits<graphType>::degree_size_type dico_degree_size;

  public:
    FsaAccessBuilder16( bool trie_direction_fwd = true );
    virtual ~FsaAccessBuilder16();

    // For IO
    FsaAccessIOHandler<graphType>* getFsaAccessIOHandler() const override;
   
    virtual void addWord( const Lima::LimaString & newWord );
    void write( const std::string & filename  );
    void write( std::ostream &os );
    void write( FsaAccessDataOutputHandler &oh );
    void pack();
  protected:
    void write( AbstractFsaAccessOStreamWrapper &ow );
    virtual void replaceOrRegister( dicoVertex candidateState );
    void merge( dicoVertex inRegister, dicoVertex tempState, dicoVertex parentState );
  private:
    dict_packing_type m_packingStatus;
};


} // namespace compactDict
} // namespace Common
} // namespace Lima

#endif   //COMPACT_DICT_FSA_DICT_BUILDER16_HPP
