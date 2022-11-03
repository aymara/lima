// Copyright 2015 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMAFILESYSTEMWATCHER_H
#define LIMAFILESYSTEMWATCHER_H

#include "common/LimaCommon.h"
#include <QObject>

namespace Lima
{

class LimaFileSystemWatcherPrivate;

/**
 * @brief @ref LimaFileSystemWatcher extends QFileSystemWatcher to work on replaced files
 *
 * QFileSystemWatcher stops monitoring a file when it is suppressed. @@ref LimaFileSystemWatcher
 * watch the deleted file directory to watch again the former when it is created again.
 *
 * The public API is a subpart of those of QFileSystemWatcher.
 * @todo Complete the API to implement all the QFileSystemWatcher one
 */
class LIMA_COMMONTOOLS_EXPORT LimaFileSystemWatcher : public QObject
{
    Q_OBJECT
public:
    LimaFileSystemWatcher(QObject* parent = 0);
    LimaFileSystemWatcher ( const LimaFileSystemWatcher& other );
    ~LimaFileSystemWatcher();
    LimaFileSystemWatcher& operator= ( const LimaFileSystemWatcher& other );
    bool operator== ( const LimaFileSystemWatcher& other );

    void  addPath ( const QString & path );
    void  removePath ( const QString & path );

Q_SIGNALS:
  /** This signal is emited whenever @ref path change, either modified, suppressed or recreated. */
  void  fileChanged ( const QString & path );

private:
    class LimaFileSystemWatcherPrivate* const m_d;
    Q_DECLARE_PRIVATE ( LimaFileSystemWatcher )
};

} // Lima

#endif // LIMAFILESYSTEMWATCHER_H
