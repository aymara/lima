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
 * @file       EntityFeatures.cpp
 * @author     Besancon Romaric (romaric.besancon@cea.fr)
 * @date       Mon Jun 12 2006
 * @version    $Id$
 * copyright   Copyright (C) 2006-2012 by CEA LIST
 * 
 ***********************************************************************/

#include "EntityFeatures.h"
#include "common/Data/strwstrtools.h"

#include <QtCore/QDate>
#include <QtCore/QTime>

#include "linguisticProcessing/LinguisticProcessingCommon.h"

using namespace std;
using namespace Lima::Common::Misc;

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

//***********************************************************************
// EntityFeature functions

EntityFeature::EntityFeature():
m_name(""),
m_value(),
m_pos(UNDEFPOSITION),
m_len(UNDEFLENGTH)
{}

EntityFeature::~EntityFeature()
{}

bool EntityFeature::operator==(const std::string& name) const
{
  return (m_name==name);
}

bool EntityFeature::operator==(const EntityFeature& f) const
{
  if (m_name!=f.m_name) {
    return false;
  }
  // have to check types 
  const type_info& type=m_value.type();
  const type_info& typeOther=f.m_value.type();
  if (type!=typeOther) {
    return false;
  }

  if (type==typeid(string)) {
    return (boost::any_cast<string>(m_value)==boost::any_cast<string>(f.m_value));
  } 
  if (type==typeid(LimaString)) {
#ifdef DEBUG_LP
    SELOGINIT
    LDEBUG << "EntityFeature::operator==(EntityFeature& f): f.value: " << boost::any_cast<LimaString>(f.m_value);
    LDEBUG << "EntityFeature::operator==(EntityFeature& f): this.value: " << boost::any_cast<LimaString>(m_value);
#endif
    return (boost::any_cast<LimaString>(m_value)==boost::any_cast<LimaString>(f.m_value));
  } 
  if (type==typeid(double)) {
    return (boost::any_cast<double>(m_value)==boost::any_cast<double>(f.m_value));
  }
  if (type==typeid(int)) {
    return (boost::any_cast<int>(m_value)==boost::any_cast<int>(f.m_value));
  }
  if (type==typeid(QDate)) {
    return (boost::any_cast<QDate>(m_value)==boost::any_cast<QDate>(f.m_value));
  }
  if (type==typeid(QTime)) {
    return (boost::any_cast<QTime>(m_value)==boost::any_cast<QTime>(f.m_value));
  }
  // other types not handled
  return false;
}

std::string EntityFeature::getValueString() const { 
  std::ostringstream oss;
  const type_info& type=m_value.type();
  if (type==typeid(std::string)) { // no need to convert
    return boost::any_cast<std::string>(m_value);
  } 
  if (type==typeid(LimaString)) {
    return limastring2utf8stdstring(boost::any_cast<LimaString>(m_value));
  } 
  if (type==typeid(double)) {
    std::ostringstream oss;
    oss << boost::any_cast<double>(m_value); 
    return oss.str();
  }
  if (type==typeid(int)) {
    std::ostringstream oss;
    oss << boost::any_cast<int>(m_value); 
    return oss.str();
  }
  if (type==typeid(QDate)) {
    //format YYYY-MM-DD
    return (boost::any_cast<QDate>(m_value)).toString(Qt::ISODate).toUtf8().data();
  }
  if (type==typeid(QTime)) {
    // format YYYY-MM-DDTHH:MM:SS,fffffffff
    return (boost::any_cast<QTime>(m_value)).toString(Qt::ISODate).toUtf8().data();
  }
  return "unknown_type";
}

LimaString EntityFeature::getValueLimaString() const { 
  std::ostringstream oss;
  const type_info& type=m_value.type();
  if (type==typeid(LimaString)) {
    return boost::any_cast<LimaString>(m_value);
  } 
  return utf8stdstring2limastring(getValueString());
}

//***********************************************************************
// EntityFeatures functions

EntityFeatures::EntityFeatures():
std::vector<EntityFeature>() 
{}

EntityFeatures::~EntityFeatures() 
{}

