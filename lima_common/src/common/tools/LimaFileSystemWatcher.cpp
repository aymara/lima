/*
 *     Copyright 2015 CEA LIST
 *
 *     This file is part of LIMA.
 *
 *     LIMA is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU Affero General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 *
 *     LIMA is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU Affero General Public License for more details.
 *
 *     You should have received a copy of the GNU Affero General Public License
 *     along with LIMA.  If not, see <http://www.gnu.org/licenses/>
 */

#include "LimaFileSystemWatcher.h"
#include "LimaFileSystemWatcher_p.h"
#include "common/LimaCommon.h"

#include <QStringList>
#include <QFileInfo>
#include <QDir>
#include <QTime>
#include <QCoreApplication>

#include <iostream>

namespace Lima
{

LimaFileSystemWatcherPrivate::LimaFileSystemWatcherPrivate ( LimaFileSystemWatcher* q, QObject* parent  ) : QObject(parent), q_ptr ( q ), m_pathToDeletedFileMapMutex()
{
//   MISCLOGINIT;
  if (!connect(&m_watcher,  SIGNAL(directoryChanged(QString)),this, SLOT(slotDirectoryChanged(QString)) ))
  {
    std::cerr << "LimaFileSystemWatcherPrivate::LimaFileSystemWatcherPrivate failed to connect directoryChanged signal"  << std::endl;
  }
  if (!connect(&m_watcher,  SIGNAL(fileChanged(QString)),this, SLOT(slotFileChanged(QString)) ))
  {
    std::cerr << "LimaFileSystemWatcherPrivate::LimaFileSystemWatcherPrivate failed to connect fileChanged signal" << std::endl;
  }
}

LimaFileSystemWatcherPrivate::~LimaFileSystemWatcherPrivate()
{
}


void LimaFileSystemWatcherPrivate::delay( int millisecondsToWait )
{
    QTime dieTime = QTime::currentTime().addMSecs( millisecondsToWait );
    while( QTime::currentTime() < dieTime )
    {
        QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
    }
}
void  LimaFileSystemWatcherPrivate::slotDirectoryChanged ( const QString & path )
{

  std::cerr  << "LimaFileSystemWatcherPrivate::slotDirectoryChanged" << path.toUtf8().constData() << std::endl;
  QMutexLocker locker(&m_pathToDeletedFileMapMutex);
  if (m_pathToDeletedFileMap.contains(path))
  {
    // for each of the files associated to the dir 'path', check if it exists again.
    QList<QString> files = m_pathToDeletedFileMap.values(path);
    for(auto file : files)
    {
      // file has been recreated: watch it again, remove it from list of deleted and signal the change
      if (QFileInfo::exists(file))
      {
        // Wait a few time to let the creator of the file to finish its work on it
        delay(1000);
        // remove file from list of deleted
        m_pathToDeletedFileMap.remove(path, file);
        // if there is no more deleted file in path, stop to watch it
        if (!m_pathToDeletedFileMap.contains(path))
        {
          m_watcher.removePath(path);
        }
        std::cerr  << "LimaFileSystemWatcherPrivate::slotDirectoryChanged watching again" << file.toUtf8().constData() << std::endl ;
        // watch file again
        m_watcher.addPath(file);
        // signal the change
        Q_EMIT fileChanged(file);
      }
    }
  }
}

void  LimaFileSystemWatcherPrivate::slotFileChanged ( const QString & path )
{
  std::cerr  << "LimaFileSystemWatcherPrivate::slotFileChanged" << path.toUtf8().constData() << std::endl;
  delay(500);
  // File just disapeared
  if (!QFileInfo::exists(path))
  {
    std::cerr  << "LimaFileSystemWatcherPrivate::slotFileChanged removed" << path.toUtf8().constData() << std::endl;
    QMutexLocker locker(&m_pathToDeletedFileMapMutex);
    // explicitely remove the  file from the watcher, otherwise, even if  it is no longer
    // monitored, it cannot be added again
    m_watcher.removePath(path);
    // get file dir
    QString dir = QFileInfo(path).absoluteDir().absolutePath();
    // add the link dir -file to the list of  deleted files
    m_pathToDeletedFileMap.insert(dir, path);
    // add the dir to the watcher
    m_watcher.addPath(dir);
  }
  // In all cases, transmit the signal
  std::cerr  << "LimaFileSystemWatcherPrivate::slotFileChanged emiting fileChanged from private " << path.toUtf8().constData() << std::endl;
  Q_EMIT fileChanged(path);
}

LimaFileSystemWatcher::LimaFileSystemWatcher( QObject* parent )
    : QObject(parent), m_d ( new LimaFileSystemWatcherPrivate ( this ) )
{
  if   (!connect(m_d, SIGNAL(fileChanged(QString)),this,SIGNAL(fileChanged(QString))))
  {
      std::cerr << "LimaFileSystemWatcher::LimaFileSystemWatcher failed to connect fileChanged signal"  << std::endl;
  }
}

LimaFileSystemWatcher::LimaFileSystemWatcher ( const LimaFileSystemWatcher& other )
    : QObject(other.parent()), m_d ( new LimaFileSystemWatcherPrivate ( this ) )
{
  if   (!connect(m_d, SIGNAL(fileChanged(QString)),this,SIGNAL(fileChanged(QString))))
  {
    std::cerr << "LimaFileSystemWatcher::LimaFileSystemWatcher failed to connect fileChanged signal" << std::endl;
  }
}

LimaFileSystemWatcher::~LimaFileSystemWatcher()
{
  delete m_d;
}

void  LimaFileSystemWatcher::addPath ( const QString & path )
{
  m_d->m_watcher.addPath(path);
}

void  LimaFileSystemWatcher::removePath ( const QString & path )
{
  m_d->m_watcher.removePath(path);
}

LimaFileSystemWatcher& LimaFileSystemWatcher::operator= ( const LimaFileSystemWatcher& other )
{
  m_d->m_watcher.removePaths(m_d->m_watcher.directories());
  m_d->m_watcher.removePaths(m_d->m_watcher.files());
  m_d->m_watcher.addPaths(other.m_d->m_watcher.directories());
  m_d->m_watcher.addPaths(other.m_d->m_watcher.files());
  return *this;
}

bool LimaFileSystemWatcher::operator== ( const LimaFileSystemWatcher& other )
{
  return m_d->m_watcher.directories() == other.m_d->m_watcher.directories()
        && m_d->m_watcher.files() == other.m_d->m_watcher.files();
}

} // Lima
