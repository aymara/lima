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
/**
 *
 * @file       relation.h
 * @author     Damien Nouvel <Damien.Nouvel@cea.fr> 

 *             Copyright (C) 2004 by CEA LIST
 * @author     Gael de Chalendar <Gael.de-Chalendar@cea.fr>
 * @author     Benoit Mathieu <mathieub@zoe.cea.fr>
 * @date       Mon Oct 07 2008
 * Project     s2lp
 *
 * @brief      represents an Easy relation (dep))
 *
 */

#ifndef LIMA_LINGUISTICPROCESSINGS_ANALYSISDUMPERS_EASYXMLDUMPER_RELATION_H
#define LIMA_LINGUISTICPROCESSINGS_ANALYSISDUMPERS_EASYXMLDUMPER_RELATION_H

#include "chaine.h"

#include <string>
#include <iostream>
#include <cstdint>

namespace Lima {
namespace LinguisticProcessing {
namespace AnalysisDumpers {
namespace EasyXmlDumper {

struct Relation
{

  Relation():
    srcVertex(0),
    tgtVertex(0),
    doFollow(true),
    secondaryVertex(0)
  {};

  uint64_t srcVertex;
  uint64_t tgtVertex;
  bool doFollow;

  /** to store the third element of a 3-ary relation, currently only the source
   * of the COORD2 relation. When we have "X <-COORD1- et <-COORD2- Y". 'X' is
   * tgtVertex, 'et' is srcVertex and 'Y' is secondaryVertex. */
  uint64_t secondaryVertex;
  Chaine chaine;
  std::string type;
  bool operator<(const Relation& r) const
  {
    if (srcVertex!=r.srcVertex) return srcVertex<r.srcVertex;
    if (tgtVertex!=r.tgtVertex) return tgtVertex<r.tgtVertex;
    return type<r.type;
  }
};


} // end namespace EasyXmlDumper
} // end namespace AnalysisDumpers
} // end namespace LinguisticProcessings
} // end namespace Lima

#endif // LIMA_LINGUISTICPROCESSINGS_ANALYSISDUMPERS_EASYXMLDUMPER_RELATION_H
