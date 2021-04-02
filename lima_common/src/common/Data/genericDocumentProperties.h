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
 * @file       documentProperties.h
 * @author     Besancon Romaric (besanconr@zoe.cea.fr)
 * @date       Fri Oct 10 2003
 * @version    $Id$
 * copyright   Copyright (C) 2003-2012 by CEA LIST
 * Project     BagOfWords
 *
 * @brief      representation of generic properties of a document
 *
 *
 ***********************************************************************/

#ifndef GENERICDOCUMENTPROPERTIES_H
#define GENERICDOCUMENTPROPERTIES_H

#include "common/LimaCommon.h"

#include <QtCore/QDate>

#include <iostream>
#include <string>
#include <vector>
#include <map>

namespace Lima {
namespace Common {
namespace Misc {

/**
* @brief Holds document properties
*/
template<typename ValT>
class MultiValuedPropertyIterator {
public:
  MultiValuedPropertyIterator(const typename std::map<std::string, std::vector<ValT> >::const_iterator pos);
  MultiValuedPropertyIterator(const MultiValuedPropertyIterator& orig);
  MultiValuedPropertyIterator& operator = (const MultiValuedPropertyIterator& orig);
  MultiValuedPropertyIterator& operator++(int);
  bool operator != (const MultiValuedPropertyIterator& orig ) const;
  bool operator == (const MultiValuedPropertyIterator& orig ) const;
  const std::string& operator *() const;
private:
  typename std::map<std::string,std::vector<ValT> >::const_iterator m_pos;
};

typedef std::vector<std::string>::const_iterator StringPropMultIter;
typedef std::vector<std::pair<std::string,float> >::const_iterator WeightedPropMultIter;

class GenericDocumentPropertiesPrivate;
class LIMA_DATA_EXPORT GenericDocumentProperties
{
  friend class GenericDocumentPropertiesPrivate;
 public:
  GenericDocumentProperties();
  GenericDocumentProperties(const GenericDocumentProperties&);
  virtual ~GenericDocumentProperties();
  GenericDocumentProperties& operator = (const GenericDocumentProperties&);

  virtual void reinit();

  virtual void read(std::istream& file);
  void write(std::ostream& file) const;

  friend LIMA_DATA_EXPORT std::ostream& operator << (std::ostream&, const GenericDocumentProperties&);
  friend LIMA_DATA_EXPORT QDebug& operator << (QDebug&, const GenericDocumentProperties&);

  virtual void print(std::ostream&) const;
  virtual void print(QDebug&) const;

  /**
   *  Read access functions for "only once valued" property propName.
   *  If property exists, return a pair (value,true) else return (default value,false)
   *  Types may be any of:
   *  - integer (uint64_t),
   *  - string (std::string),
   *  - date (QDate)
   *  - period (std::pair<QDate,QDate>) used for approximativ date
  **/
  std::pair<uint64_t, bool> getIntValue(std::string propName) const;
  std::pair<std::string, bool> getStringValue(std::string propName) const;
  std::pair<QDate, bool> getDateValue(std::string propName) const;
  std::pair<std::pair<QDate,QDate>, bool> getDateIntervalValue(std::string propName) const;

  /**
   *  Read access functions for "multiple-valued" property propName
   *  Return a pair of iterator (begin,end) to iterate through values
   *  types may be any of:
   *  - string (std::string),
   *  - weighted property (std::pair<std::string,float) used for result of filtering and categorization
  **/
  std::pair< StringPropMultIter, StringPropMultIter > getMultipleStringPropValue(std::string propName) const;
  std::pair< WeightedPropMultIter, WeightedPropMultIter > getMultipleWeightedPropValue(std::string propName) const;

  /**
   *  Read access functions for all "only once valued" properties
   *  return an iterator
   *  Types may be any of (uint64_t), (std::string), (QDate) or
   *   (std::pair<QDate,QDate>) used for approximativ date
  **/
  typedef std::map<std::string,uint64_t>::const_iterator IntPropertiesIterator;
  std::pair<IntPropertiesIterator,IntPropertiesIterator> getIntProperties() const;

  typedef std::map<std::string,std::string>::const_iterator StringPropertiesIterator;
  std::pair<StringPropertiesIterator,StringPropertiesIterator> getStringProperties() const;

  typedef std::map<std::string,QDate>::const_iterator DatePropertiesIterator;
  std::pair<DatePropertiesIterator,DatePropertiesIterator> getDateProperties() const;

   typedef std::map<std::string,std::pair<QDate,QDate> >::const_iterator DateIntervalPropertiesIterator;
  std::pair<DateIntervalPropertiesIterator,DateIntervalPropertiesIterator> getDateIntervalProperties() const;

  /**
   *  Read access functions for all "mutiple valued" perties
   *  return an iterator of names
   *  Types may be any of (std::string) or (std::pair<std::string,float)
  **/
  std::pair<MultiValuedPropertyIterator<std::string>,MultiValuedPropertyIterator<std::string> > getStringPropertyNames() const;
  std::pair<MultiValuedPropertyIterator<std::pair<std::string,float> >,MultiValuedPropertyIterator<std::pair<std::string,float> > > getWeightedPropPropertyNames() const;
  void setIntValue(const std::string& propName, const uint64_t& val);
  void setStringValue(const std::string& propName, const std::string& val);
  void setDateValue(const std::string& propName, const QDate& val);
  void setDateIntervalValue(const std::string& propName, const std::pair<QDate,QDate>& val);

  void addStringValue(const std::string& propName, const std::string& str);
  void addWeightedPropValue(const std::string& propName, const std::pair<std::string,float>& str);

 protected:
   GenericDocumentPropertiesPrivate* m_d;
};

//***********************************************************************
// iterator for multi-valued properties
//***********************************************************************
template<typename ValT>
// Constructor
MultiValuedPropertyIterator<ValT>::MultiValuedPropertyIterator(
  const typename std::map<std::string,std::vector<ValT> >::const_iterator pos)
  : m_pos(pos) {
}

template<typename ValT>
MultiValuedPropertyIterator<ValT>::MultiValuedPropertyIterator(
  const MultiValuedPropertyIterator& orig)
  : m_pos(orig.m_pos) {
}

template<typename ValT>
MultiValuedPropertyIterator<ValT>& MultiValuedPropertyIterator<ValT>::operator = (
 const MultiValuedPropertyIterator<ValT>& orig ) {
  m_pos = orig.m_pos;
  return *this;
}

template<typename ValT>
MultiValuedPropertyIterator<ValT>& MultiValuedPropertyIterator<ValT>::operator++ (int) {
  m_pos++;
  return *this;
}

template<typename ValT>
bool MultiValuedPropertyIterator<ValT>::operator != ( const MultiValuedPropertyIterator<ValT>& orig ) const {
  return( m_pos != orig.m_pos );
}

template<typename ValT>
bool MultiValuedPropertyIterator<ValT>::operator == ( const MultiValuedPropertyIterator<ValT>& orig ) const {
  return( m_pos == orig.m_pos );
}

template<typename ValT>
const std::string& MultiValuedPropertyIterator<ValT>::operator *() const {
  return( (*m_pos).first );
}


} // end namespace Misc
} // end namespace Common
} // end namespace Lima

#endif
