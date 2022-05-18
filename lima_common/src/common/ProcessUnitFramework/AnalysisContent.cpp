// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "AnalysisContent.h"

using namespace std;

namespace Lima
{

AnalysisContent::AnalysisContent() :
    m_analysisData()
{}

AnalysisContent::~AnalysisContent()
{
  for (map<string,AnalysisData*>::iterator it=m_analysisData.begin();
         it!=m_analysisData.end();
         it++)
  {
    delete it->second;
    it->second=0;
  }
}

AnalysisData* AnalysisContent::getData(
  const std::string& id)
{
  map<string,AnalysisData*>::iterator it=m_analysisData.find(id);
  if (it==m_analysisData.end())
  {
#ifdef DEBUG_CD
    PROCESSUNITFRAMEWORKLOGINIT;
    LTRACE << "data " << id.c_str() << " doesn't exists, return 0";
#endif
    return nullptr;
  }
  return it->second;
}

const AnalysisData* AnalysisContent::getData(
  const std::string& id) const
{
  map<string,AnalysisData*>::const_iterator it=m_analysisData.find(id);
  if (it==m_analysisData.end())
  {
#ifdef DEBUG_CD
    PROCESSUNITFRAMEWORKLOGINIT;
    LTRACE << "data " << id.c_str() << " doesn't exists, return 0";
#endif
    return nullptr;
  }
  return it->second;
}

void AnalysisContent::setData(
  const std::string& id,
  AnalysisData* data)
{
#ifdef DEBUG_CD
  PROCESSUNITFRAMEWORKLOGINIT;
  LDEBUG << "setData " << id.c_str();
#endif
  map<string,AnalysisData*>::iterator it=m_analysisData.find(id);
  if (it!=m_analysisData.end())
  {
    PROCESSUNITFRAMEWORKLOGINIT;
    LERROR << "id " << id.c_str() << " already exists, it will be replaced";
    if (it->second != 0) {
      delete it->second;
    }
    it->second=data;
  }
  m_analysisData[id]=data;
}

void AnalysisContent::removeData(const std::string& id)
{
#ifdef DEBUG_CD
  PROCESSUNITFRAMEWORKLOGINIT;
  LDEBUG << "removeData " << id.c_str();
#endif
  map<string,AnalysisData*>::iterator it=m_analysisData.find(id);
  if (it!=m_analysisData.end())
  {
    if (it->second != 0) {
      delete it->second;
    }
    m_analysisData.erase(it);
  }
}

void AnalysisContent::releaseData(const std::string& id)
{
#ifdef DEBUG_CD
  PROCESSUNITFRAMEWORKLOGINIT;
  LDEBUG << "AnalysisContent::releaseData " << id.c_str();
#endif
  map<string,AnalysisData*>::iterator it=m_analysisData.find(id);
  if (it!=m_analysisData.end())
  {
    m_analysisData.erase(it);
  }
}

} // Lima
