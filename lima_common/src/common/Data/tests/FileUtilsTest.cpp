// Copyright 2015 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "FileUtilsTest.h"
#include "common/tools/FileUtils.h"
#include "common/QsLog/QsLogCategories.h"

#include <QtCore/QTemporaryFile>
#include <QtCore/QTextStream>
#include <QtTest/QTest>

#include <fstream>

using namespace Lima::Common::Misc;

QTEST_MAIN ( FileUtilsTest );

// Testing uint64_t countLines(QFile& file)
void FileUtilsTest::FileUtilsTest0()
{
  try
  {
    QTemporaryFile file;

    QVERIFY2( file.open(), "Opens a temporary file" );
    QVERIFY2( countLines(file) == 0, "Initial temp file is empty" );
    QTextStream stream(&file);
    stream << "auie";
    stream.flush();
    stream.seek(0);
    QVERIFY2( countLines(file) == 1, "file is not empty but has no \\n: 1 line" );
    stream.seek(file.size());
    stream << QTENDL;
    stream.flush();
    stream.seek(0);
    QVERIFY2( countLines(file) == 1, "file has now one \\n and nothing after it: still one line" );
    stream.seek(file.size());
    stream << QString::fromUtf8("bépo");
    stream.flush();
    stream.seek(0);
    QVERIFY2( countLines(file) == 2, "file has now two \\n and chars on second line: two lines" );
  }
  catch (...)
  {
     QFAIL("An non identified exception was thrown during the test.");
  }
}

// Testing uint64_t countLines(QFile& file)
void FileUtilsTest::FileUtilsTest1()
{
  QTemporaryFile file;

  QVERIFY2( file.open(), "Opens a temporary file" );
  QVERIFY2( countLines(file) == 0, "Initial temp file is empty" );
  QTextStream stream(&file);
  // write two lines
  stream << "auie" << QTENDL << QString::fromUtf8("bépo") << QTENDL << QString::fromUtf8("ctsr") << QTENDL;
  stream.flush();
  stream.seek(0);
  QVERIFY2( countLines(file) == 3, "file has now three \\n and no chars on fourth line: three lines" );
  stream.seek(7);
  QVERIFY2( countLines(file) == 2, "has moved inside second line. count it and the third one." );
}

// Testing uint64_t countLines(std::istream& file)
void FileUtilsTest::FileUtilsTest2()
{
  QTemporaryFile tempFile;
  QVERIFY2( tempFile.open(), "Opens a temporary file" );
  QString tempFileName = tempFile.fileName();
  tempFile.close();
  std::fstream file;
  file.open(tempFileName.toUtf8().constData(),std::fstream::binary | std::fstream::in | std::fstream::out| std::fstream::trunc);

  QVERIFY2( file.good(), "Opens a temporary file" );

  uint64_t nbLines = countLines(file);
  QVERIFY2( nbLines == 0, QString(QLatin1String("Initial temp file should be empty but has %1 lines.")).arg(nbLines).toUtf8().constData() );

  file << "auie" << std::flush;
  file.seekg(0, std::ios::beg);
  QVERIFY(file.good());
  nbLines = countLines(file);
  QVERIFY2( nbLines == 1, QString(QLatin1String("File is not empty but has no \\n: result should be 1 but is %1")).arg(nbLines).toUtf8().constData() );
  file.seekg(0, std::ios::end);
  file << '\n' << std::flush;
  file.seekg(0, std::ios::beg);
  nbLines = countLines(file);
  QVERIFY2( nbLines == 1, QString(QLatin1String("file has now one \\n and nothing after it: should still count one line but get %1")).arg(nbLines).toUtf8().constData() );
  file.seekg(0, std::ios::end);
  file << "bepo" << std::flush;
  file.seekg(0, std::ios::beg);
  nbLines = countLines(file);
  QVERIFY2( nbLines == 2, QString(QLatin1String("file has now two \\n and chars on second line: should count two lines but get %1")).arg(nbLines).toUtf8().constData() );
  file.close();

}

// Testing uint64_t countLines(std::istream& file)
void FileUtilsTest::FileUtilsTest3()
{
  QTemporaryFile tempFile;
  QVERIFY2( tempFile.open(), "Opens a temporary file" );
  QString tempFileName = tempFile.fileName();
  tempFile.close();
  std::fstream file;
  file.open(tempFileName.toUtf8().constData(),std::fstream::binary | std::fstream::in | std::fstream::out| std::fstream::trunc);


  QVERIFY2( file.good(), "Opens a temporary file" );

  uint64_t nbLines = countLines(file);
  QVERIFY2( nbLines == 0, "Initial temp file is empty" );
  // write two lines
  file << "auie" << std::endl << "bepo" << std::endl << "ctsr" << std::endl << std::flush;
  file.seekg(0, std::ios::beg);
  nbLines = countLines(file);
  QVERIFY2( nbLines == 3, QString(QLatin1String("file has now three \\n and no chars on fourth line: should count three lines but get %1")).arg(nbLines).toUtf8().constData() );
  file.seekg(7, std::ios::beg);
  nbLines = countLines(file);
  QVERIFY2( nbLines == 2, "has moved inside second line. count it and the third one." );
  file.close();
}

