// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
