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
 * @file       forme.h
 * @author     Damien Nouvel <Damien.Nouvel@cea.fr> 

 *             Copyright (C) 2004 by CEA LIST
 * @author     Gael de Chalendar <Gael.de-Chalendar@cea.fr>
 * @author     Benoit Mathieu <mathieub@zoe.cea.fr>
 * @date       Mon Oct 07 2008
 * Project     s2lp
 *
 * @brief      represents an Easy form
 *
 */

#ifndef LIMA_LINGUISTICPROCESSINGS_ANALYSISDUMPERS_EASYXMLDUMPER_FORME_H
#define LIMA_LINGUISTICPROCESSINGS_ANALYSISDUMPERS_EASYXMLDUMPER_FORME_H


#include "poslong.h"
#include "relation.h"

#include <string>
#include <vector>
#include <cstdint>

namespace Lima {
namespace LinguisticProcessing {
namespace AnalysisDumpers {
namespace EasyXmlDumper {

struct Forme
{
  Forme();

  Forme(const Forme* f);

  uint64_t id;
  PosLong poslong;
  std::string forme;
  std::string inflForme;
  std::string macro;
  std::string micro;
  bool operator<(const Forme& f) const { return id<f.id; }
  std::vector<Relation*> m_outRelations;
  std::vector<Relation*> m_inRelations;

  bool hasInRelation(const std::string& type) const;
  bool hasOutRelation(const std::string& type) const;
};


} // end namespace EasyXmlDumper
} // end namespace AnalysisDumpers
} // end namespace LinguisticProcessings
} // end namespace Lima

#endif // LIMA_LINGUISTICPROCESSINGS_ANALYSISDUMPERS_EASYXMLDUMPER_FORME_H
