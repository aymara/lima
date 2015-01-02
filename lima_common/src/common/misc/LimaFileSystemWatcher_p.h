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

#ifndef LIMAFILESYSTEMWATCHER_PRIVATE_H
#define LIMAFILESYSTEMWATCHER_PRIVATE_H

#include <QtCore/qglobal.h>
#include <QFileSystemWatcher>
#include <QMultiMap>
#include "stringspool.h"

namespace Lima
{
  
class LimaFileSystemWatcher;
class LimaFileSystemWatcherPrivate : public QObject
{
  Q_OBJECT
public:
    LimaFileSystemWatcherPrivate ( LimaFileSystemWatcher* q, QObject* parent = 0 );
    virtual ~LimaFileSystemWatcherPrivate();

private Q_SLOTS:
    void  slotDirectoryChanged ( const QString & path );
    void  slotFileChanged ( const QString & path );

Q_SIGNALS:
    void  fileChanged ( const QString & path );

private:
    void delay( int millisecondsToWait );

    LimaFileSystemWatcher* const q_ptr;
    Q_DECLARE_PUBLIC ( LimaFileSystemWatcher )
    
    QFileSystemWatcher m_watcher;
    QMultiMap<QString,QString> m_pathToDeletedFileMap;
};

} // Lima

#endif // LIMAFILESYSTEMWATCHER_PRIVATE_H
