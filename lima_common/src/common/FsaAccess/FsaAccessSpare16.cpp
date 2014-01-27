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
: FsaAccessReader16<selected_graph_types16::spareGraphType>(trie_direction_fwd),
  m_packingStatus(SPARE)
  {
#ifdef DEBUG_CD
  FSAALOGINIT;
  LDEBUG <<  "FsaAccessSpare16::FsaAccessSpare16()" << LENDL;
#endif
}

FsaAccessSpare16::~FsaAccessSpare16()
{
#ifdef DEBUG_CD
  FSAALOGINIT;
  LDEBUG <<  "FsaAccessSpare16::~FsaAccessSpare16()" << LENDL;
#endif
}
    
FsaAccessIOHandler<selected_graph_types16::spareGraphType>* 
FsaAccessSpare16::getFsaAccessIOHandler() const {
  return new FsaAccessIOHandlerWithoutMapping<selected_graph_types16::spareGraphType>();
}


} // namespace FsaAccess
} // namespace Commmon
} // namespace Lima
