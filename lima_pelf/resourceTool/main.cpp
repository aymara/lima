// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
#if  QT_VERSION >= 0x050500
    case QtInfoMsg:
        fprintf(stderr, "Info: %s\n", msg.toUtf8().constData());
        break;
#endif
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
