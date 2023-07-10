// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

// clazy:skip


#include "SearchTest0.h"
#include <QtTest/QTest>
#include <QProcess>
#include <QCoreApplication>
#include <QDebug>
#include "common/tools/FileUtils.h"



QTEST_MAIN ( SearchTest );


void SearchTest::SearchTest0()
{
    qputenv("LIMA_CONF", "src/conf");

    QProcess process;

    process.setProgram("compile-rules");

    QStringList arguments;
    arguments << "--language=fre" << "--modex=TEST-modex.xml" << "-osrc/resources/SpecificEntities/test.bin" << "src/rules/test.rules";
    process.setArguments(arguments);

    process.start();

    process.waitForFinished();

    QByteArray standardOutput = process.readAllStandardOutput();
    QByteArray standardError = process.readAllStandardError();

    QCOMPARE(process.exitCode(), 0);

    qDebug() << "file found in the current directory";

    qDebug() << "Sortie standard :";
    qDebug() << standardOutput;

    qDebug() << "Sortie d'erreur :";
    qDebug() << standardError;

    QString sourceFilePath_currentDirectory = "petit.lst";
    QString destinationFilePath_filerulesDirectory = "src/rules/petit.lst";

    QFile sourceFile_currentDirectory(sourceFilePath_currentDirectory);
    QFile destinationFile_filerulesDirectory(destinationFilePath_filerulesDirectory);

    if (sourceFile_currentDirectory.rename(destinationFilePath_filerulesDirectory)) {
        qDebug() << "File moved successfuly";
    } else {
        qDebug() << "Connot move file";
    }

    process.start();

    QVERIFY(process.waitForFinished());

    standardOutput = process.readAllStandardOutput();
    standardError = process.readAllStandardError();

    QCOMPARE(process.exitCode(), 0);

    qDebug() << "file found in rule file directory";

    qDebug() << "Sortie standard :";
    qDebug() << standardOutput;

    qDebug() << "Sortie d'erreur :";
    qDebug() << standardError;

    std::string lima_resources=qEnvironmentVariableIsEmpty("LIMA_RESOURCES")
    ?"/usr/share/apps/lima/resources"
    :std::string(qgetenv("LIMA_RESOURCES").constData());
    QStringList resourcesPaths = QString::fromUtf8(lima_resources.c_str()).split(LIMA_PATH_SEPARATOR);
    qDebug() << resourcesPaths.size();

    QString sourceFilePath_filerulesDirectory = "src/rules/petit.lst";
    QString destinationFilePath_LIMA_RESOURCES = (QDir::cleanPath(resourcesPaths.first() + QDir::separator() + "petit.lst"));

    QFile sourceFile_filerulesDirectory(sourceFilePath_filerulesDirectory);
    QFile destinationFile_LIMA_RESOURCES(destinationFilePath_LIMA_RESOURCES);

    if (sourceFile_filerulesDirectory.rename(destinationFilePath_LIMA_RESOURCES)) {
        qDebug() << "File moved successfuly";
    } else {
        qDebug() << "Connot move file";
    }

    process.start();

    QVERIFY(process.waitForFinished());

    standardOutput = process.readAllStandardOutput();
    standardError = process.readAllStandardError();

    QCOMPARE(process.exitCode(), 0);

    qDebug() << "file found in LIMA_RESOURCES";

    qDebug() << "Sortie standard :";
    qDebug() << standardOutput;

    qDebug() << "Sortie d'erreur :";
    qDebug() << standardError;


    for(int i=1; i<resourcesPaths.size(); i++) {
        QString sourceFilePath = QDir::cleanPath(resourcesPaths[i-1] + QDir::separator() + "petit.lst");
        QString destinationFilePath = QDir::cleanPath(resourcesPaths[i] + QDir::separator() + "petit.lst");

        QFile sourceFile(sourceFilePath);
        QFile destinationFile(destinationFilePath);

    if (sourceFile.rename(destinationFilePath)) {
        qDebug() << "File moved successfuly";
    } else {
        qDebug() << "Connot move file";
    }

        process.start();

        process.waitForFinished();

        QByteArray standardOutput = process.readAllStandardOutput();
        QByteArray standardError = process.readAllStandardError();

        QCOMPARE(process.exitCode(), 0);

        qDebug() << "Sortie standard :";
        qDebug() << standardOutput;

        qDebug() << "Sortie d'erreur :";
        qDebug() << standardError;

        qDebug() << "file found in LIMA_RESOURCES";
     }
     QString sourceFilePath = QDir::cleanPath(resourcesPaths.last() + QDir::separator() + "petit.lst");
     QString destinationFilePath = QDir::cleanPath(QDir::currentPath() + QDir::separator() + "petit.lst");

     QFile sourceFile(sourceFilePath);
     QFile destinationFile(destinationFilePath);

     if (sourceFile.rename(destinationFilePath)) {
         qDebug() << "File moved successfuly";
     } else {
         qDebug() << "Connot move file";
     }
}

