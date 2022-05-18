// Copyright 2007-2020 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Tue Jan 23 2007
 ***********************************************************************/

//**********************************************************************
// constructors and desctructor

template <typename Object, typename Id>
DoubleAccessObjectToIdMap<Object,Id>::DoubleAccessObjectToIdMap():
    m_accessMap(),
    m_reverseAccessMap()
{
  // keep the id 0 for null object
  m_reverseAccessMap.push_back( static_cast<Object*>(0) );
}

template <typename Object, typename Id>
DoubleAccessObjectToIdMap<Object,Id>::~DoubleAccessObjectToIdMap()
{
  // delete pointers only in map (pointers in vectors points on same objects)
  for (auto it = m_accessMap.begin(), it_end = m_accessMap.end();
       it!=it_end; it++)
  {
    if ((*it).first!=0)
    {
      delete (*it).first;
    }
  }
}

//**********************************************************************
// template member functions

template <typename Object, typename Id>
const Id& DoubleAccessObjectToIdMap<Object,Id>::
get(const Object& val) const
{
  auto it = m_accessMap.find(&val);
  if (it == m_accessMap.end())
  {
    LDATALOGINIT;
    LIMA_EXCEPTION("DoubleAccessObjectToIdMap<Object,Id>::get(val) parameter not in map.");
  }
  return (*it).second;
}

template <typename Object, typename Id>
bool DoubleAccessObjectToIdMap<Object,Id>::
hasValue(const Object& val) const
{
  return (m_accessMap.find(&val) != m_accessMap.end());
}

template <typename Object, typename Id>
bool DoubleAccessObjectToIdMap<Object,Id>::
hasId(const Id& id) const
{
  size_t i=(size_t) id;
  return (i < m_reverseAccessMap.size());
}


template <typename Object, typename Id>
const Object& DoubleAccessObjectToIdMap<Object,Id>::
get(const Id& id) const
{
  size_t i=(size_t) id;
  if (i >= m_reverseAccessMap.size())
  {
    LDATALOGINIT;
    LIMA_EXCEPTION("DoubleAccessObjectToIdMap<Object,Id>::get(id) parameter not in reverse map.");
  }
  return *(m_reverseAccessMap[i]);
}

template <typename Object, typename Id>
Id DoubleAccessObjectToIdMap<Object,Id>::
insert(const Object& val)
{
  // same as [] operator
  return (*this)[val];
}

// implement [] operators for access and insertion
template <typename Object, typename Id>
Id& DoubleAccessObjectToIdMap<Object,Id>::
operator[](const Object& val)
{
#ifdef DEBUG_CD
  LDATALOGINIT;
  LDEBUG << "DoubleAccessObjectToIdMap:operator["
         << val
         << "(" << &val << ")]";
#endif
  auto it = m_accessMap.find(&val);
  if (it == m_accessMap.end())
  {
    // insert it
#ifdef DEBUG_CD
    LDEBUG << "DoubleAccessObjectToIdMap: new element: insert it";
#endif
    Id id= static_cast<Id>(m_reverseAccessMap.size());
    auto ret = m_accessMap.insert(std::make_pair(new Object(val),id));
    if (ret.second)
    {
      auto inserted = ret.first;
      m_reverseAccessMap.push_back((*inserted).first);
#ifdef DEBUG_CD
      LDEBUG << "DoubleAccessObjectToIdMap: new element: return "
              << (*inserted).second;
#endif
      return (*inserted).second;
    }
    else
    {
      LDATALOGINIT;
      LIMA_EXCEPTION("DoubleAccessObjectToIdMap<Object,Id>::operator[](val) parameter not in map");
    }
  }
  else {
#ifdef DEBUG_CD
    LDEBUG << "DoubleAccessObjectToIdMap: already inserted element: return "
            << (*it).second;
#endif
    return (*it).second;
  }
}

template <typename Object, typename Id>
Object& DoubleAccessObjectToIdMap<Object,Id>::operator[](const Id& id)
{
  // same as get
  return get(id);
}

