// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
