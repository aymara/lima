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
 * @file       DoubleAccessObjectToIdMap.tcc
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Tue Jan 23 2007
 * copyright   Copyright (C) 2007-2012 by CEA LIST
 * 
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
  for (typename DoubleAccessObjectToIdMap<Object,Id>::AccessMap::iterator it=m_accessMap.begin(),
         it_end=m_accessMap.end(); it!=it_end; it++) {
    if ((*it).first!=0) {
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
  typename DoubleAccessObjectToIdMap<Object,Id>::AccessMap::const_iterator it=m_accessMap.find(&val);
  if (it==m_accessMap.end()) {
    throw LimaException();
  }
  else {
    return (*it).second;
  }
}

template <typename Object, typename Id>
const Object& DoubleAccessObjectToIdMap<Object,Id>::
get(const Id& id) const
{
  size_t i=(size_t) id;
  if (i >= m_reverseAccessMap.size()) {
    throw LimaException();
  }
  else {
    return *(m_reverseAccessMap[i]);
  }
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
    typename DoubleAccessObjectToIdMap<Object,Id>::AccessMap::iterator it=m_accessMap.find(&val);
  if (it==m_accessMap.end()) {
    // insert it 
#ifdef DEBUG_CD
    LDEBUG << "DoubleAccessObjectToIdMap: new element: insert it";
#endif
    Id id= static_cast<Id>(m_reverseAccessMap.size());
    std::pair<typename DoubleAccessObjectToIdMap<Object,Id>::AccessMap::iterator, bool>
      ret=m_accessMap.insert(std::make_pair(new Object(val),id));
    if (ret.second) {
      typename DoubleAccessObjectToIdMap<Object,Id>::AccessMap::iterator inserted=ret.first;
      m_reverseAccessMap.push_back((*inserted).first);
#ifdef DEBUG_CD
      LDEBUG << "DoubleAccessObjectToIdMap: new element: return "<< (*inserted).second;
#endif
      return (*inserted).second;
    }
    else {
      throw LimaException();
    }
  }
  else {
#ifdef DEBUG_CD
    LDEBUG << "DoubleAccessObjectToIdMap: already inserted element: return "<< (*it).second;
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

// DoubleAccessObjectToIdMap iterator 
 
/*template <typename Object, typename Id>
class DoubleAccessObjectToIdMap<Object,Id>::iterator : 
public std::pair<DoubleAccessObjectToIdMap<Object,Id>::AccessMap::const_iterator,std::vector<Object*>::const_iterator>
{
 public:
  iterator(DoubleAccessObjectToIdMap<Object,Id>::AccessMap::const_iterator it1,
           std::vector<Object*>::const_iterator it2):
    std::pair<DoubleAccessObjectToIdMap<Object,Id>::AccessMap::const_iterator,
              std::vector<Object*>::const_iterator>(it1,it2)
  {}
  
  bool operator==(const DoubleAccessObjectToIdMap::iterator& other) const 
    { return (first==other.first || second==other.second) }
  bool operator!=(const DoubleAccessObjectToIdMap::iterator& other) const 
    { return (first!=other.first && second!=other.second) }
  
  iterator& operator++() {   //prefix ++
    first++;
    second++;
    return *this;
  }
  
  iterator operator++(int) { //postfix++
    iterator it = *this;
    ++(*this);
    return it;
  }
  
  const Object& getObject() {
    return *((*first).first);
  }
  
  const Id& getId() {
    return (*second);
  }
};

template <typename Object, typename Id>
DoubleAccessObjectToIdMap<Object,Id>::iterator 
DoubleAccessObjectToIdMap<Object,Id>::find(const Object& val);
{
  return DoubleAccessObjectToIdMap<Object,Id>::iterator(m_accessMap.find(&val),
                                                 m_reverseAccessMap.end());
}

template <typename Object, typename Id>
DoubleAccessObjectToIdMap<Object,Id>::iterator 
DoubleAccessObjectToIdMap<Object,Id>::find(const Id& val);
{
  return DoubleAccessObjectToIdMap<Object,Id>::iterator(m_accessMap.end(),
                                                        m_reverseAccessMap.find(val));
}

template <typename Object, typename Id>
DoubleAccessObjectToIdMap<Object,Id>::iterator 
begin() { return iterator(m_accessMap.begin(),m_reverseAccessMap.begin()); }

template <typename Object, typename Id>
DoubleAccessObjectToIdMap<Object,Id>::iterator 
end() { return iterator(m_accessMap.end(),m_reverseAccessMap.end()); }
*/
