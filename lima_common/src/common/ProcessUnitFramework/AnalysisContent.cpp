// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "AnalysisContent.h"

using namespace std;

namespace Lima
{

class AnalysisContentPrivate
{
  friend class AnalysisContent;
public:

  AnalysisContentPrivate() = default;

  /**
   * Destroy all AnalysisData in AnalysisContent
   * @return
   */
  ~AnalysisContentPrivate() = default;

  std::map<QString, std::shared_ptr< AnalysisData > > m_analysisData;

};

AnalysisContent::AnalysisContent() :
    m_d(new AnalysisContentPrivate())
{}

AnalysisContent::~AnalysisContent()
{
  delete m_d;
}

std::shared_ptr< AnalysisData > AnalysisContent::getData(const QString& id)
{
  auto it = m_d->m_analysisData.find(id);
  if (it == m_d->m_analysisData.end())
  {
#ifdef DEBUG_CD
    PROCESSUNITFRAMEWORKLOGINIT;
    LTRACE << "analysis data " << id << " doesn't exists, return 0";
#endif
    return nullptr;
  }
  return it->second;
}

const std::shared_ptr< AnalysisData > AnalysisContent::getData(const QString& id) const
{
  auto it = m_d->m_analysisData.find(id);
  if (it == m_d->m_analysisData.end())
  {
#ifdef DEBUG_CD
    PROCESSUNITFRAMEWORKLOGINIT;
    LTRACE << "data " << id << " doesn't exists, return 0";
#endif
    return nullptr;
  }
  return it->second;
}

void AnalysisContent::setData(const QString& id, std::shared_ptr< AnalysisData > data)
{
#ifdef DEBUG_CD
  PROCESSUNITFRAMEWORKLOGINIT;
  LDEBUG << "AnalysisContent::setData" << id;
#endif
  auto it = m_d->m_analysisData.find(id);
  if (it != m_d->m_analysisData.end())
  {
    PROCESSUNITFRAMEWORKLOGINIT;
    LWARN << "id " << id << " already exists, it will be replaced";
  }
  m_d->m_analysisData[id] = data;
}

void AnalysisContent::setData(const QString& id, AnalysisData* data)
{
#ifdef DEBUG_CD
  PROCESSUNITFRAMEWORKLOGINIT;
  LDEBUG << "AnalysisContent::setData" << id;
#endif
 setData(id, std::shared_ptr<AnalysisData>(data));
}

void AnalysisContent::removeData(const QString& id)
{
#ifdef DEBUG_CD
  PROCESSUNITFRAMEWORKLOGINIT;
  LDEBUG << "removeData " << id;
#endif
  auto it = m_d->m_analysisData.find(id);
  if (it != m_d->m_analysisData.end())
  {
    m_d->m_analysisData.erase(it);
  }
}

std::shared_ptr< AnalysisData > AnalysisContent::getData(const std::string& id)
{
  return getData(QString::fromStdString(id));
}

std::shared_ptr< AnalysisData > AnalysisContent::getData(const char* id)
{
  return getData(QString::fromStdString(id));
}

const std::shared_ptr< AnalysisData > AnalysisContent::getData(const char* id) const
{
  return getData(QString::fromStdString(id));
}

const std::shared_ptr< AnalysisData > AnalysisContent::getData(const std::string& id) const
{
  return getData(QString::fromStdString(id));
}

void AnalysisContent::setData(const std::string& id, std::shared_ptr<AnalysisData> data)
{
  setData(QString::fromStdString(id), data);
}

void AnalysisContent::setData(const std::string& id, AnalysisData* data)
{
  setData(QString::fromStdString(id), data);
}

void AnalysisContent::setData(const char* id, std::shared_ptr<AnalysisData> data)
{
  setData(QString::fromStdString(id), data);
}

void AnalysisContent::setData(const char* id, AnalysisData* data)
{
  setData(QString::fromStdString(id), data);
}

void AnalysisContent::removeData(const std::string& id)
{
  removeData(QString::fromStdString(id));
}

void AnalysisContent::removeData(const char* id)
{
  removeData(QString::fromStdString(id));
}

void AnalysisContent::releaseData(const std::string& id)
{
#ifdef DEBUG_CD
  PROCESSUNITFRAMEWORKLOGINIT;
  LDEBUG << "AnalysisContent::releaseData " << id.c_str();
#endif
  removeData(id);
}

} // Lima
