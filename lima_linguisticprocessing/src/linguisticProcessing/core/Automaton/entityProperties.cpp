// Copyright 2002-2020 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
