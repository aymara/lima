/*
 *    Copyright 2002-2013 CEA LIST
 * 
 *    This file is part of LIMA.
 * 
 *    LIMA is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Affero General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 * 
 *    LIMA is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Affero General Public License for more details.
 * 
 *    You should have received a copy of the GNU Affero General Public License
 *    along with LIMA.  If not, see <http://www.gnu.org/licenses/>
 */
#include "ResourceTool.h"

#include <QtGlobal>
#include <QMessageLogContext>
#include <QString>

using namespace Lima::Pelf;

static ResourceTool* resourceTool;

void msgHandler (QtMsgType type, const QMessageLogContext &, const QString &msg)
{
    resourceTool->logDebugMsg(type, msg.toUtf8().constData());
    switch (type)
    {
    case QtDebugMsg:
        fprintf(stderr, "Debug: %s\n", msg.toUtf8().constData());
        break;
    case QtInfoMsg:
        fprintf(stderr, "Info: %s\n", msg.toUtf8().constData());
        break;
    case QtWarningMsg:
        fprintf(stderr, "Warning: %s\n", msg.toUtf8().constData());
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s\n", msg.toUtf8().constData());
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s\n", msg.toUtf8().constData());
        abort();
    }
}

int main (int argc, char** argv)
{
    QApplication* qApplication = new QApplication(argc, argv);
    resourceTool = new ResourceTool();
    qInstallMessageHandler(msgHandler);
    if(!resourceTool->init())
    {
        QCoreApplication::quit();
        return 1;
    }
    return qApplication->exec();
}
