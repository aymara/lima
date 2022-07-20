// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2005 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/

#include "linguisticProcessing/core/Automaton/SpecificEntityAnnotation.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"

using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::LinguisticProcessing::Automaton;

namespace Lima
{

namespace LinguisticProcessing
{

namespace SpecificEntities
{

class SpecificEntityAnnotationPrivate
{
  friend class SpecificEntityAnnotation;

  SpecificEntityAnnotationPrivate(const Automaton::RecognizerMatch& entity,
                           FsaStringsPool& sp);
  SpecificEntityAnnotationPrivate( const std::deque<LinguisticGraphVertex>& vertices,
                            Common::MediaticData::EntityType entityType,
                            const LimaString& form,
                            const LimaString& normalizedForm,
                            int nbError,
                            uint64_t startPos,
                            uint64_t length,
                            FsaStringsPool& sp);
  virtual ~SpecificEntityAnnotationPrivate() = default;
  SpecificEntityAnnotationPrivate(const SpecificEntityAnnotationPrivate& ) = default;
  SpecificEntityAnnotationPrivate& operator=(const SpecificEntityAnnotationPrivate& ) = default;

  std::vector< LinguisticGraphVertex> m_vertices;

  LinguisticGraphVertex m_head;
  Common::MediaticData::EntityType m_type;    /**< the type of the entity */
  Automaton::EntityFeatures m_features;
  StringsPoolIndex m_string;
  StringsPoolIndex m_normalizedString;
  StringsPoolIndex m_normalizedForm;
  uint64_t m_position;
  uint64_t m_length;
};


SpecificEntityAnnotationPrivate::SpecificEntityAnnotationPrivate(
    const std::deque<LinguisticGraphVertex>& vertices,
    Common::MediaticData::EntityType entityType,
    const LimaString& form,
    const LimaString& normalizedForm,
    int nbError,
    uint64_t startPos,
    uint64_t length,
    FsaStringsPool& sp) :
  // front, head or any hint for another vertex?
  m_vertices(vertices.begin(),vertices.end()),
  m_head(vertices.front()),
  m_type(entityType),
  m_string(sp[form]),
  m_normalizedString(sp[normalizedForm]),
  m_normalizedForm(sp[normalizedForm]),
  m_position(startPos),
  m_length(length)
{
  // no features??
  m_features.addFeature("value", normalizedForm);
  // no features??
  m_features.addFeature("nbError", nbError);
}

SpecificEntityAnnotationPrivate::SpecificEntityAnnotationPrivate(
    const RecognizerMatch& entity,
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

SpecificEntityAnnotation::SpecificEntityAnnotation(
  const std::deque<LinguisticGraphVertex>& vertices,
  Common::MediaticData::EntityType entityType,
  const LimaString& form,
  const LimaString& normalizedForm,
  int nbError,
  uint64_t startPos,
  uint64_t length,
  FsaStringsPool& sp) :
  m_d(new SpecificEntityAnnotationPrivate(vertices,
                                          entityType,
                                          form,
                                            normalizedForm,
                                          nbError,
                                          startPos,
                                          length,
                                          sp))
{
}

SpecificEntityAnnotation::SpecificEntityAnnotation(
    const RecognizerMatch& entity,
    FsaStringsPool& sp) :
  m_d(new SpecificEntityAnnotationPrivate(entity, sp))
{
}

SpecificEntityAnnotation::~SpecificEntityAnnotation()
{
  delete m_d;
}

SpecificEntityAnnotation::SpecificEntityAnnotation(const SpecificEntityAnnotation& annotation):
  m_d(new SpecificEntityAnnotationPrivate(*annotation.m_d))
{
}

SpecificEntityAnnotation& SpecificEntityAnnotation::operator=(const SpecificEntityAnnotation& annotation)
{
  if (m_d != nullptr)
    delete m_d;
  m_d = new SpecificEntityAnnotationPrivate(*annotation.m_d);
  return *this;
}

LinguisticGraphVertex SpecificEntityAnnotation::getHead() const
{
  return m_d->m_head;
}

void SpecificEntityAnnotation::setHead(const LinguisticGraphVertex& head)
{
  m_d->m_head = head;
}

Common::MediaticData::EntityType SpecificEntityAnnotation::getType() const
{
  return m_d->m_type;
}

void SpecificEntityAnnotation::setType(const Common::MediaticData::EntityType& type)
{
  m_d->m_type = type;
}

const Automaton::EntityFeatures& SpecificEntityAnnotation::getFeatures() const
{
  return m_d->m_features;
}

void SpecificEntityAnnotation::setFeatures(const Automaton::EntityFeatures& features)
{
  m_d->m_features = features;
}

StringsPoolIndex SpecificEntityAnnotation::getString() const
{
  return m_d->m_string;
}

StringsPoolIndex SpecificEntityAnnotation::getNormalizedString() const
{
  return m_d->m_normalizedString;
}

StringsPoolIndex SpecificEntityAnnotation::getNormalizedForm() const
{
  return m_d->m_normalizedForm;
}

uint64_t SpecificEntityAnnotation::getPosition() const
{
  return m_d->m_position;
}

uint64_t SpecificEntityAnnotation::getLength() const
{
  return m_d->m_length;
}

const std::vector< LinguisticGraphVertex>& SpecificEntityAnnotation::vertices() const
{
  return m_d->m_vertices;
}

void SpecificEntityAnnotation::dump(std::ostream& os) const
{
  os << "head:" << m_d->m_head << " type:" << m_d->m_type << " vertices=";
  if (m_d->m_vertices.size() != 0)
  {
    os << m_d->m_vertices[0];
    for (const auto& v : m_d->m_vertices)
    {
      os << "," << v;
    }
  }
}


int DumpSpecificEntityAnnotation::dump(std::ostream& os,
                                       GenericAnnotation& ga) const
{
  PROCESSORSLOGINIT;
  try
  {
    ga.value<SpecificEntityAnnotation>().dump(os);
    return SUCCESS_ID;
  }
  catch (const boost::bad_any_cast& )
  {
    LERROR << "This annotation is not a SpecificEntityAnnotation ; nothing dumped";
    return UNKNOWN_ERROR;
  }
}


} // SpecificEntities
} // LinguisticProcessing
} // Lima
