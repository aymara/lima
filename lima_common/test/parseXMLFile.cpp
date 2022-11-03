// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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

using namespace Lima::Common::XMLConfigurationFiles;

//****************************************************************************
void usage(int argc, char* argv[]);

//****************************************************************************
// GLOBAL variable -> the command line arguments
typedef struct
{
  std::string inputFile; // file to parse
  std::string module;   // module to find
  std::string group;    // group to find
  std::string param;    // param to find
  bool help;       // help mode
} Param;

Q_GLOBAL_STATIC_WITH_ARGS(Param, param,({"", "", "", "", false}));

void readCommandLineArguments(uint64_t argc, char *argv[])
{
  for(uint64_t i(1); i<argc; i++)
  {
    std::string s(argv[i]);
    if (s=="-h" || s=="--help")
      param->help=true;
    else if (s.find("--module=")==0)
      param->module=std::string(s,9);
    else if (s.find("--group=")==0)
      param->group=std::string(s,8);
    else if (s.find("--param=")==0)
      param->param=std::string(s,8);
    else if (s[0]=='-')
    {
      std::cerr << "unrecognized option " <<  s << std::endl;
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
      if (param->group.empty()) { std::cerr << "need group name to get param" << std::endl; exit(1); }
      if (param->module.empty()) { std::cerr << "need module name to get param" << std::endl; exit(1); }
  }
  else if (! param->group.empty()) {
      if (param->module.empty()) { std::cerr << "need module name to get param" << std::endl; exit(1); }
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

  std::string resourcesPath=qEnvironmentVariableIsEmpty("LIMA_RESOURCES")?"/usr/share/apps/lima/resources":std::string(qgetenv("LIMA_RESOURCES").constData());
  std::string commonConfigFile=std::string("lima-common.xml");
  std::string configDir=qEnvironmentVariableIsEmpty("LIMA_CONF")?"/usr/share/config/lima":std::string(qgetenv("LIMA_CONF").constData());

  XMLConfigurationFileParser parser(param->inputFile.c_str());

  if (! param->param.empty()) {
      try {
        std::cout << parser.getModuleGroupParamValue(param->module,param->group,param->param) << std::endl;
      }
      catch (NoSuchParam& e) {
        std::cout << e.what() << std::endl;
      }
  }
  else if (! param->group.empty()) {
      try {
//     std::cout << parser.getModuleGroupConfiguration(param->module,param->group) << std::endl;
      }
      catch (NoSuchGroup& e) {
        std::cout << e.what() << std::endl;
      }
  }
  else if (! param->module.empty()) {
      try {
//     std::cout << parser.getModuleConfiguration(param->module) << std::endl;
      }
      catch (NoSuchModule & e) {
        std::cout << e.what() << std::endl;
      }
  }
  else {
      std::cout << parser << std::endl;
  }
  return 0;
}

void usage(int argc, char *argv[])
{
  LIMA_UNUSED(argc);
    std::cout << "usage: " << argv[0] << "[--module=..] [--group=..] [--param=..] xmlFile" << std::endl;
  exit(0);
}
