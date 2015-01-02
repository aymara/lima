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

#ifndef LIMAFILESYSTEMWATCHER_H
#define LIMAFILESYSTEMWATCHER_H

#include <QObject>
#include "stringspool.h"

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
class LimaFileSystemWatcher : public QObject
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
