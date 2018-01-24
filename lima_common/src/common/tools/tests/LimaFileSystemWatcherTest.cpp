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

#include "LimaFileSystemWatcherTest.h"
#include "common/tools/LimaFileSystemWatcher.h"
#include "common/QsLog/QsLogCategories.h"
#include "common/AbstractFactoryPattern/AmosePluginsManager.h"

#include <QtCore/QTemporaryFile>
#include <QtTest/QtTest>

using namespace Lima;

QTEST_MAIN ( LimaFileSystemWatcherTest );

void LimaFileSystemWatcherTest::initTestCase()
{
    // Called before the first testfunction is executed
  QsLogging::initQsLog();
  // Necessary to initialize factories under Windows
  Lima::AmosePluginsManager::single();
}

void LimaFileSystemWatcherTest::cleanupTestCase()
{
    // Called after the last testfunction was executed
} 

void LimaFileSystemWatcherTest::init()
{
    // Called before each testfunction is executed
}

void LimaFileSystemWatcherTest::cleanup()
{
    // Called after every testfunction
}

void LimaFileSystemWatcherTest::LimaFileSystemWatcherTest0()
{
  LimaFileSystemWatcher watcher;

  QSignalSpy stateSpy( &watcher, &LimaFileSystemWatcher::fileChanged );

  QVERIFY( stateSpy.isValid() );

  QCOMPARE( stateSpy.count(), 0 );

 
  QTemporaryFile tmpFile;
  QVERIFY2(tmpFile.open(),"Was not able to open the temporary file");
  QString tmpFileName = tmpFile.fileName();
  
  watcher.addPath(tmpFileName);

  QTextStream out(&tmpFile);
  out << "yo";
  tmpFile.close();
  QVERIFY2( QFile(tmpFileName).exists(),  "The tmpFile does not exist while it should");
  QVERIFY2( stateSpy.wait(5000),  "We changed the file. The fileChanged signal should have been triggered");

  // remove the tmp file. This should trigger the signal
  QFile::remove(tmpFileName);
  
  QVERIFY2( !QFile(tmpFileName).exists(),  "The tmpFile still exists while it has  been removed");
  QVERIFY2( stateSpy.wait(5000),  "We removed the file. The fileChanged signal should have been triggered");

  // recreate the file. This should also trigger the signal
  QFile recreatedFile(tmpFileName);
  QVERIFY2( recreatedFile.open(QIODevice::ReadWrite), "Was not able to recreate the file");
  std::cerr << "";
  recreatedFile.close();
  QVERIFY2( stateSpy.wait(5000), "We recreated the file. The fileChanged signal should have been triggered");

}

