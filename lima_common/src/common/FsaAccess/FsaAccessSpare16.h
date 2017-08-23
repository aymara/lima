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
                          FsaDictOp.h  -  description
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
#ifndef FSA_ACCESS_SPARE16_H
#define FSA_ACCESS_SPARE16_H


#include "common/LimaCommon.h"
#include "FsaAccessReader16.h"

namespace Lima {
namespace Common {
namespace FsaAccess {

//   EXPIMP_TEMPLATE template class LIMA_FSAACCESS_EXPORT FsaAccessReader16<boost::adjacency_list<boost::vecS,boost::vecS,boost::bidirectionalS,selected_graph_types16::dicoVertexCountProperty>>;
 class LIMA_FSAACCESS_EXPORT FsaAccessSpare16: public FsaAccessReader16<selected_graph_types16::spareGraphType> {
  // type of graph
  typedef selected_graph_types16::spareGraphType graphType;

  typedef boost::graph_traits<graphType>::vertex_descriptor dicoVertex;
  typedef boost::graph_traits<graphType>::edge_descriptor dicoEdge;
  typedef boost::graph_traits<graphType>::degree_size_type dico_degree_size;

  public:
   typedef fsaReader_superword_iterator16<graphType> superword_iterator;
   typedef fsaReader_subword_iterator16<graphType> subword_iterator;
   FsaAccessSpare16(bool trie_direction_fwd=true);
   ~FsaAccessSpare16();
    // For IO
    FsaAccessIOHandler<graphType>* getFsaAccessIOHandler() const override;

};

} // namespace compactDict
} // namespace Common
} // namespace Lima

#endif   //FSA_ACCESS_SPARE16_H
