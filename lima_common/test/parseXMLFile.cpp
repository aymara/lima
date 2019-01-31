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
#include "common/XMLConfigurationFiles/xmlConfigurationFileParser.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"

#include <string>
#include <vector>
#include <iostream>

#include <QtCore/QCoreApplication>

using namespace std;
using namespace Lima::Common::XMLConfigurationFiles;

//****************************************************************************
void usage(int argc, char* argv[]);

//****************************************************************************
// GLOBAL variable -> the command line arguments
typedef struct
{
  string inputFile; // file to parse
  string module;   // module to find
  string group;    // group to find
  string param;    // param to find
  bool help;       // help mode
} Param;

Q_GLOBAL_STATIC_WITH_ARGS(Param, param,({"", "", "", "", false}));

void readCommandLineArguments(uint64_t argc, char *argv[])
{
  for(uint64_t i(1); i<argc; i++)
  {
    string s(argv[i]);
    if (s=="-h" || s=="--help")
      param->help=true;
    else if (s.find("--module=")==0)
      param->module=string(s,9);
    else if (s.find("--group=")==0)
      param->group=string(s,8);
    else if (s.find("--param=")==0)
      param->param=string(s,8);
    else if (s[0]=='-')
    {
      cerr << "unrecognized option " <<  s << endl;
      usage(argc,argv);
      exit(1);
    }
    else
    {
      param->inputFile=s;
    }
  }
  
  // some consistency checks in arguments
  if (! param->param.empty()) {
      if (param->group.empty()) { cerr << "need group name to get param" << endl; exit(1); }
      if (param->module.empty()) { cerr << "need module name to get param" << endl; exit(1); }
  }
  else if (! param->group.empty()) {
      if (param->module.empty()) { cerr << "need module name to get param" << endl; exit(1); }
  }
}

//****************************************************************************
#include "common/tools/LimaMainTaskRunner.h"
#include "common/AbstractFactoryPattern/AmosePluginsManager.h"
#include <QtCore/QTimer>

int run(int aargc,char** aargv);

int main(int argc, char **argv)
{
  QCoreApplication a(argc, argv);

  // Task parented to the application so that it
  // will be deleted by the application.
  Lima::LimaMainTaskRunner* task = new Lima::LimaMainTaskRunner(argc, argv, run, &a);

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
  if (param->help) {
      usage(argc,argv);
      exit(0);
  }
  
  string resourcesPath=qEnvironmentVariableIsEmpty("LIMA_RESOURCES")?"/usr/share/apps/lima/resources":string(qgetenv("LIMA_RESOURCES").constData());
  string commonConfigFile=string("lima-common.xml");
  string configDir=qEnvironmentVariableIsEmpty("LIMA_CONF")?"/usr/share/config/lima":string(qgetenv("LIMA_CONF").constData());

  XMLConfigurationFileParser parser(param->inputFile.c_str());
  
  if (! param->param.empty()) {
      try {
        cout << parser.getModuleGroupParamValue(param->module,param->group,param->param) << endl;
      }  
      catch (NoSuchParam& e) {
        cout << e.what() << endl;
      }
  }
  else if (! param->group.empty()) {
      try {
//     cout << parser.getModuleGroupConfiguration(param->module,param->group) << endl;
      }
      catch (NoSuchGroup& e) {
        cout << e.what() << endl;
      }
  }
  else if (! param->module.empty()) {
      try {
//     cout << parser.getModuleConfiguration(param->module) << endl;
      }
      catch (NoSuchModule & e) {
        cout << e.what() << endl;
      }
  }
  else {
      cout << parser << endl;
  }
  return 0;
}

void usage(int argc, char *argv[])
{
  LIMA_UNUSED(argc);
    std::cout << "usage: " << argv[0] << "[--module=..] [--group=..] [--param=..] xmlFile" << endl;
  exit(0);
}
