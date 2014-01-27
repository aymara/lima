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
