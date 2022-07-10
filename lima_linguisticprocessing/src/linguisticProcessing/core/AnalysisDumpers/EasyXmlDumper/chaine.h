// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
