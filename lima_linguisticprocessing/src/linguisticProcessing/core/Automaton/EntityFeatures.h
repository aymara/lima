// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
#include "linguisticProcessing/LinguisticProcessingCommon.h"

#include <vector>
#include <iostream>

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

#define DEFAULT_ATTRIBUTE "value"
#define UNDEFLENGTH std::numeric_limits<uint64_t>::max()
#define UNDEFPOSITION std::numeric_limits<uint64_t>::max()

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

  void setPosition(const uint64_t& pos) { m_pos=pos; }
  void setLength(const uint64_t& val) { m_len=val; }
  uint64_t getPosition() const { return m_pos; }
  uint64_t getLength() const { return m_len; }

  // these functions test value type using RTTI: this is not really
  // efficient and should be avoided unless necessary (i.e. if the
  // type of the feature is known)
  std::string getValueString() const;
  LimaString getValueLimaString() const;

private:
  std::string m_name;
  boost::any m_value;
  uint64_t m_pos;
  uint64_t m_len;
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
  void setFeature(const std::string& name,
                  const ValueType& value);

  template<typename ValueType>
  void addFeature(const std::string& name,
                  const ValueType& value);

  
  template<typename ValueType>
  void appendFeature(const std::string& name,
                  const ValueType& value);
  

  EntityFeatures::const_iterator find(const std::string& featureName) const;
  EntityFeatures::iterator find(const std::string& featureName);
  EntityFeatures::const_iterator findLast(const std::string& featureName) const;
  EntityFeatures::iterator findLast(const std::string& featureName);

  friend LIMA_AUTOMATON_EXPORT std::ostream& operator<<(std::ostream& os, const EntityFeatures& f);
  friend LIMA_AUTOMATON_EXPORT QDebug& operator<<(QDebug& os, const EntityFeatures& f);
};

#if defined(WIN32)
  extern template LIMA_AUTOMATON_EXPORT void EntityFeatures::appendFeature<int>(const std::string&, const int& );
  extern template LIMA_AUTOMATON_EXPORT void EntityFeatures::appendFeature<double>(const std::string&, const double& );
  extern template LIMA_AUTOMATON_EXPORT void EntityFeatures::appendFeature<QString>(const std::string&, const QString& );
#endif

template<typename ValueType>
  void EntityFeatures::setFeature(const std::string& name,
                  const ValueType& value)
    {
//      SELOGINIT;
//      LDEBUG << "EntityFeatures::<ValueType>setFeature(" << name << "," << value << ")";
      // if feature with same name already exists, overwrite it
      EntityFeatures::iterator it=find(name);
      if (it!=end()) {
//      if( (it!=end()) && (name==DEFAULT_ATTRIBUTE) ){
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
template<typename ValueType>
  void EntityFeatures::addFeature(const std::string& name,
                  const ValueType& value)
    {
//      SELOGINIT;
//      LDEBUG << "EntityFeatures::<ValueType>addFeature(" << name << "," << value << ")";
      push_back(EntityFeature());
      back().setName(name);
      back().setValue(boost::any(value));
    }
/*
template<typename ValueType>
  void EntityFeatures::appendFeature(const std::string& name,
                  const ValueType& value)
    {
      SELOGINIT;
      // if feature with same name already exists, append to it
      LDEBUG << "EntityFeatures::<ValueType>appendFeature(" << name << "," << value << ")";
      // if feature with same name already exists, but type is neither
      // int neitheer float, nor string, overwrite it
      EntityFeatures::iterator it=findLast(name);
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
*/

  
} // end namespace
} // end namespace
} // end namespace

#endif
