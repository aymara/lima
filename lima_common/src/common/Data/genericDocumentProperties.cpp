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
 * @file       documentProperties.cpp
 * @author     Besancon Romaric (besanconr@zoe.cea.fr)
 * @date       Fri Oct 10 2003
 * @version    $Id$
 * copyright   Copyright (C) 2003-2012 by CEA LIST
 *
 ***********************************************************************/

#include "genericDocumentProperties.h"
#include "strwstrtools.h"

#include <QtCore/QDate>

#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;

namespace Lima {
namespace Common {
namespace Misc {

class GenericDocumentPropertiesPrivate
{
  friend class GenericDocumentProperties;

  GenericDocumentPropertiesPrivate();
  GenericDocumentPropertiesPrivate(const GenericDocumentPropertiesPrivate&);
  virtual ~GenericDocumentPropertiesPrivate();

  std::map<std::string,uint64_t> m_intValues;
  std::map<std::string,std::string> m_stringValues;
  std::map<std::string,QDate> m_dateValues;
  std::map<std::string,std::pair<QDate,QDate> > m_dateIntervalValues;
  // properties with several values
  std::map<std::string,std::vector<std::string> > m_multipleStringValues;
  std::map<std::string,std::vector<std::pair<std::string,float> > > m_multipleWeightedPropValues;

