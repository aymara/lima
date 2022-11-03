// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                              *
 *                                                                         *
 ***************************************************************************/
#include "LinguisticMetaData.h"

#include <QDate>

namespace Lima {

namespace LinguisticProcessing {

LinguisticMetaData::LinguisticMetaData()
 : AnalysisData()
{
}


LinguisticMetaData::~LinguisticMetaData()
{
}

bool LinguisticMetaData::hasMetaData(const std::string& id) const 
{
  return (m_metaData.find(id)!=m_metaData.end());
}

const std::string& LinguisticMetaData::getMetaData(const std::string& id) const 
{
  std::map<std::string,std::string>::const_iterator it=m_metaData.find(id);
  if (it==m_metaData.end())
  {
    throw LinguisticProcessingException( std::string("metadata [" + id + "] not found.") );
  }
  return it->second;
}
const std::map<std::string,std::string>& LinguisticMetaData::getAllMetaData(void) const
{
  return m_metaData;
}

void LinguisticMetaData::setMetaData(const std::string& id,const std::string& value)
{
  m_metaData[id]=value;
}

void LinguisticMetaData::setMetaData(const std::map<std::string,std::string>& metaDatas)
{
  m_metaData=metaDatas;
}

uint64_t LinguisticMetaData::getStartOffset() const
{ return m_startOffset; }

void LinguisticMetaData::setStartOffset(uint64_t startOffset)
{ m_startOffset=startOffset;}

const QDate& LinguisticMetaData::
getDate(const std::string& dateName) const
{
  static QDate noDate;
  std::map<std::string,QDate>::const_iterator
    d=m_dates.find(dateName);
  if (d==m_dates.end()) {
    return noDate;
  }
  return (*d).second;
}
const QTime& LinguisticMetaData::
getTime(const std::string& timeName) const
{
  static QTime noTime;
  std::map<std::string,QTime>::const_iterator
    t=m_times.find(timeName);
  if (t==m_times.end()) {
    return noTime;
  }
  return (*t).second;
}

const std::string& LinguisticMetaData::
getLocation(const std::string& locName) const
{
  static std::string noLoc;
  std::map<std::string,std::string>::const_iterator
    l=m_locations.find(locName);
  if (l==m_locations.end()) {
    return noLoc;
  }
  return (*l).second;
}

void LinguisticMetaData::
setDate(const std::string& dateName,
        const QDate& d)
{
  m_dates[dateName]=d;
}
void LinguisticMetaData::
setTime(const std::string& timeName,
        const QTime& t)
{
  m_times[timeName]=t;
}
void LinguisticMetaData::
setLocation(const std::string& locName,
            const std::string& loc)
{
  m_locations[locName]=loc;
}


} // LinguisticProcessing
} // Lima
