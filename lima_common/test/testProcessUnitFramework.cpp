// Copyright 2002-2019 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004 by CEA - LIST                                      *
 *                                                                         *
 ***************************************************************************/


#include "common/LimaCommon.h"

#include "dummyProcessUnits.h"
#include "common/AbstractFactoryPattern/AmosePluginsManager.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "common/ProcessUnitFramework/AnalysisContent.h"
#include "common/tools/LimaMainTaskRunner.h"
#include "common/XMLConfigurationFiles/moduleConfigurationStructure.h"
#include "common/XMLConfigurationFiles/groupConfigurationStructure.h"

#include <QtCore/QTimer>
#include <QtCore/QCoreApplication>

using namespace std;

using namespace Lima;

SimpleFactory<DummyProcessUnit,DreamingProcessUnit> dreamingFactory("DreamingProcessUnit");
SimpleFactory<DummyProcessUnit,ZenProcessUnit> zenFactory("ZenProcessUnit");
SimpleFactory<DummyProcessUnit,DummyProcessUnitPipeline> pipFactory("ProcessUnitPipeline");


//****************************************************************************

int run(int aargc, char** aargv);

int main(int argc, char** argv)
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

int run(int argc, char** argv)
{
  LIMA_UNUSED(argc);
  LIMA_UNUSED(argv);
  QsLogging::initQsLog();
  // Necessary to initialize factories
  Lima::AmosePluginsManager::single();

  cout << "dummy program, just to instantiate templates from ProcessUnitFramework" << endl;

  // build fake moduleconfigurationstructure
  Common::XMLConfigurationFiles::ModuleConfigurationStructure modconf(QLatin1String("carpet"));
  {
    modconf.addGroupNamed(QLatin1String("myDreamer"));
    Common::XMLConfigurationFiles::GroupConfigurationStructure& groupConf=modconf.getGroupNamed("myDreamer");
    groupConf.addAttribute("class","DreamingProcessUnit");
  }
  {
    modconf.addGroupNamed(QLatin1String("otherDreamer"));
    Common::XMLConfigurationFiles::GroupConfigurationStructure& groupConf=modconf.getGroupNamed("otherDreamer");
    groupConf.addAttribute("class","DreamingProcessUnit");
  }
  {
    modconf.addGroupNamed(QLatin1String("myZen"));
    Common::XMLConfigurationFiles::GroupConfigurationStructure& groupConf=modconf.getGroupNamed("myZen");
    groupConf.addAttribute("class","ZenProcessUnit");
  }
  {
    modconf.addGroupNamed(QLatin1String("myPipeline"));
    Common::XMLConfigurationFiles::GroupConfigurationStructure& groupConf=modconf.getGroupNamed("myPipeline");
    groupConf.addAttribute("class","ProcessUnitPipeline");
    groupConf.addListNamed("processUnitSequence");
    groupConf.addItemInListNamed("myDreamer","processUnitSequence");
    groupConf.addItemInListNamed("otherDreamer","processUnitSequence");
    groupConf.addItemInListNamed("myZen","processUnitSequence");
    groupConf.addItemInListNamed("myDreamer","processUnitSequence");
  }

  DummyProcessUnit::Manager manager(modconf);

  auto pip = manager.getObject("myPipeline");
  if (pip==0) {
    cerr << "FAILED : Getting myPipeline failed !!" << endl;
    return -1;
  }
  AnalysisContent ac;
  if (pip->process(ac) != SUCCESS_ID) {
    cerr << "FAILED : Getting myPipeline failed !!" << endl;
    return -1;
  }
  cout << "test is OK. Have a good day, and remember to wash your teeth after diner ..." << endl;
  return 0;
}
