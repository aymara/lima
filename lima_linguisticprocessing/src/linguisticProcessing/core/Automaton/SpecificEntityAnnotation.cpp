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
/***************************************************************************
 *   Copyright (C) 2005 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/

#include "linguisticProcessing/core/Automaton/SpecificEntityAnnotation.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"

using namespace Lima::LinguisticProcessing::Automaton;

namespace Lima
{

namespace LinguisticProcessing
{

namespace SpecificEntities
{

SpecificEntityAnnotation::
SpecificEntityAnnotation(const RecognizerMatch& entity,
                         FsaStringsPool& sp) :
m_head(0),
m_type(entity.getType()),
m_features(entity.features()),
m_string(sp[entity.getString()]),
m_normalizedString(sp[entity.getNormalizedString(sp)]),
m_normalizedForm(0),
m_position(entity.positionBegin()),
m_length(entity.length())
{
  Automaton::EntityFeatures::const_iterator 
    f=entity.features().find(DEFAULT_ATTRIBUTE);
  if (f!=entity.features().end()) {
    m_normalizedForm=sp[boost::any_cast<const LimaString&>((*f).getValue())];
  }

  if (entity.getHead() == 0)
  {
    m_head = entity[0].m_elem.first;
  }
  else
  {
    m_head = entity.getHead();
  }

  m_vertices.reserve(entity.size());

  Automaton::RecognizerMatch::const_iterator it, it_end;
  it = entity.begin(); it_end = entity.end();
  for (; it != it_end; it++)
  {
    if ( (*it).m_elem.second )
    {
      m_vertices.push_back((*it).m_elem.first);
    }
  }
}

SpecificEntityAnnotation::~SpecificEntityAnnotation()
{
}

void SpecificEntityAnnotation::dump(std::ostream& os) const
{
  os << "head:" << m_head << " type:" << m_type << " vertices=";
  if (m_vertices.size() != 0)
  {
    os << m_vertices[0];
    for (uint64_t i = 1; i < m_vertices.size(); i++)
    {
      os << "," << m_vertices[i];
    }
  }
// OME   os << "<se h=\"" << m_head << "\" t=\"" << m_type << "\" vertices=\"";
// OME   if (m_vertices.size() != 0)
// OME   {
// OME     os << m_vertices[0];
// OME     for (uint64_t i = 1; i < m_vertices.size(); i++)
// OME     {
// OME       os << "," << m_vertices[i];
// OME     }
// OME   }
// OME   os << "\" />" << std::endl;
}


int DumpSpecificEntityAnnotation::dump(std::ostream& os, Common::AnnotationGraphs::GenericAnnotation& ga) const
{
  PROCESSORSLOGINIT;
  try
  {
    ga.value<SpecificEntityAnnotation>().dump(os);
    return SUCCESS_ID;
  }
  catch (const boost::bad_any_cast& )
  {
    LERROR << "This annotation is not a SpecificEntityAnnotation ; nothing dumped" << LENDL;
    return UNKNOWN_ERROR;
  }
}


} // SpecificEntities
} // LinguisticProcessing
} // Lima
