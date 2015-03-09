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

#ifndef DOUBLEACCESSOBJECTTOIDMAP_H
#define DOUBLEACCESSOBJECTTOIDMAP_H

#include <map>
#include <vector>
#include "common/LimaCommon.h"
#include "common/Data/strwstrtools.h" // to be removed (just for debug)

namespace Lima {
namespace Common {
namespace Misc {

// comparison functions for pointers (for insertion in map)
template<typename Object>
class CompareObjectPtr {
 public:
  bool operator()(const Object* o1, const Object* o2) const {
    return (*o1 < *o2);
  }
};

/**
 *
 * @brief storage for double access: from object to id and from id to
 * object : id must be integer.
 * Objects and ids cannot be of the same type
 *
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Mon Jan 22 2007
 */
template<typename Object, typename Id>
class DoubleAccessObjectToIdMap
{
 public:
  typedef std::map<const Object*,Id,CompareObjectPtr<Object> > AccessMap;
  typedef std::vector<const Object*> ReverseAccessMap;


  DoubleAccessObjectToIdMap(); 
  virtual ~DoubleAccessObjectToIdMap();
  
  /** @{
   * get accessors do not try to insert object if not found:
   * throw an exception
   * 
   * @return 
   */
  const Object& get(const Id&) const;
  const Id& get(const Object&) const;
  /** @} */

  /** 
   * insert an object if not already inserted
   */
  Id insert(const Object&);

  /** @{
   * double access [] operators 
   */
  Id& operator[](const Object&);
  Object& operator[](const Id&);
  /** @} */

  const AccessMap& getAccessMap() const 
    { return m_accessMap; }
  const ReverseAccessMap& getReverseAccessMap() const 
    { return m_reverseAccessMap; }

  /** @{
   * double access find accessors use iterators 
   */
  /*class iterator;
 
  iterator begin();
  iterator end();
  iterator find(const Object& val);
  iterator find(const Id& val);
  */
  
 private:
  // object in map must be a pointer because otherwise,
  // map may change allocation of its elements when inserting
  // new elements => pointers in vectors become invalid
  // 
  AccessMap m_accessMap;
  ReverseAccessMap m_reverseAccessMap;
};

#include "DoubleAccessObjectToIdMap.tcc"


} // end namespace
} // end namespace
} // end namespace

#endif
