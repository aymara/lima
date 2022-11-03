// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2003 by  CEA                                            *
 *   author Olivier MESNARD olivier.mesnard@cea.fr                         *
 *                                                                         *
 *  Compact dictionnary based on finite state automata implemented with    *
 *  Boost Graph library.                                                   *
 *  Algorithm is described in article from Daciuk, Mihov, Watson & Watson: *
 *  "Incremental Construction of Minimal Acyclic Finite State Automata"    *
 ***************************************************************************/

// From standard library
#include <iostream>
#include <sstream>
#include <algorithm>

#include "common/LimaCommon.h"

#include "FsaAccessSpare16.h"
#include "FsaAccessIOHandler.h"

using namespace Lima;

namespace Lima {
namespace Common {
namespace FsaAccess {

template class LIMA_FSAACCESS_EXPORT FsaAccessReader16<class boost::adjacency_list<struct boost::vecS,struct boost::vecS,struct boost::bidirectionalS,struct boost::property<enum vertex_count_t,class std::vector<int,class std::allocator<int> >,struct boost::property<enum boost::vertex_name_t,uint64_t,struct boost::property<enum vertex_text_t,class QString,struct boost::no_property> > >,struct boost::no_property,struct boost::no_property,struct boost::listS> >;

FsaAccessSpare16::FsaAccessSpare16(bool trie_direction_fwd)
: FsaAccessReader16<selected_graph_types16::spareGraphType>(trie_direction_fwd)
{
#ifdef DEBUG_CD
  FSAALOGINIT;
  LDEBUG <<  "FsaAccessSpare16::FsaAccessSpare16()";
#endif
}

FsaAccessSpare16::~FsaAccessSpare16()
{
}
    
FsaAccessIOHandler<selected_graph_types16::spareGraphType>* 
FsaAccessSpare16::getFsaAccessIOHandler() const {
  return new FsaAccessIOHandlerWithoutMapping<selected_graph_types16::spareGraphType>();
}


} // namespace FsaAccess
} // namespace Commmon
} // namespace Lima
