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
 * @file       EntityFeatures.h
 * @author     Besancon Romaric (romaric.besancon@cea.fr)
 * @date       Mon Jun 12 2006
 * @version    $Id$
 * copyright   Copyright (C) 2006-2012 by CEA LIST
 * Project     s2lp - Automaton
 * 
 * @brief generic features associated to a recognized entity :
 * normalized information attached to the entity
 * 
 * 
 ***********************************************************************/

#ifndef ENTITYFEATURES_H
#define ENTITYFEATURES_H

#include "AutomatonExport.h"
#include "common/Data/LimaString.h"
#include "boost/any.hpp"
#include "common/misc/gregoriannowarn.hpp"

#include <vector>
#include <iostream>

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

#define DEFAULT_ATTRIBUTE "value"

/** 
 * @brief a generic feature that is defined by a name and a value, which can be
 * of any type (use boost::any type)
 */
class LIMA_AUTOMATON_EXPORT EntityFeature {
public:
  EntityFeature();
  template<typename ValueType> 
    EntityFeature(const std::string& name, 
                  const ValueType& value):
    m_name(name),
    m_value(boost::any(value))
  {}
  ~EntityFeature();
  
  bool operator==(const EntityFeature& f) const;
  // comparison operator on name only
  bool operator==(const std::string& name) const;

  void setName(const std::string& name) { m_name=name; }
  void setValue(const boost::any& val) { m_value=val; }

  const std::string& getName() const { return m_name; }
  const boost::any& getValue() const { return m_value; }
  boost::any getValue() { return m_value; }

  void setPosition(const uint64_t& pos) { m_pos=pos; m_hasPos=true; }
  void setLength(const uint64_t& val) { m_length=val; }
  uint64_t getPosition() const { return m_pos; }
  uint64_t getLength() const { return m_length; }
  uint64_t hasPos() const { return m_hasPos; }

  // these functions test value type using RTTI: this is not really
  // efficient and should be avoided unless necessary (i.e. if the
  // type of the feature is known)
  std::string getValueString() const;
  LimaString getValueLimaString() const;

private:
  std::string m_name;
  boost::any m_value;
  bool m_hasPos;
  uint64_t m_pos;
  uint64_t m_length;
};

/** 
 * @brief a list of generic features: each feature is unique (only one
 * feature for a name)
 */
class LIMA_AUTOMATON_EXPORT EntityFeatures: public std::vector<EntityFeature> {
public:
  EntityFeatures();
  ~EntityFeatures();
  bool operator==(const EntityFeatures& f) const;

  template<typename ValueType>
  void addFeature(const std::string& name,
                  const ValueType& value)
    {
      // if feature with same name already exists, overwrite it
      EntityFeatures::iterator it=find(name);
      if (it!=end()) {
        (*it).setValue(boost::any(value));
      }
      else {
        //push empy feature and set values to avoid two copies
        //of value (do not know the type: it may be a big class)
        push_back(EntityFeature());
        back().setName(name);
        back().setValue(boost::any(value));
      }
    }
  

  EntityFeatures::const_iterator find(const std::string& featureName) const;
  EntityFeatures::iterator find(const std::string& featureName);

  friend LIMA_AUTOMATON_EXPORT std::ostream& operator<<(std::ostream& os, const EntityFeatures& f);
};

} // end namespace
} // end namespace
} // end namespace

#endif