  QDate readDate(std::istream& file);
  void writeDate(std::ostream& file,const QDate& d) const;
  std::vector<std::string> m_noStringVal;
  std::vector<std::pair<std::string,float> > m_noWeightedPropVal;
};

GenericDocumentPropertiesPrivate::GenericDocumentPropertiesPrivate(){
}

GenericDocumentPropertiesPrivate::GenericDocumentPropertiesPrivate(const GenericDocumentPropertiesPrivate& p):
m_intValues(p.m_intValues),
m_stringValues(p.m_stringValues),
m_dateValues(p.m_dateValues),
m_dateIntervalValues(p.m_dateIntervalValues),
m_multipleStringValues(p.m_multipleStringValues),
m_multipleWeightedPropValues(p.m_multipleWeightedPropValues)
{
}

//***********************************************************************
// destructor
//***********************************************************************
GenericDocumentPropertiesPrivate::~GenericDocumentPropertiesPrivate() {
}


//***********************************************************************
// constructors
//***********************************************************************
GenericDocumentProperties::GenericDocumentProperties() :
    m_d(new GenericDocumentPropertiesPrivate())
{
}

GenericDocumentProperties::GenericDocumentProperties(const GenericDocumentProperties& p):
    m_d(new GenericDocumentPropertiesPrivate(*p.m_d))
{
}

//***********************************************************************
// destructor
//***********************************************************************
GenericDocumentProperties::~GenericDocumentProperties()
{
  delete m_d;
}

//***********************************************************************
// assignment operator
//***********************************************************************
GenericDocumentProperties& GenericDocumentProperties::operator = (const GenericDocumentProperties& p) {
  if (&p != this ) {
    m_d->m_intValues=p.m_d->m_intValues;
    m_d->m_stringValues=p.m_d->m_stringValues;
    m_d->m_dateValues=p.m_d->m_dateValues;
    m_d->m_dateIntervalValues=p.m_d->m_dateIntervalValues;
    m_d->m_multipleStringValues=p.m_d->m_multipleStringValues;
    m_d->m_multipleWeightedPropValues=p.m_d->m_multipleWeightedPropValues;
  }
  return *this;
}

void GenericDocumentProperties::setStringValue(const std::string& propName,
                                               const std::string& str)
{ m_d->m_stringValues[propName]=str; }

void GenericDocumentProperties::setIntValue(const std::string& propName,
                                            const uint64_t& val)
{ m_d->m_intValues[propName]=val; }

void GenericDocumentProperties::setDateValue(const std::string& propName,
                                             const QDate& val)
{ m_d->m_dateValues[propName]=val; }

void GenericDocumentProperties::setDateIntervalValue(const std::string& propName,
       const std::pair<QDate,QDate>& val)
{ m_d->m_dateIntervalValues[propName]=val; }


std::pair<uint64_t, bool>
GenericDocumentProperties::getIntValue(std::string propName) const
{
  std::map<std::string,uint64_t>::const_iterator pos = m_d->m_intValues.find(propName);
  if( pos == m_d->m_intValues.end() ) {
    return( std::pair<uint64_t, bool>(0,false) );
  }
  else {
    return( std::pair<uint64_t, bool>((*pos).second,true) );
  }
}

std::pair<std::string, bool>
GenericDocumentProperties::getStringValue(std::string propName) const
{
  std::map<std::string,std::string>::const_iterator pos = m_d->m_stringValues.find(propName);
  if( pos == m_d->m_stringValues.end() ) {
    return( std::pair<std::string, bool>("",false) );
  }
  else {
    return( std::pair<std::string, bool>((*pos).second,true) );
  }
}

std::pair<QDate, bool>
GenericDocumentProperties::getDateValue(std::string propName) const
{
  std::map<std::string,QDate>::const_iterator pos = m_d->m_dateValues.find(propName);
  if( pos == m_d->m_dateValues.end() ) {
    return( std::pair<QDate, bool>(QDate(),false) );
  }
  else {
    return( std::pair<QDate, bool>((*pos).second,true) );
  }
}

std::pair<std::pair<QDate,QDate>, bool>
GenericDocumentProperties::getDateIntervalValue(std::string propName) const
{
  std::map<std::string,std::pair<QDate,QDate> >::const_iterator pos = m_d->m_dateIntervalValues.find(propName);
  if( pos == m_d->m_dateIntervalValues.end() ) {
    std::pair<QDate,QDate> defaultInterval;
//      (QDate(),QDate());
    return( std::pair<std::pair<QDate,QDate>, bool>(defaultInterval,false) );
  }
  else {
    return( std::pair<std::pair<QDate,QDate>, bool>((*pos).second,true) );
  }
}

std::pair<StringPropMultIter,StringPropMultIter> GenericDocumentProperties::getMultipleStringPropValue(std::string propName) const {
  std::map<std::string,std::vector<std::string> >::const_iterator pos = m_d->m_multipleStringValues.find(propName);
  if( pos == m_d->m_multipleStringValues.end() ) {
    return std::pair<StringPropMultIter, StringPropMultIter>(m_d->m_noStringVal.begin(), m_d->m_noStringVal.end());
  }
  else {
    const std::vector<std::string>& val = (*pos).second;
    return std::pair<StringPropMultIter, StringPropMultIter>(val.begin(),val.end());
  }
}

std::pair<WeightedPropMultIter,WeightedPropMultIter> GenericDocumentProperties::getMultipleWeightedPropValue(std::string propName) const {
  std::map<std::string,std::vector<std::pair<std::string,float> > >::const_iterator pos =
     m_d->m_multipleWeightedPropValues.find(propName);
  if( pos == m_d->m_multipleWeightedPropValues.end() ) {
    return std::pair<WeightedPropMultIter, WeightedPropMultIter>(m_d->m_noWeightedPropVal.begin(), m_d->m_noWeightedPropVal.end());
  }
  else {
    const std::vector<std::pair<std::string,float> >& val = (*pos).second;
    return std::pair<WeightedPropMultIter, WeightedPropMultIter>(val.begin(),val.end());
  }
}

std::pair<GenericDocumentProperties::IntPropertiesIterator,GenericDocumentProperties::IntPropertiesIterator> GenericDocumentProperties::getIntProperties() const {
  return  std::make_pair(m_d->m_intValues.begin(),m_d->m_intValues.end());
}

std::pair<GenericDocumentProperties::StringPropertiesIterator,GenericDocumentProperties::StringPropertiesIterator> GenericDocumentProperties::getStringProperties() const {
  return  std::make_pair(m_d->m_stringValues.begin(),m_d->m_stringValues.end());
}

std::pair<GenericDocumentProperties::DatePropertiesIterator,GenericDocumentProperties::DatePropertiesIterator> GenericDocumentProperties::getDateProperties() const {
  return  std::make_pair(m_d->m_dateValues.begin(),m_d->m_dateValues.end());
}

std::pair<GenericDocumentProperties::DateIntervalPropertiesIterator,GenericDocumentProperties::DateIntervalPropertiesIterator> GenericDocumentProperties::getDateIntervalProperties() const {
  return  std::make_pair(m_d->m_dateIntervalValues.begin(),m_d->m_dateIntervalValues.end());
}

std::pair<MultiValuedPropertyIterator<std::string>,MultiValuedPropertyIterator<std::string> > GenericDocumentProperties::getStringPropertyNames() const {
  return std::make_pair(
    MultiValuedPropertyIterator<std::string>(m_d->m_multipleStringValues.begin()),
    MultiValuedPropertyIterator<std::string>(m_d->m_multipleStringValues.end()) );
}

