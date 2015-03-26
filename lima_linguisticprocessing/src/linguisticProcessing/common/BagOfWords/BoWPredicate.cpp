/*
    Copyright 2014 CEA LIST

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
/************************************************************************
 * @file     BoWPredicate.cpp
 * @author   Besancon Romaric
 * @date     Tue Oct  7 2003
 * copyright Copyright (C) 2003 by CEA LIST
 *
 ***********************************************************************/

#include "BoWPredicate.h"
#include "common/Data/strwstrtools.h"
#include "common/MediaticData/mediaticData.h"

namespace Lima {
namespace Common {
namespace BagOfWords {

class BoWPredicatePrivate
{
public:
  friend class BoWPredicate;

  BoWPredicatePrivate();

  BoWPredicatePrivate(const BoWPredicatePrivate&);
  
  /**
   * get a string of the roles, of the kind :
   * role1=value1;role2=value2
   */
  std::string getRolesUtf8String(void) const;
  
  void setRoles(QMultiMap<Common::MediaticData::EntityType, AbstractBoWElement*> pRoles);


  MediaticData::EntityType m_predicateType;
  uint64_t m_position;
  uint64_t m_length;
  QMultiMap<Common::MediaticData::EntityType, AbstractBoWElement*> m_roles;
};

BoWPredicatePrivate::BoWPredicatePrivate():
m_predicateType(),
m_position(0),
m_length(0),
m_roles()
{
}

BoWPredicatePrivate::BoWPredicatePrivate(const BoWPredicatePrivate& bnep):
m_predicateType(bnep.m_predicateType),
m_position(bnep.m_position),
m_length(bnep.m_length),
m_roles(bnep.m_roles)
{
}

//**********************************************************************
// constructors

BoWPredicate::BoWPredicate():
m_d(new BoWPredicatePrivate())
{
}

BoWPredicate::BoWPredicate(const Common::MediaticData::EntityType theType) :
    m_d(new BoWPredicatePrivate())
{
  m_d->m_predicateType = theType;
}


BoWPredicate::BoWPredicate(const Common::MediaticData::EntityType theType, QMultiMap<Common::MediaticData::EntityType, AbstractBoWElement*> pRoles) :
    m_d(new BoWPredicatePrivate())
{
  m_d->m_predicateType = theType;
  m_d->m_roles = pRoles;
}

BoWPredicate::BoWPredicate(const BoWPredicate& ne):
m_d(new BoWPredicatePrivate(*ne.m_d))
{
}

BoWPredicate::BoWPredicate(BoWPredicatePrivate& d) :
m_d(new BoWPredicatePrivate(d))
{
}


BoWPredicate::~BoWPredicate()
{
  delete m_d;
}

BoWPredicate& BoWPredicate::operator=(const BoWPredicate& t)
{
  if (&t != this) {
    m_d->m_predicateType=t.m_d->m_predicateType;
    m_d->m_roles=t.m_d->m_roles;
    m_d->m_position = t.m_d->m_position;
    m_d->m_length = t.m_d->m_length;
  }
  return *this;
}

bool BoWPredicate::operator==(const BoWPredicate& t)
{
  return( m_d->m_predicateType == t.m_d->m_predicateType 
      && m_d->m_roles == t.m_d->m_roles
      && m_d->m_position == t.m_d->m_position
      && m_d->m_length == t.m_d->m_length);
}

MediaticData::EntityType BoWPredicate::getPredicateType(void) const
{
  return m_d->m_predicateType;
}

void BoWPredicate::setPredicateType(const MediaticData::EntityType& predicateType)
{
  m_d->m_predicateType = predicateType;
}

const QMultiMap<Common::MediaticData::EntityType, AbstractBoWElement*>& BoWPredicate::roles() const
{
  return m_d->m_roles;
}

QMultiMap<Common::MediaticData::EntityType, AbstractBoWElement*>& BoWPredicate::roles()
{
  return m_d->m_roles;
}

void BoWPredicate::setRoles(QMultiMap< Lima::Common::MediaticData::EntityType, Lima::Common::BagOfWords::AbstractBoWElement* >& pRoles){
  m_d->m_roles=pRoles;
}


BoWPredicate* BoWPredicate::clone() const
{
  return new BoWPredicate(*(new BoWPredicatePrivate(*(this->m_d))));
}

std::set< uint64_t > BoWPredicate::getVertices() const
{
  /// FIXME Vertex of predicate is missing from the class definition. Cannot add it.
  std::set< uint64_t > result;
  for (QMultiMap<Common::MediaticData::EntityType, AbstractBoWElement*>::const_iterator it=m_d->m_roles.begin(); it != m_d->m_roles.end(); it++)
  {
    std::set< uint64_t > vertices = (*it)->getVertices();
    for (std::set< uint64_t >::const_iterator it2 = vertices.begin(); it2 != vertices.end(); it2++)
    {
      result.insert(*it2);
    }
  }
  return result;
}

uint64_t BoWPredicate::getPosition(void) const {return m_d->m_position;};
uint64_t BoWPredicate::getLength(void)   const {return m_d->m_length;};
void BoWPredicate::setPosition(const uint64_t pos){m_d->m_position = pos;};
void BoWPredicate::setLength(const uint64_t len)  {m_d->m_length = len;};

//**********************************************************************
// input/output functions
//**********************************************************************
Lima::LimaString BoWPredicate::getString(void) const
{
  return MediaticData::MediaticData::single().getEntityName(static_cast<BoWPredicatePrivate*>(m_d)->m_predicateType);
}

std::string BoWPredicate::getOutputUTF8String(const Common::PropertyCode::PropertyManager* macroManager) const 
{
  std::ostringstream oss;
  oss << Misc::limastring2utf8stdstring(MediaticData::MediaticData::single().getEntityName(static_cast<BoWPredicatePrivate*>(m_d)->m_predicateType)) << ":" << m_d->getRolesUtf8String();
  return oss.str();
}

std::string BoWPredicate::getIdUTF8String() const 
{
  std::ostringstream oss;
  oss << getOutputUTF8String();
  return oss.str();
}

std::string BoWPredicatePrivate::getRolesUtf8String() const 
{
  std::ostringstream oss;
  if (! m_roles.empty()) 
  { 
    QMultiMap<Common::MediaticData::EntityType, AbstractBoWElement*>::const_iterator it=m_roles.begin();   
    oss <<  Misc::limastring2utf8stdstring(MediaticData::MediaticData::single().getEntityName(it.key()))  << "=" 
      << it.value()->getOutputUTF8String();
    it++;
    while (it != m_roles.end()) 
    {
      oss << ";" 
          << Misc::limastring2utf8stdstring(MediaticData::MediaticData::single().getEntityName(it.key()))   << "=" 
        << it.value()->getOutputUTF8String();
      it++;
    }
  }
  return oss.str();
}

//   void BoWPredicatePrivate::setRoles(QMultiMap<Common::MediaticData::EntityType, AbstractBoWElement*> pRoles){
//   QMultiMap<Common::MediaticData::EntityType, AbstractBoWElement*>::const_iterator it=pRoles.begin();
//     while (it != pRoles.end()) 
//     {
//       m_roles.insert(it.key(),it.value());
//     }
//   }


} // namespace BagOfWords
} // namespace Common
} // namespace Lima
