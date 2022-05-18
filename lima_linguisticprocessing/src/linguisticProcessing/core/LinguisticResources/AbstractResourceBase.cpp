// Copyright 2017 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "AbstractResourceBase.h"

#include "common/MediaticData/mediaticData.h"
#include "common/tools/FileUtils.h"

#include <QFileInfo>

namespace Lima {
namespace LinguisticProcessing {

class AbstractResourceBasePrivate
{
friend  class AbstractResourceBase;

  AbstractResourceBasePrivate() {}
  ~AbstractResourceBasePrivate() {}
  AbstractResourceBasePrivate(const AbstractResourceBasePrivate&) {}

  LimaFileSystemWatcher m_resourceFileWatcher;
};

AbstractResourceBase::AbstractResourceBase( QObject* parent ) :
    QObject( parent ),
    m_d(new AbstractResourceBasePrivate())
{
  connect(&m_d->m_resourceFileWatcher, &LimaFileSystemWatcher::fileChanged,
          this, &AbstractResourceBase::resourceFileChanged);
}

AbstractResourceBase::~AbstractResourceBase()
{
  delete m_d;
}

AbstractResourceBase::AbstractResourceBase(const AbstractResourceBase& r) :
    QObject(r.parent()),
    m_d(new AbstractResourceBasePrivate(*r.m_d))
{
  connect(&m_d->m_resourceFileWatcher, &LimaFileSystemWatcher::fileChanged,
          this, &AbstractResourceBase::resourceFileChanged);
}

QString AbstractResourceBase::getResourceFileName(const QString& paramName)
{
  QString file;
  QStringList resourcesPaths = QString::fromUtf8(Common::MediaticData::MediaticData::single().getResourcesPath().c_str()).split(LIMA_PATH_SEPARATOR);
  Q_FOREACH(QString resPath, resourcesPaths)
  {
    file = resPath + "/" + paramName;
    if (QFileInfo::exists(file))
    {
      resourceFileWatcher().addPath(file);
      break;
    }
  }
  return file;
}


LimaFileSystemWatcher& AbstractResourceBase::resourceFileWatcher()
{
  return m_d->m_resourceFileWatcher;
}
// void AbstractResourceBase::resourceFileChanged ( const QString & path )
// {
//   std::cerr << "AbstractResourceBase::resourceFileChanged" << path.toUtf8().constData();
// //   configure(path);
// }


} // LinguisticProcessing
} // Lima

