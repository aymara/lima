// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "BenchmarkingTool.h"
#include "EvaluationResult.h"
using namespace Lima::Pelf;

static BenchmarkingTool* benchmarkingTool;

void msgHandler (QtMsgType type, const char* msg)
{
    benchmarkingTool->logDebugMsg(type, msg);
    switch (type)
    {
    case QtDebugMsg:
        fprintf(stderr, "Debug: %s\n", msg);
        break;
    case QtWarningMsg:
        fprintf(stderr, "Warning: %s\n", msg);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s\n", msg);
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s\n", msg);
        abort();
    }
}

int main (int argc, char** argv)
{
    QApplication* qApplication = new QApplication(argc, argv);
    benchmarkingTool = new BenchmarkingTool();
    qInstallMsgHandler(msgHandler);
    benchmarkingTool->init();
    return qApplication->exec();
}
