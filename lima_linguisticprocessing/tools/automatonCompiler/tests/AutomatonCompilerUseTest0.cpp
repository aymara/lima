// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

// clazy:skip


#include "AutomatonCompilerUseTest0.h"
#include <QtTest/QTest>
#include <QProcess>
#include <QCoreApplication>
#include <QDebug>
#include "common/tools/FileUtils.h"



QTEST_MAIN ( AutomatonCompilerUseTest );


void AutomatonCompilerUseTest::AutomatonCompilerUseTest0()
{
    QProcess process;
    process.setProgram("compile-rules");
    process.setWorkingDirectory(qgetenv("LIMA_PWD"));
    qDebug() << "environment is " << process.systemEnvironment();
    qDebug() << "application dir is " << QCoreApplication::applicationDirPath();
    qDebug() << "current path is " << QDir::currentPath();

    // Prepare data
    QDir currentPath(QDir::currentPath());
    QVERIFY(QDir(QDir::currentPath()+"/src").removeRecursively());
    QFile::remove("petit.lst");
    QVERIFY(currentPath.mkpath("src/conf"));
    QVERIFY(QFile::copy("tests/src/conf/lima-lp-fre.xml", "src/conf/lima-lp-fre.xml"));
    QVERIFY(QFile::copy("tests/src/conf/TEST-modex.xml", "src/conf/TEST-modex.xml"));
    QVERIFY(currentPath.mkpath("src/rules"));
    QVERIFY(QFile::copy("tests/src/petit.lst", "petit.lst"));
    QVERIFY(QFile::copy("tests/src/test.txt", "src/test.txt"));
    QVERIFY(QFile::copy("tests/src/rules/test.rules", "src/rules/test.rules"));

    // Prepare command
    QStringList arguments;
    arguments << "--language=fre" << "--modex=TEST-modex.xml" << "-oresources/SpecificEntities/test.bin" << "src/rules/test.rules";
    process.setArguments(arguments);

    process.start();

    process.waitForFinished();
    qDebug() << "working directory was " << process.workingDirectory();
    qDebug() << "standart output:" << process.readAllStandardOutput();
    qDebug() << "standart error:" << process.readAllStandardError();

    QCOMPARE(process.exitCode(), 0);

    qDebug() << "file found in the current directory";

    qDebug() << "current path is " << QDir::currentPath();
    QVERIFY(QFile::exists("petit.lst"));
    QVERIFY(QFile::remove("petit.lst"));
    QDir destinationDir("src/rules");
    QVERIFY(destinationDir.exists());
    QVERIFY(QFile::copy("tests/src/petit.lst", "src/rules/petit.lst"));
    qDebug() << "New location for list file OK";

    process.start();

    QVERIFY(process.waitForFinished());

    QCOMPARE(process.exitCode(), 0);

    qDebug() << "file found in rule file directory";
    QVERIFY(QFile::remove("src/rules/petit.lst"));

    auto lima_resources = qEnvironmentVariableIsEmpty("LIMA_RESOURCES")
      ? QString::fromLatin1("/usr/share/apps/lima/resources")
      : qgetenv("LIMA_RESOURCES").constData();
    auto resourcesPaths = lima_resources.split(LIMA_PATH_SEPARATOR);
    qDebug() << "resourcesPaths is" << resourcesPaths;

    QString destinationFilePath_LIMA_RESOURCES = (QDir::cleanPath(resourcesPaths.first() + QDir::separator() + "petit.lst"));
    qDebug() << "destinationFilePath_LIMA_RESOURCES is" << destinationFilePath_LIMA_RESOURCES;

    QVERIFY(QFile::copy("tests/src/petit.lst", destinationFilePath_LIMA_RESOURCES));
    process.start();

    QVERIFY(process.waitForFinished());

    QCOMPARE(process.exitCode(), 0);

    qDebug() << "file found in LIMA_RESOURCES";
    QVERIFY(QFile::remove(destinationFilePath_LIMA_RESOURCES));

    for(int i=1; i<resourcesPaths.size(); i++)
    {
        QString destinationFilePath = QDir::cleanPath(resourcesPaths[i] + QDir::separator() + "petit.lst");

        QVERIFY(QFile::copy("tests/src/petit.lst", destinationFilePath));

        qDebug() << "List file copied successfuly to" << destinationFilePath;

        process.start();

        QVERIFY(process.waitForFinished());

        QCOMPARE(process.exitCode(), 0);
        QVERIFY(QFile::remove(destinationFilePath));

        qDebug() << "file found in LIMA_RESOURCES";
    }
    QString destinationFilePath = QDir::cleanPath(QDir::currentPath() + QDir::separator() + "src/petit.lst");

    QVERIFY(QFile::copy("tests/src/petit.lst", destinationFilePath));

    qDebug() << "List file copied successfuly to" << destinationFilePath;

    process.start();

    QVERIFY(process.waitForFinished());

    QVERIFY(process.exitCode() != 0);

    qDebug() << "List file not found elsewhere as expected";


    QVERIFY(QFile::remove(destinationFilePath));
    qDebug() << "List file removed successfuly";

}

