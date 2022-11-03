// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/**
 *
 * @file       forme.cpp
 * @author     Damien Nouvel <Damien.Nouvel@cea.fr> 

 *             Copyright (C) 2004 by CEA LIST
 * @author     Gael de Chalendar <Gael.de-Chalendar@cea.fr>
 * @author     Benoit Mathieu <mathieub@zoe.cea.fr>
 * @date       Mon Oct 07 2008
 *
 * @brief      represents an Easy form
 *
 */

#include "forme.h"

namespace Lima {
namespace LinguisticProcessing {
namespace AnalysisDumpers {
namespace EasyXmlDumper {

Forme::Forme() :
    id(0),
    poslong(),
    forme(),
    inflForme(),
    macro(),
    micro(),
    m_outRelations(),
    m_inRelations()
{
}

Forme::Forme(const Forme* f) :
    id(f->id),
    poslong(f->poslong),
    forme(f->forme),
    inflForme(f->inflForme),
    macro(f->macro),
    micro(f->micro),
    m_outRelations(f->m_outRelations),
    m_inRelations(f->m_inRelations)
{
}

bool Forme::hasInRelation(const std::string& type) const
{
  std::vector<Relation*>::const_iterator it, it_end;
  it = m_inRelations.begin();
  it_end = m_inRelations.end();
  for (; it != it_end; it++)
  {
    if ( (*it)->type == type )
      return true;
  }
  return false;
}

bool Forme::hasOutRelation(const std::string& type) const
{
  std::vector< Relation* >::const_iterator it, it_end;
  it = m_outRelations.begin();
  it_end = m_outRelations.end();
  for (; it != it_end; it++)
  {
    if ( (*it)->type == type )
      return true;
  }
  return false;
}

} // end namespace EasyXmlDumper
} // end namespace AnalysisDumpers
} // end namespace LinguisticProcessings
} // end namespace Lima
