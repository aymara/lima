/*
    Copyright 2002-2013 CEA LIST

    This file is part of LIMA.

    LIMA is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    LIMA is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with LIMA.  If not, see <http://www.gnu.org/licenses/>
*/
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
    const QString sLogPath(QDir(a.applicationDirPath()).filePath("log.txt"));
    QsLogging::DestinationPtr fileDestination(
        QsLogging::DestinationFactory::MakeFileDestination(sLogPath) );
    QsLogging::DestinationPtr debugDestination(
        QsLogging::DestinationFactory::MakeDebugOutputDestination() );
    logger.addDestination(debugDestination.get());
    logger.addDestination(fileDestination.get());
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
