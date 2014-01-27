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
 * @file       chaine.h
 * @author     Damien Nouvel <Damien.Nouvel@cea.fr> 

 *             Copyright (C) 2004 by CEA LIST
 * @author     Gael de Chalendar <Gael.de-Chalendar@cea.fr>
 * @author     Benoit Mathieu <mathieub@zoe.cea.fr>
 * @date       Mon Oct 07 2008
 * Project     s2lp
 *
 * @brief      represents a chain
 *
 */

#ifndef LIMA_LINGUISTICPROCESSINGS_ANALYSISDUMPERS_EASYXMLDUMPER_CHAINE_H
#define LIMA_LINGUISTICPROCESSINGS_ANALYSISDUMPERS_EASYXMLDUMPER_CHAINE_H

#include <limits>
#include <string>

namespace Lima {
namespace LinguisticProcessing {
namespace AnalysisDumpers {
namespace EasyXmlDumper {

class Chaine
{
public:
  Chaine() : type(""), id(std::numeric_limits<unsigned int>::max()) {};
  Chaine(const Chaine& c) : type(c.type), id(c.id) {};
  virtual ~Chaine() {};
  Chaine& operator=(const Chaine& c) {type = c.type; id = c.id; return *this;};
  
  std::string type;
  unsigned int id;
  bool operator<(const Chaine& ch) const
  {
    if (type==ch.type) return (id<ch.id);
    return type<ch.type;
  };
};

} // end namespace EasyXmlDumper
} // end namespace AnalysisDumpers
} // end namespace LinguisticProcessings
} // end namespace Lima

#endif // LIMA_LINGUISTICPROCESSINGS_ANALYSISDUMPERS_EASYXMLDUMPER_CHAINE_H
