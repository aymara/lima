/*
    Copyright 2002-2019 CEA LIST

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
/***************************************************************************
 *   Copyright (C) 2004 by CEA - LIST                                      *
 *                                                                         *
 ***************************************************************************/

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include "common/LimaCommon.h"
#include "common/tools/FileUtils.h"
#include "linguisticProcessing/common/PropertyCode/PropertyCodeManager.h"

using namespace std;
using namespace Lima;
using namespace Lima::Common::Misc;
using namespace Lima::Common::PropertyCode;

void usage(int argc, char *argv[]);

//****************************************************************************
// GLOBAL variable -> the command line arguments
typedef struct
{
  string codeFile;           // code file to parse
  string outputFile;
  string configDir;          // this path overwrites environment variable
  vector<string> inputFiles; // symbolic code file to parse
  bool help;                 // help mode
} Param;
Q_GLOBAL_STATIC_WITH_ARGS(Param, param, ({"",
       "",
       "",
       vector<string>(),
       false}));

void readCommandLineArguments(uint64_t argc, char *argv[])
{
  for(uint64_t i(1); i<argc; i++)
  {
    string arg(argv[i]);
    std::string::size_type pos;

    if (arg == "-h" || arg == "--help")
      param->help=true;
    else if ( (pos = arg.find("--code=")) != std::string::npos )
    {
      param->codeFile = arg.substr(pos+7);
    }
    else if ( (pos = arg.find("--output=")) != std::string::npos )
    {
      param->outputFile = arg.substr(pos+9);
    }
    else if ( (pos = arg.find("--configDir=")) != std::string::npos )
    {
      param->configDir = arg.substr(pos+12);
    }
    else if (arg[0] == '-')
    {
      cerr << "unrecognized option " << arg << endl;
      usage(argc, argv);
      exit(1);
    }
    else
    {
      param->inputFiles.push_back(arg);
    }
  }
}

//****************************************************************************
#include "common/tools/LimaMainTaskRunner.h"
#include "common/AbstractFactoryPattern/AmosePluginsManager.h"
#include <QtCore/QTimer>

int run(int aargc, char** aargv);

int main(int argc, char **argv)
{
  QCoreApplication a(argc, argv);

  // Task parented to the application so that it
  // will be deleted by the application.
  LimaMainTaskRunner* task = new LimaMainTaskRunner(argc, argv, run, &a);

  // This will cause the application to exit when
  // the task signals finished.
  QObject::connect(task, &Lima::LimaMainTaskRunner::finished, [](int returnCode){ QCoreApplication::exit(returnCode); } );

  // This will run the task from the application event loop.
  QTimer::singleShot(0, task, SLOT(run()));

  return a.exec();
}

int run(int argc, char** argv)
{
  readCommandLineArguments(argc, argv);

  if (param->help)
  {
    usage(argc, argv);
    exit(0);
  }

  auto configDirs = buildConfigurationDirectoriesList(QStringList({"lima"}),
                                                      QStringList());
  auto configPath = configDirs.join(LIMA_PATH_SEPARATOR);

  if (!param->configDir.empty())
  {
    configPath = QString::fromUtf8(param->configDir.c_str());
    configDirs = configPath.split(LIMA_PATH_SEPARATOR);
  }

  if (QsLogging::initQsLog(configPath) != 0)
  {
    LOGINIT("Common::Misc");
    LERROR << "Call to QsLogging::initQsLog(\"" << configPath << "\") failed.";
    return EXIT_FAILURE;
  }

  // Necessary to initialize factories
  Lima::AmosePluginsManager::single();

  // parse code file
  PropertyCodeManager propcodemanager;
  propcodemanager.readFromXmlFile(param->codeFile);

  // Convert symbolic codes
  map<string,LinguisticCode> conversionMap;
  for (vector<string>::const_iterator fileItr=param->inputFiles.begin();
       fileItr!=param->inputFiles.end();
       fileItr++)
  {
    propcodemanager.convertSymbolicCodes(fileItr->c_str(),conversionMap);
  }

  // output results
  std::ostream* out(0);
  if (param->outputFile == "")
  {
    out = &std::cout;
  }
  else
  {
    out = new ofstream(param->outputFile.c_str());
  }
  for (map<string,LinguisticCode>::const_iterator it=conversionMap.begin();
       it!=conversionMap.end();
       it++)
  {
    *out << it->first << ";" << it->second << ";" << endl;
  }
  if ((param->outputFile != "") && (out != 0)) {
    delete out;
  }
  return 0;
}

void usage(int argc, char *argv[])
{
  LIMA_UNUSED(argc);
  std::cout << "usage: " << argv[0] << " --code=<xmlPropertyFile> [--output=<file>] [<symbolicFile>] [<symbolicFile>] ..." << endl;
  exit(0);
}
