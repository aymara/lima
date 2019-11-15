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

// hierachy of entity types 
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
 
private:
  EntityTypeHierarchyPrivate* m_d;
};

} // end namespace
} // end namespace
} // end namespace

#endif