 std::pair<MultiValuedPropertyIterator<std::pair<std::string,float> >,
           MultiValuedPropertyIterator<std::pair<std::string,float> > > GenericDocumentProperties::getWeightedPropPropertyNames() const {
  return std::make_pair(
    MultiValuedPropertyIterator<std::pair<std::string,float> >(m_d->m_multipleWeightedPropValues.begin()),
    MultiValuedPropertyIterator<std::pair<std::string,float> >(m_d->m_multipleWeightedPropValues.end()) );
}

void GenericDocumentProperties::addStringValue(const std::string& propName,
                                               const std::string& val)
{
  std::map<std::string,std::vector<std::string> >::iterator pos = m_d->m_multipleStringValues.find(propName);
  if( pos == m_d->m_multipleStringValues.end() ) {
    std::vector<std::string> values;
    values.push_back(val);
    m_d->m_multipleStringValues.insert(std::pair<std::string,std::vector<std::string> >(propName,values) );
  }
  else {
    std::vector<std::string>& values = (*pos).second;
    auto iter = std::find(values.begin(), values.end(), val);
    if (iter == values.end()){
        values.push_back(val);
    }
    // else avoid duplicate entries: Do not append
  }
}


bool mypredicate(const std::pair<std::string,float>& a, const std::pair<std::string,float>& b){
  return a.first == b.first;
}

void GenericDocumentProperties::addWeightedPropValue(const std::string& propName,
                                               const std::pair<std::string,float>& val)
{
  std::map<std::string,std::vector<std::pair<std::string,float> > >::iterator pos = m_d->m_multipleWeightedPropValues.find(propName);
  if( pos == m_d->m_multipleWeightedPropValues.end() ) {
    std::vector<std::pair<std::string,float> > values;
    values.push_back(val);
    m_d->m_multipleWeightedPropValues.insert(std::pair<std::string,std::vector<std::pair<std::string,float> > >(propName,values) );
  }
  else {
    std::vector<std::pair<std::string,float> >& values = (*pos).second;

    const std::pair<std::string,float> needle [1] = { val };
    auto iter = std::search(values.begin(), values.end(), needle, needle+1, mypredicate);
    if (iter == values.end()){
      values.push_back(val);
    }
    else // else avoid duplicate entries: Do not append, but overwrite the associated weight
    {
      iter->second = val.second;
    }

  }
}


//***********************************************************************
// binary read/write functions
//***********************************************************************
void GenericDocumentProperties::reinit() {
  m_d->m_intValues.clear();
  m_d->m_stringValues.clear();
  m_d->m_dateValues.clear();
  m_d->m_dateIntervalValues.clear();
  m_d->m_multipleStringValues.clear();
  m_d->m_multipleWeightedPropValues.clear();
}


//***********************************************************************
// binary read/write functions
//***********************************************************************
QDate GenericDocumentPropertiesPrivate::readDate(std::istream& file) {
  // read date as three unsigned short if defined
  bool isADate(true);
  file.read((char*) &isADate, sizeof(bool));
  if (isADate) {
    unsigned short day(0);
    unsigned short month(0);
    unsigned short year(0);
    file.read((char*) &day, sizeof(unsigned short));
    file.read((char*) &month, sizeof(unsigned short));
    file.read((char*) &year, sizeof(unsigned short));
    return QDate(year,month,day);
  }
  else {
    return QDate();
  }
}

void GenericDocumentPropertiesPrivate::writeDate(std::ostream& file,
                                   const QDate& d) const {
  // write date as three unsigned short if defined
  if (!d.isValid()) {
    bool isADate(false);
    file.write((char*) &isADate, sizeof(bool));
  }
  else {
    bool isADate(true);
    file.write((char*) &isADate, sizeof(bool));
    unsigned short day=d.day();
    unsigned short month=d.month();
    unsigned short year=d.year();
    file.write((char*) &day, sizeof(unsigned short));
    file.write((char*) &month, sizeof(unsigned short));
    file.write((char*) &year, sizeof(unsigned short));
  }
}

void GenericDocumentProperties::read(std::istream& file) {
  uint32_t size(0);

  // clean structures
  m_d->m_intValues.clear();
  m_d->m_stringValues.clear();
  m_d->m_dateValues.clear();
  m_d->m_dateIntervalValues.clear();
  m_d->m_multipleStringValues.clear();
  m_d->m_multipleWeightedPropValues.clear();

#ifdef DEBUG_CD
  BOWLOGINIT;
  LDEBUG << "GenericDocumentProperties::read()";
#endif
  // read integer properties
  file.read((char*) &size, sizeof(uint32_t));
#ifdef DEBUG_CD
  LDEBUG << "read" << size << "int properties";
#endif
  for (uint32_t i(0); i<size; i++) {
    string name;
    Misc::readStringField(file,name);
    uint64_t val;
    file.read((char*) &val, sizeof(uint64_t));
#ifdef DEBUG_CD
    LDEBUG << "read int '" << val << "' as value of " << name.c_str();
#endif
    m_d->m_intValues.insert(std::pair<std::string,uint64_t>(name,val));
  }

  // read string properties
  file.read((char*) &size, sizeof(uint32_t));
#ifdef DEBUG_CD
  LDEBUG << "read" << size << "string properties";
#endif
  for (uint32_t i(0); i<size; i++) {
    string name;
    Misc::readStringField(file,name);
    string str;
    Misc::readStringField(file,str);
#ifdef DEBUG_CD
    LDEBUG << "read string '" << str.c_str() << "' as value of " << name.c_str();
#endif
    m_d->m_stringValues.insert(std::pair<std::string,std::string>(name,str) );
  }

  // read date properties
  file.read((char*) &size, sizeof(uint32_t));
#ifdef DEBUG_CD
  LDEBUG << "read" << size << "date properties";
#endif
  for (uint32_t i(0); i<size; i++) {
    string name;
    Misc::readStringField(file,name);
    QDate d=m_d->readDate(file);
    string strDate=d.toString().toUtf8().data();
#ifdef DEBUG_CD
    LDEBUG << "read date '" << strDate.c_str() << "' as value of " << name.c_str();
#endif
    m_d->m_dateValues.insert(std::pair<std::string,QDate>(name,d));
  }

  // read date interval properties
  file.read((char*) &size, sizeof(uint32_t));
#ifdef DEBUG_CD
  LDEBUG << "read" << size << "date-interval properties";
#endif
  for (uint32_t i(0); i<size; i++) {
    string name;
    Misc::readStringField(file,name);
    QDate startD=m_d->readDate(file);
    QDate endD=m_d->readDate(file);
    string strStartDate=startD.toString().toUtf8().data();
    string strEndDate=endD.toString().toUtf8().data();
#ifdef DEBUG_CD
    LDEBUG << "read date-interval [" << strStartDate.c_str() << "," << strEndDate.c_str() << "] as value of " << name.c_str();
#endif
    std::pair<QDate,QDate> interval(startD,endD);
    m_d->m_dateIntervalValues.insert(std::pair<std::string,std::pair<QDate,QDate> >(name,interval));
  }

  // read multi-valued string properties
  file.read((char*) &size, sizeof(uint32_t));
#ifdef DEBUG_CD
  LDEBUG << "read" << size << "multi-valued properties";
#endif
  for (uint32_t i(0); i<size; i++) {
    string name;
    Misc::readStringField(file,name);
    uint32_t sizeMultipleValues(0);
    file.read((char*) &sizeMultipleValues, sizeof(uint32_t));
    vector<string> val;
    for (uint32_t j(0); j<sizeMultipleValues; j++) {
      string str;
      Misc::readStringField(file,str);
#ifdef DEBUG_CD
      LDEBUG << "read string '" << str.c_str() <<"' as "<<j<<"/"<<sizeMultipleValues<< " value of" << name.c_str();
#endif
      val.push_back(str);
    }
    m_d->m_multipleStringValues.insert(std::pair<std::string,std::vector<std::string> >(name,val) );
  }

  // read multi-valued weighted properties
  file.read((char*) &size, sizeof(uint32_t));
//   LDEBUG << "read size " << size;
  for (uint32_t i(0); i<size; i++) {
    string name;
    Misc::readStringField(file,name);
    uint32_t sizeMultipleValues(0);
    file.read((char*) &sizeMultipleValues, sizeof(uint32_t));
    vector<std::pair<std::string,float> > val;
    for (uint32_t j(0); j<sizeMultipleValues; j++) {
      string str;
      Misc::readStringField(file,str);
      float weight;
      file.read((char*) &weight, sizeof(float));
      val.push_back(std::pair<std::string,float>(str,weight) );
#ifdef DEBUG_CD
      LDEBUG << "read weighted-string '" << str.c_str() <<":"<<weight<<"' as "<<j<<"/"<<sizeMultipleValues<< " value of" << name.c_str();
#endif
    }
    m_d->m_multipleWeightedPropValues.insert(std::pair<std::string,vector<std::pair<std::string,float> > > (name,val) );
  }

#ifdef DEBUG_CD
      LDEBUG << "end of properties";
#endif
}

void GenericDocumentProperties::write(std::ostream& file) const {
  std::map <std::string, uint64_t >::size_type size(0);

#ifdef DEBUG_CD
  BOWLOGINIT;
  LDEBUG << "GenericDocumentProperties::write()";
#endif

  // Write integer properties
  size=m_d->m_intValues.size();
#ifdef DEBUG_CD
  LDEBUG << "write" << size << "int properties";
#endif
  file.write((char*) &size, sizeof(uint32_t));
  for (std::map<std::string,uint64_t>::const_iterator it = m_d->m_intValues.begin() ;
     it != m_d->m_intValues.end() ; it++) {
    Misc::writeStringField(file,(*it).first);
#ifdef DEBUG_CD
    LDEBUG << "write int name" << (*it).first.c_str() << "value" << (*it).second;
#endif
    file.write((char*) &((*it).second), sizeof(uint64_t));
  }
//  Misc::writeStringField(file,"eoi");

  // Write string properties
  size=m_d->m_stringValues.size();
#ifdef DEBUG_CD
  LDEBUG << "write" << size << "string properties";
#endif
  file.write((char*) &size, sizeof(uint32_t));
  for (std::map<std::string,std::string>::const_iterator it = m_d->m_stringValues.begin() ;
    it != m_d->m_stringValues.end() ; it++) {
    Misc::writeStringField(file,(*it).first);
#ifdef DEBUG_CD
  LDEBUG << "write string name" << (*it).first.c_str() << "value" << (*it).second;
#endif
    Misc::writeStringField(file,(*it).second);
  }
//  Misc::writeStringField(file,"eos");

  // Write date properties
  size=m_d->m_dateValues.size();
#ifdef DEBUG_CD
  LDEBUG << "write" << size << "date properties";
#endif
  file.write((char*) &size, sizeof(uint32_t));
  for (std::map<std::string,QDate>::const_iterator it = m_d->m_dateValues.begin() ;
     it != m_d->m_dateValues.end() ; it++) {
    Misc::writeStringField(file,(*it).first);
    //string strDate=(*it).second.toString().toUtf8().data();
#ifdef DEBUG_CD
  LDEBUG << "write date name" << (*it).first.c_str() << "value" << (*it).second;
#endif
    m_d->writeDate(file,(*it).second);
  }
//  Misc::writeStringField(file,"eod");

  // Write date interval properties
  size=m_d->m_dateIntervalValues.size();
#ifdef DEBUG_CD
  LDEBUG << "write" << size << "date-interval properties";
#endif
  file.write((char*) &size, sizeof(uint32_t));
  for (std::map<std::string,std::pair<QDate,QDate> >::const_iterator it
     = m_d->m_dateIntervalValues.begin() ;  it != m_d->m_dateIntervalValues.end() ; it++) {
    const std::pair<std::string,std::pair<QDate,QDate> >&val=*it;
    Misc::writeStringField(file,val.first);
    //string strStartDate=(val.second).first.toString().toUtf8().data();
    //string strEndDate=(val.second).second.toString().toUtf8().data();
#ifdef DEBUG_CD
  LDEBUG << "write date-interval name [" << (val.second).first << "," << (val.second).second << "]";
#endif
    m_d->writeDate(file,(val.second).first);
    m_d->writeDate(file,(val.second).second);
  }

  // Write multiple string properties
  size=m_d->m_multipleStringValues.size();
#ifdef DEBUG_CD
  LDEBUG << "write" << size << "multi-valued properties";
#endif
  file.write((char*) &size, sizeof(uint32_t));
  for (std::map<std::string,std::vector<std::string> >::const_iterator it = m_d->m_multipleStringValues.begin() ;
     it != m_d->m_multipleStringValues.end() ; it++) {
#ifdef DEBUG_CD
  LDEBUG << "write multi-valued  name" << (*it).first.c_str();;
#endif
    Misc::writeStringField(file,(*it).first);
    const std::vector<std::string>& vval = (*it).second;
    std::vector<std::string >::size_type sizeMultipleValues=vval.size();
    file.write((char*) &sizeMultipleValues, sizeof(uint32_t));
    for( std::vector<std::string>::const_iterator vit = vval.begin() ;
      vit != vval.end() ; vit++ ) {
      Misc::writeStringField(file,*vit);
    }
  }
//  Misc::writeStringField(file,"eoss");

  // Write multiple weighted properties
  size=m_d->m_multipleWeightedPropValues.size();
#ifdef DEBUG_CD
  LDEBUG << "write" << size << "weighted multi-valued properties";
#endif
  file.write((char*) &size, sizeof(uint32_t));
  for (std::map<std::string,std::vector<std::pair<std::string,float> > >::const_iterator it =
     m_d->m_multipleWeightedPropValues.begin() ;
     it != m_d->m_multipleWeightedPropValues.end() ; it++) {
//     LDEBUG << "write name " << (*it).first.c_str();
    Misc::writeStringField(file,(*it).first);
    const std::vector<std::pair<std::string,float> >& vval = (*it).second;
    std::vector<std::pair <std::string, float > >::size_type sizeMultipleValues=vval.size();
    file.write((char*) &sizeMultipleValues, sizeof(uint32_t));
    for( std::vector<std::pair<std::string,float> >::const_iterator vit = vval.begin() ;
      vit != vval.end() ; vit++ ) {
      Misc::writeStringField(file,(*vit).first);
      file.write((char*) &((*vit).second), sizeof(float));
    }
  }

#ifdef DEBUG_CD
      LDEBUG << "end of properties";
#endif
}

//***********************************************************************
// output
//***********************************************************************
std::ostream& operator << (std::ostream& os, const GenericDocumentProperties& p) {
  p.print(os);
  return os;
}

QDebug& operator << (QDebug& os, const GenericDocumentProperties& p) {
  p.print(os);
  return os;
}

void GenericDocumentProperties::print(std::ostream& os) const {

  for (std::map<std::string, uint64_t>::const_iterator it =
      m_d->m_intValues.begin (); it != m_d->m_intValues.end (); it++)
    {
      os << (*it).first << "=" << (*it).second << ";";
    }
  for (std::map<std::string, std::string>::const_iterator it =
      m_d->m_stringValues.begin (); it != m_d->m_stringValues.end ();
      it++)
    {
      os << (*it).first << "=" << (*it).second << ";";
    }
  for (std::map<std::string, QDate>::const_iterator it =
      m_d->m_dateValues.begin (); it != m_d->m_dateValues.end (); it++)
    {
      os << (*it).first << "="
          << (*it).second.toString ().toUtf8 ().data () << ";";
    }
  for (std::map<std::string, std::pair<QDate, QDate> >::const_iterator it =
      m_d->m_dateIntervalValues.begin ();
      it != m_d->m_dateIntervalValues.end (); it++)
    {
      os << (*it).first << "=["
          << ((*it).second).first.toString ().toUtf8 ().data () << ","
          << ((*it).second).second.toString ().toUtf8 ().data () << "];";
    }
  for (std::map<std::string, std::vector<std::string> >::const_iterator it =
      m_d->m_multipleStringValues.begin ();
      it != m_d->m_multipleStringValues.end (); it++)
    {
      os << (*it).first << "=";
      const std::vector<std::string>& vval = (*it).second;
      if (!vval.empty ())
        {
          std::vector<std::string>::const_iterator vit = vval.begin ();
          os << *vit;
          vit++;
          for (; vit != vval.end (); vit++)
            {
              os << "," << *vit;
            }
        }
      os << ";";
    }
  for (std::map<std::string, std::vector<std::pair<std::string, float> > >::const_iterator it =
      m_d->m_multipleWeightedPropValues.begin ();
      it != m_d->m_multipleWeightedPropValues.end (); it++)
    {
      os << (*it).first << "=";
      const std::vector<std::pair<std::string, float> >& vval =
          (*it).second;
      if (!vval.empty ())
        {
          std::vector<std::pair<std::string, float> >::const_iterator vit =
              vval.begin ();
          os << "(" << (*vit).first << "," << (*vit).second << ")";
          vit++;
          for (; vit != vval.end (); vit++)
            {
              os << ", (" << (*vit).first << "," << (*vit).second << ")";
            }
        }
      os << ";";
    }
}

void GenericDocumentProperties::print (QDebug& os) const
{

  for (std::map<std::string, uint64_t>::const_iterator it =
      m_d->m_intValues.begin (); it != m_d->m_intValues.end (); it++)
    {
      os << (*it).first << "=" << (*it).second << ";";
    }
  for (std::map<std::string, std::string>::const_iterator it =
      m_d->m_stringValues.begin (); it != m_d->m_stringValues.end ();
      it++)
    {
      os << (*it).first << "=" << (*it).second << ";";
    }
  for (std::map<std::string, QDate>::const_iterator it =
      m_d->m_dateValues.begin (); it != m_d->m_dateValues.end (); it++)
    {
      os << (*it).first << "="
          << (*it).second.toString ().toUtf8 ().data () << ";";
    }
  for (std::map<std::string, std::pair<QDate, QDate> >::const_iterator it =
      m_d->m_dateIntervalValues.begin ();
      it != m_d->m_dateIntervalValues.end (); it++)
    {
      os << (*it).first << "=["
          << ((*it).second).first.toString ().toUtf8 ().data () << ","
          << ((*it).second).second.toString ().toUtf8 ().data () << "];";
    }
  for (std::map<std::string, std::vector<std::string> >::const_iterator it =
      m_d->m_multipleStringValues.begin ();
      it != m_d->m_multipleStringValues.end (); it++)
    {
      os << (*it).first << "=";
      const std::vector<std::string>& vval = (*it).second;
      if (!vval.empty ())
        {
          std::vector<std::string>::const_iterator vit = vval.begin ();
          os << *vit;
          vit++;
          for (; vit != vval.end (); vit++)
            {
              os << "," << *vit;
            }
        }
      os << ";";
    }
  for (std::map<std::string, std::vector<std::pair<std::string, float> > >::const_iterator it =
      m_d->m_multipleWeightedPropValues.begin ();
      it != m_d->m_multipleWeightedPropValues.end (); it++)
    {
      os << (*it).first << "=";
      const std::vector<std::pair<std::string, float> >& vval =
          (*it).second;
      if (!vval.empty ())
        {
          std::vector<std::pair<std::string, float> >::const_iterator vit =
              vval.begin ();
          os << "(" << (*vit).first << "," << (*vit).second << ")";
          vit++;
          for (; vit != vval.end (); vit++)
            {
              os << ", (" << (*vit).first << "," << (*vit).second << ")";
            }
        }
      os << ";";
    }
}

} // end namespace Misc
} // end namespace Common
} // end namespace Lima
