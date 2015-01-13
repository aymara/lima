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
/***************************************************************************
 *   Copyright (C) 2004 by CEA - LIST                                      *
 *                                                                         *
 ***************************************************************************/


#include "common/LimaCommon.h"
#include "linguisticProcessing/common/PropertyCode/PropertyCodeManager.h"


#include <string>
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;
using namespace Lima;
using namespace Lima::Common::PropertyCode;


//****************************************************************************
// GLOBAL variable -> the command line arguments
struct
{
  string codeFile; // code file to parse
  string outputFile;
  vector<string> inputFiles; // symbolic code file to parse
  bool help;       // help mode
}
param={"",
       "",
       vector<string>(),
       false};

void readCommandLineArguments(uint64_t argc, char *argv[])
{
  for(uint64_t i(1); i<argc; i++)
  {
    string arg(argv[i]);
    std::string::size_type pos;
    if (arg=="-h" || arg=="--help")
      param.help=true;
    else if ( (pos = arg.find("--code=")) != std::string::npos )
    {
      param.codeFile = arg.substr(pos+7);
    }
    else if ( (pos = arg.find("--output=")) != std::string::npos )
    {
      param.outputFile = arg.substr(pos+9);
    }
    else if (arg[0]=='-')
    {
      cerr << "unrecognized option " << arg << endl;
//       usage(argc,argv);
      exit(1);
    }
    else
    {
      param.inputFiles.push_back(arg);
    }
  }
}

//****************************************************************************
#include "common/misc/LimaMainTaskRunner.h"
#include "common/AbstractFactoryPattern/AmosePluginsManager.h"
#include <QtCore/QTimer>
#include <QtCore/QCoreApplication>

int run(int aargc,char** aargv);

int main(int argc, char **argv)
{
  QCoreApplication a(argc, argv);

  // Task parented to the application so that it
  // will be deleted by the application.
  LimaMainTaskRunner* task = new LimaMainTaskRunner(argc, argv, run, &a);

  // This will cause the application to exit when
  // the task signals finished.
  QObject::connect(task, SIGNAL(finished(int)), &a, SLOT(quit()));

  // This will run the task from the application event loop.
  QTimer::singleShot(0, task, SLOT(run()));

  return a.exec();

}


int run(int argc,char** argv)
{
  QsLogging::initQsLog();
  // Necessary to initialize factories
  Lima::AmosePluginsManager::single();
  
  readCommandLineArguments(argc,argv);
  if (param.help)
  {
//     usage(argc,argv);
    exit(0);
  }

  std::string resourcesPath=(getenv("LIMA_RESOURCES")!=0)?string(getenv("LIMA_RESOURCES")):string("/usr/share/apps/lima/resources");
  std::string configDir=(getenv("LIMA_CONF")!=0)?string(getenv("LIMA_CONF")):string("/usr/share/config/lima");

  // parse code file
  PropertyCodeManager propcodemanager;
  propcodemanager.readFromXmlFile(param.codeFile);
  
  // Convert symbolic codes
  map<string,LinguisticCode> conversionMap;
  for (vector<string>::const_iterator fileItr=param.inputFiles.begin();
       fileItr!=param.inputFiles.end();
       fileItr++)
  {
    propcodemanager.convertSymbolicCodes(fileItr->c_str(),conversionMap);
  }
  
  // output results
  std::ostream* out(0);
  if (param.outputFile == "")
  {
    out = &std::cout;
  }
  else
  {
    out = new ofstream(param.outputFile.c_str());
  }
  for (map<string,LinguisticCode>::const_iterator it=conversionMap.begin();
       it!=conversionMap.end();
       it++)
  {
    *out << it->first << ";" << it->second << ";" << endl;
  }
  return 0;
}

void usage(int argc, char *argv[])
{
  LIMA_UNUSED(argc);
  std::cout << "usage: " << argv[0] << " --code=<xmlPropertyFile> [--output=<file>] [<symbolicFile>] [<symbolicFile>] ..." << endl;
  exit(0);
}
