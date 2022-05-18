// Copyright 2015 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMAFILESYSTEMWATCHER_PRIVATE_H
#define LIMAFILESYSTEMWATCHER_PRIVATE_H

#include <QtCore/qglobal.h>
#include <QFileSystemWatcher>
#include <QMultiMap>
#include <QMutex>

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
    QMutex m_pathToDeletedFileMapMutex;
};

} // Lima

#endif // LIMAFILESYSTEMWATCHER_PRIVATE_H
