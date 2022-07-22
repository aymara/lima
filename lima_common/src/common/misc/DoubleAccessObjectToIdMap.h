// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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

  /** @{
   * just check the existence of key or value in the access map
   * 
   * @return 
   */
  bool hasId(const Id&) const;
  bool hasValue(const Object&) const;
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