bool EntityFeatures::operator==(const EntityFeatures& f) const
{
  if (size()!=f.size()) {
    return false;
  }
  for (EntityFeatures::const_iterator it1=begin(),it1_end=end(),it2=f.begin();
       it1!=it1_end; it1++,it2++) {
    if (! (*it1 == *it2) ) {
      return false;
    }
  }
  return true;
}

template<>
  void EntityFeatures::appendFeature(const std::string& name,
                  const QString& value)
    {
      SELOGINIT;
      // if feature with same name already exists, append to it
      LDEBUG << "EntityFeatures<QString>::appendFeature(" << name << "," << value << ")";
      EntityFeatures::iterator it=findLast(name);
      if (it!=end()) {
       QString previous = boost::any_cast<QString>(it->getValue());
       LDEBUG << "EntityFeatures::appendFeature: previous =" << previous;
       previous.append(" ").append(value);
        (*it).setValue(boost::any(previous));
       LDEBUG << "EntityFeatures::appendFeature: now =" << previous;
      }
      else {
        //push empy feature and set values to avoid two copies
        //of value (do not know the type: it may be a big class)
        push_back(EntityFeature());
        back().setName(name);
        back().setValue(boost::any(value));
      }
    }
 
template<>
  void EntityFeatures::appendFeature(const std::string& name,
                  const int& value)
    {
      SELOGINIT;
      LDEBUG << "EntityFeatures<int>::appendFeature(" << name << "," << value << ")";
      // if feature with same name already exists, append to it
      EntityFeatures::iterator it=findLast(name);
      if (it!=end()) {
       int previous = boost::any_cast<int>(*it);
       previous += value;
        (*it).setValue(boost::any(previous));
      }
      else {
        //push empy feature and set values to avoid two copies
        //of value (do not know the type: it may be a big class)
        push_back(EntityFeature());
        back().setName(name);
        back().setValue(boost::any(value));
      }
    }
 
template<>
  void EntityFeatures::appendFeature(const std::string& name,
                  const double& value)
    {
      SELOGINIT;
      LDEBUG << "EntityFeatures<double>::appendFeature(" << name << "," << value << ")";
      // if feature with same name already exists, append to it
      EntityFeatures::iterator it=findLast(name);
      if (it!=end()) {
       double previous = boost::any_cast<double>(*it);
       previous += value;
        (*it).setValue(boost::any(previous));
      }
      else {
        //push empy feature and set values to avoid two copies
        //of value (do not know the type: it may be a big class)
        push_back(EntityFeature());
        back().setName(name);
        back().setValue(boost::any(value));
      }
    }
 
EntityFeatures::const_iterator EntityFeatures::
find(const std::string& featureName) const 
{ 
  return std::find(begin(),end(),featureName);
}

EntityFeatures::iterator EntityFeatures::
find(const std::string& featureName)
{
  return std::find(begin(),end(),featureName);
}

EntityFeatures::const_iterator EntityFeatures::
findLast(const std::string& featureName) const 
{ 
  std::vector<std::string> pattern(1,featureName);
  return std::find_end(begin(),end(),pattern.begin(),pattern.end());
}

EntityFeatures::iterator EntityFeatures::
findLast(const std::string& featureName)
{
  std::vector<std::string> pattern(1,featureName);
  return std::find_end(begin(),end(),pattern.begin(),pattern.end());
}

///***********************************************************************
std::ostream& operator<<(std::ostream& os, const EntityFeatures& f) {
  if (f.empty()) {
    return os;
  }
  EntityFeatures::const_iterator it=f.begin(),it_end=f.end();
  os << (*it).getName() << "=" << (*it).getValueString();
  for (it++; it!=it_end; it++) {
    os << "/" << (*it).getName() << "=" << (*it).getValueString();
  }
  return os;
}

QDebug& operator<<(QDebug& os, const EntityFeatures& f) {
  if (f.empty()) {
      return os;
  }
  EntityFeatures::const_iterator it=f.begin(),it_end=f.end();
  os << (*it).getName() << "=" << (*it).getValueString();
  for (it++; it!=it_end; it++) {
    os << "/" << (*it).getName() << "=" << (*it).getValueString();
  }
  return os;
}


} // end namespace
} // end namespace
} // end namespace
