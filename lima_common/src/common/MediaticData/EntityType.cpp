// Copyright 2002-2018 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       EntityType.cpp
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Mon Jan 22 2007
 ***********************************************************************/

#include "EntityType.h"
#include "common/LimaCommon.h"

namespace Lima {
namespace Common {
namespace MediaticData {

class EntityTypePrivate
{
  friend class EntityType;
  friend QTextStream& operator << (QTextStream&, const EntityType&);
  friend std::ostream& operator << (std::ostream&, const EntityType&);
  friend QDebug& operator << (QDebug&, const EntityType&);

  EntityTypePrivate();
  EntityTypePrivate(const EntityTypePrivate& etp);
  EntityTypePrivate& operator=(const EntityTypePrivate& etp);
  EntityTypePrivate(EntityTypeId id, EntityGroupId groupId);
  ~EntityTypePrivate();

  EntityTypeId m_id;
  EntityGroupId m_groupId;
};

EntityTypePrivate::EntityTypePrivate():
m_id(0),
m_groupId(0)
{
}

EntityTypePrivate::EntityTypePrivate(const EntityTypePrivate& etp):
m_id(etp.m_id),
m_groupId(etp.m_groupId)
{
}

EntityTypePrivate& EntityTypePrivate::operator=(const EntityTypePrivate& etp)
{
  m_id = etp.m_id;
  m_groupId = etp.m_groupId;
  return *this;
}

EntityTypePrivate::EntityTypePrivate(EntityTypeId id, EntityGroupId groupId):
m_id(id),
m_groupId(groupId)
{
}

EntityTypePrivate::~EntityTypePrivate()
{
}

//***********************************************************************
// constructors and destructors
EntityType::EntityType():
m_d(new EntityTypePrivate())
{
}

EntityType::EntityType(const EntityType& et):
m_d(new EntityTypePrivate(*et.m_d))
{
}

EntityType& EntityType::operator=(const EntityType& et)
{
  *m_d = *et.m_d;
  return *this;
}

EntityType::EntityType(EntityTypeId id, EntityGroupId groupId):
m_d(new EntityTypePrivate(id, groupId))
{
}

EntityType::~EntityType()
{
  delete m_d;
}

bool EntityType::operator==(const EntityType& other) const
{
  return (m_d->m_groupId==other.m_d->m_groupId && m_d->m_id==other.m_d->m_id);
}

bool EntityType::operator!=(const EntityType& other) const
{
  return !(operator==(other));
}

bool EntityType::operator<(const EntityType& other) const
{
  if (m_d->m_groupId < other.m_d->m_groupId) return true;
  if (m_d->m_groupId == other.m_d->m_groupId)
  {
    if (m_d->m_id < other.m_d->m_id)
      return true;
  }
  return false;
}

bool EntityType::isNull() const
{
  return (m_d->m_id==0 && m_d->m_groupId==static_cast<EntityGroupId>(0);
}

EntityTypeId EntityType::getTypeId() const
{
  return m_d->m_id;
}

EntityGroupId EntityType::getGroupId() const
{
  return m_d->m_groupId;
}

void EntityType::setTypeId(EntityTypeId id)
{
  m_d->m_id=id;
}
void EntityType::setGroupId(EntityGroupId groupId)
{
  m_d->m_groupId=groupId;
}

QTextStream& operator << (QTextStream& os, const EntityType& type)
{
  return os << type.m_d->m_groupId << "." << type.m_d->m_id;
}

std::ostream& operator << (std::ostream& os, const EntityType& type)
{
  return os << type.m_d->m_groupId << "." << type.m_d->m_id;
}

QDebug& operator << (QDebug& os, const EntityType& type)
{
  os.nospace() << type.m_d->m_groupId << "." << type.m_d->m_id;
  return os.space();
}

//***********************************************************************
// Hierarchy
// simple child->parent map (does not handle multiple inheritance)
// structure allows entities from different groups to be in the same hierarchy, but this will
// not occur with the current parsing of entity declaration in config file

// inherit from map in case we need specific member functions for the interface
class EntityTypeHierarchyPrivate : public std::map<EntityType, EntityType>
{
public:
  EntityTypeHierarchyPrivate():std::map<EntityType, EntityType>() {}
  ~EntityTypeHierarchyPrivate() {}
};

EntityTypeHierarchy::EntityTypeHierarchy():
m_d(nullptr)
{
  m_d=new EntityTypeHierarchyPrivate();
}

EntityTypeHierarchy::~EntityTypeHierarchy()
{
  delete m_d;
}

void EntityTypeHierarchy::addParentLink(const EntityType& child,
                                        const EntityType& parent)
{
  (*m_d)[child]=parent;
}

bool EntityTypeHierarchy::isParent(const EntityType& child,
                                   const EntityType& parent) const
{
  const auto& it=m_d->find(child);
  if (it==m_d->end())
  {
    return false;
  }
  return ((*it).second==parent);
}

bool EntityTypeHierarchy::isAncestor(const EntityType& child,
                                     const EntityType& parent) const
{
  const auto& it=m_d->find(child);
  if (it==m_d->end())
  {
    return false;
  }
  if ((*it).second==parent)
  {
    return true;
  }
  return isAncestor((*it).second,parent);
}

EntityType EntityTypeHierarchy::getAncestor(const EntityType& child) const
{
  // get oldest ancestor
  EntityType current=child;
  auto it=m_d->find(current);
  while (it != m_d->end()) {
    current=(*it).second;
    it=m_d->find(current);
  }
  return current; // if no parent, return type itself
}

EntityType EntityTypeHierarchy::getParent(const EntityType& child) const
{
  // get first ancestor
  EntityType current=child;
  auto it=m_d->find(current);
  if (it != m_d->end()) {
    current=(*it).second;
  }
  return current; // if no parent, return type itself
}


bool EntityTypeHierarchy::getChildren(const EntityType& ancestor, std::map<EntityType,EntityType>& childList) const
{
  for(auto it = m_d->begin(); it != m_d->end(); it++){
      if( isParent( (*it).first, ancestor) ){
          childList.insert( std::make_pair( (*it).first, (*it).second) );
          getChildren((*it).first, childList);
      }
  }
  return true;
}

} // end namespace
} // end namespace
} // end namespace
