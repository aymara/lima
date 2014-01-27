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
