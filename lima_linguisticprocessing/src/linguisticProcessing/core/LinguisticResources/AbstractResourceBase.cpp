/*
    Copyright 2017 CEA LIST

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

