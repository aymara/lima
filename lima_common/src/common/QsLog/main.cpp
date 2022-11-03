// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "common/LimaCommon.h"
#include "QsLog.h"
#include "QsLogDest.h"
#include "QsLogCategories.h"
#include <QtCore/QCoreApplication>
#include <QDir>
#include <iostream>

int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);
  QsLogging::initQsLog();
  int ret = 0;
  {
    // init the logging mechanism
    QsLogging::Logger& logger = QsLogging::Logger::instance("Zone");
    logger.setLoggingLevel(QsLogging::TraceLevel);
    const QString sLogPath(QDir(".").filePath("log.txt"));
    QsLogging::DestinationPtr fileDestination(
        QsLogging::DestinationFactory::MakeFileDestination(sLogPath) );
    QsLogging::DestinationPtr debugDestination(
        QsLogging::DestinationFactory::MakeDebugOutputDestination() );
    logger.addDestination(debugDestination);
    logger.addDestination(fileDestination);
    //logger.setLoggingLevel(QsLogging::InfoLevel);

    QLOG_INFO() << "Program started";
    QLOG_INFO() << "Built with Qt" << QT_VERSION_STR << "running on" << qVersion();

    QLOG_TRACE() << "Here's a" << QLatin1String("trace") << "message";
    QLOG_DEBUG() << "Here's a" << static_cast<int>(QsLogging::DebugLevel) << "message";
    QLOG_WARN()  << "Uh-oh!";
    qDebug() << "This message won't be picked up by the logger";
    QLOG_ERROR() << "An error has occurred";
    qWarning() << "Neither will this one";
    QLOG_FATAL() << "Fatal error!";
  }
  {
    AGLOGINIT;
    LDEBUG << "AnalysisGraph 1";
  }
  {
    CLIENTFACTORYLOGINIT;
    LDEBUG << "Oh!";
  }
  {
    AGLOGINIT;
    LDEBUG << "AnalysisGraph 2";
  }
  return ret;
}
