// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       entityType.h
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Mon Jan 22 2007
 * copyright   Copyright (C) 2007-2012 by CEA LIST
 * Project     InformationExtraction
 *
 * @brief
 *
 *
 ***********************************************************************/

#ifndef ENTITYTYPE_H
#define ENTITYTYPE_H

#include "common/LimaCommon.h"

#include <iostream>
#include <map>

#include <boost/serialization/strong_typedef.hpp>

namespace Lima {
namespace Common {
namespace MediaticData {

BOOST_STRONG_TYPEDEF(unsigned short, EntityTypeId);
BOOST_STRONG_TYPEDEF(unsigned short, EntityGroupId);

class EntityTypePrivate;
class LIMA_MEDIATICDATA_EXPORT EntityType
{
 public:
  EntityType();
  EntityType(EntityTypeId id, EntityGroupId groupId);
  EntityType& operator=(const EntityType& et);
  EntityType(const EntityType& et);
  ~EntityType();

  bool operator==(const EntityType& other) const;
  bool operator!=(const EntityType& other) const;
  bool operator<(const EntityType& other) const;

  bool isNull() const;

  EntityTypeId getTypeId() const;
  EntityGroupId getGroupId() const;

  void setTypeId(EntityTypeId id);
  void setGroupId(EntityGroupId groupId);

  friend LIMA_MEDIATICDATA_EXPORT QTextStream& operator << (QTextStream&, const EntityType&);
  friend LIMA_MEDIATICDATA_EXPORT std::ostream& operator << (std::ostream&, const EntityType&);
  friend LIMA_MEDIATICDATA_EXPORT QDebug& operator << (QDebug&, const EntityType&);

private:
  EntityTypePrivate* m_d;
};

// hierarchy of entity types
class EntityTypeHierarchyPrivate;
class LIMA_MEDIATICDATA_EXPORT EntityTypeHierarchy
{
public:
  EntityTypeHierarchy();
  ~EntityTypeHierarchy();

  void addParentLink(const EntityType& child, const EntityType& parent);
  bool isParent(const EntityType& child, const EntityType& parent) const;
  bool isAncestor(const EntityType& child, const EntityType& parent) const;
  // get highest ancestor in the hierarchy (return child itself if has no parent)
  EntityType getAncestor(const EntityType& child) const;
  // get the list of nodes under the given ancestor as a map of child-firstParent tuples
  bool getChildren(const EntityType& ancestor, std::map<EntityType,EntityType>& childList) const;

private:
  EntityTypeHierarchyPrivate* m_d;
};

} // end namespace
} // end namespace
} // end namespace

#endif
