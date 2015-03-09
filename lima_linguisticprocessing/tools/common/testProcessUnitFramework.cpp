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

#include "dummyProcessUnits.h"
#include "common/ProcessUnitFramework/AnalysisContent.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "common/XMLConfigurationFiles/moduleConfigurationStructure.h"
#include "common/XMLConfigurationFiles/groupConfigurationStructure.h"

#include <QtCore/QCoreApplication>

using namespace std;

using namespace Lima;

SimpleFactory<DummyProcessUnit,DreamingProcessUnit> dreamingFactory("DreamingProcessUnit");
SimpleFactory<DummyProcessUnit,ZenProcessUnit> zenFactory("ZenProcessUnit");
SimpleFactory<DummyProcessUnit,DummyProcessUnitPipeline> pipFactory("ProcessUnitPipeline");

template<> MainFactory< RegistrableFactory< InitializableObjectFactory< DummyProcessUnit > > >* MainFactory< RegistrableFactory< InitializableObjectFactory< DummyProcessUnit > > >::s_instance(0);


//****************************************************************************
#include "common/misc/LimaMainTaskRunner.h"
#include "common/AbstractFactoryPattern/AmosePluginsManager.h"
#include <QtCore/QTimer>

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
  
  cout << "dummy program, just to instantiate templates from ProcessUnitFramework" << endl;
  
  // build fake moduleconfigurationstructure
  Common::XMLConfigurationFiles::ModuleConfigurationStructure modconf("carpet");
  {
    modconf.addGroupNamed("myDreamer");
    Common::XMLConfigurationFiles::GroupConfigurationStructure& groupConf=modconf.getGroupNamed("myDreamer");
    groupConf.addAttribute("class","DreamingProcessUnit");
  }
  {
    modconf.addGroupNamed("otherDreamer");
    Common::XMLConfigurationFiles::GroupConfigurationStructure& groupConf=modconf.getGroupNamed("otherDreamer");
    groupConf.addAttribute("class","DreamingProcessUnit");
  }
  {
    modconf.addGroupNamed("myZen");
    Common::XMLConfigurationFiles::GroupConfigurationStructure& groupConf=modconf.getGroupNamed("myZen");
    groupConf.addAttribute("class","ZenProcessUnit");
  }  
  {
    modconf.addGroupNamed("myPipeline");
    Common::XMLConfigurationFiles::GroupConfigurationStructure& groupConf=modconf.getGroupNamed("myPipeline");
    groupConf.addAttribute("class","ProcessUnitPipeline");
    groupConf.addListNamed("processUnitSequence");
    groupConf.addItemInListNamed("myDreamer","processUnitSequence");
    groupConf.addItemInListNamed("otherDreamer","processUnitSequence");
    groupConf.addItemInListNamed("myZen","processUnitSequence");
    groupConf.addItemInListNamed("myDreamer","processUnitSequence");
  }  
    
  DummyProcessUnit::Manager manager(modconf);
  
  DummyProcessUnit* pip=manager.getObject("myPipeline");
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
