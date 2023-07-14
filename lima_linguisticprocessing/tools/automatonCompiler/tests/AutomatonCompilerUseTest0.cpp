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

    QStringList arguments;
    arguments << "--language=fre" << "--modex=TEST-modex.xml" << "-osrc/resources/SpecificEntities/test.bin" << "src/rules/test.rules";
    process.setArguments(arguments);

    process.start();

    process.waitForFinished();
    qDebug() << "working directory was " << process.workingDirectory();
    qDebug() << "standart output:" << process.readAllStandardOutput();
    qDebug() << "standart error:" << process.readAllStandardError();

    QCOMPARE(process.exitCode(), 0);

    qDebug() << "file found in the current directory";

    QString sourceFilePath_currentDirectory = "petit.lst";
    QString destinationFilePath_filerulesDirectory = "src/rules/petit.lst";

    QFile sourceFile_currentDirectory(sourceFilePath_currentDirectory);
    QFile destinationFile_filerulesDirectory(destinationFilePath_filerulesDirectory);

    QVERIFY(sourceFile_currentDirectory.rename(destinationFilePath_filerulesDirectory));
    qDebug() << "File moved successfuly";

    process.start();

    QVERIFY(process.waitForFinished());

    QCOMPARE(process.exitCode(), 0);

    qDebug() << "file found in rule file directory";

    std::string lima_resources=qEnvironmentVariableIsEmpty("LIMA_RESOURCES")
    ?"/usr/share/apps/lima/resources"
    :std::string(qgetenv("LIMA_RESOURCES").constData());
    QStringList resourcesPaths = QString::fromUtf8(lima_resources.c_str()).split(LIMA_PATH_SEPARATOR);

    QString sourceFilePath_filerulesDirectory = "src/rules/petit.lst";
    QString destinationFilePath_LIMA_RESOURCES = (QDir::cleanPath(resourcesPaths.first() + QDir::separator() + "petit.lst"));

    QFile sourceFile_filerulesDirectory(sourceFilePath_filerulesDirectory);
    QFile destinationFile_LIMA_RESOURCES(destinationFilePath_LIMA_RESOURCES);

    QVERIFY(sourceFile_filerulesDirectory.rename(destinationFilePath_LIMA_RESOURCES));
    qDebug() << "File moved successfuly";

    process.start();

    QVERIFY(process.waitForFinished());

    QCOMPARE(process.exitCode(), 0);

    qDebug() << "file found in LIMA_RESOURCES";

    for(int i=1; i<resourcesPaths.size(); i++) {
        QString sourceFilePath = QDir::cleanPath(resourcesPaths[i-1] + QDir::separator() + "petit.lst");
        QString destinationFilePath = QDir::cleanPath(resourcesPaths[i] + QDir::separator() + "petit.lst");

        QFile sourceFile(sourceFilePath);
        QFile destinationFile(destinationFilePath);

        QVERIFY(sourceFile.rename(destinationFilePath));
        qDebug() << "File moved successfuly";

        process.start();

        process.waitForFinished();

        QCOMPARE(process.exitCode(), 0);

        qDebug() << "file found in LIMA_RESOURCES";
     }
     QString sourceFilePath = QDir::cleanPath(resourcesPaths.last() + QDir::separator() + "petit.lst");
     QString destinationFilePath = QDir::cleanPath(QDir::currentPath() + QDir::separator() + "src/petit.lst");

     QFile sourceFile(sourceFilePath);
     QFile destinationFile(destinationFilePath);

     QVERIFY(sourceFile.rename(destinationFilePath));
     qDebug() << "File moved successfuly";

     process.start();

     QVERIFY(process.waitForFinished());

     QVERIFY(process.exitCode() != 0);

     qDebug() << "file not found elsewhere";

     QString sourceFilePath_src = "src/petit.lst";
     QString destinationFilePath_currentDirectory = QDir::cleanPath(QDir::currentPath() + QDir::separator() + "petit.lst");

     QFile sourceFile_src(sourceFilePath_src);
     QFile destinationFile_currentDirectory(destinationFilePath_currentDirectory);

     QVERIFY(sourceFile.rename(destinationFilePath_currentDirectory));
     qDebug() << "File moved successfuly";

}

