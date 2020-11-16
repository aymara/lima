/*
    Copyright 2002-2020 CEA LIST

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
/*************************************************************************
*
* File        : entityProperties.cpp
* Author      : Besancon Romaric (besanconr@zoe.cea.fr)
* Created on  : Fri Jan 17 2003
* Copyright   : (c) 2003 by CEA
* Version     : $Id$
*
*************************************************************************/

#include "entityProperties.h"
#include "common/MediaticData/EntityType.h"
#include "common/MediaticData/mediaticData.h"
#include "common/Data/strwstrtools.h"

using namespace std;

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {


/***********************************************************************/
// constructors
/***********************************************************************/
EntityProperties::EntityProperties():
  m_head(0),
  m_type(),
  m_features(),
  m_contextual(true)
{}

EntityProperties::EntityProperties(const EntityProperties& p)
{
  copy(p);
}

EntityProperties& EntityProperties::operator = (const EntityProperties& p)
{
  if (this != &p)
  {
    copy(p);
  }
  return (*this);
}

bool EntityProperties::operator== (const EntityProperties& p)
{
  return (m_head == p.m_head &&
          m_type == p.m_type &&
          m_linguisticProperties == p.m_linguisticProperties &&
          m_features == p.m_features &&
          m_contextual == p.m_contextual);
}

void EntityProperties::copy(const EntityProperties& p)
{
  if (p.getHead() != 0)
  {
    m_head = p.getHead();
  }
  else
  {
    m_head = 0;
  }
  m_type=p.m_type;
  m_linguisticProperties=p.m_linguisticProperties;
  m_features=p.m_features;
  m_contextual=p.m_contextual;
}


/***********************************************************************/
// destructor
/***********************************************************************/
EntityProperties::~EntityProperties()
{
}

/***********************************************************************/
// output
/***********************************************************************/
LimaString EntityProperties::str() const
{
  ostringstream oss;
  if (m_head)
  {
    oss << "head=" << m_head << ";";
  }
  oss << "type=[" << m_type << "];";
  if (m_linguisticProperties.toBool())
  {
    oss << "lingProp=[" << m_linguisticProperties.toString() << "];";
  }
  oss << "norm=[" << m_features << "]"
      << "contextual=[" << m_contextual << "]";
  return Common::Misc::utf8stdstring2limastring(oss.str());
}

} // end namespace
} // end namespace
} // end namespace
