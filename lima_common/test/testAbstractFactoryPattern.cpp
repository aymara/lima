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


#include "common/LimaCommon.h"

#include "dummyInitializableObject.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "common/XMLConfigurationFiles/moduleConfigurationStructure.h"
#include "common/XMLConfigurationFiles/groupConfigurationStructure.h"

#include <QtCore/QCoreApplication>

using namespace std;

using namespace Lima;

SimpleFactory<DummyInitializableObject,VeryDummyInitializableObject> veryDummyFactory("VeryDummyInitializableObject");
SimpleFactory<DummyInitializableObject,NotSoDummyInitializableObject> notSoDummyFactory("NotSoDummyInitializableObject");

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
  LIMA_UNUSED(argc);
  LIMA_UNUSED(argv);
  QsLogging::initQsLog();
  // Necessary to initialize factories
  Lima::AmosePluginsManager::single();

  cout << "dummy program, just to instantiate templates from AbstractFactoryPattern" << endl;

  // build fake moduleconfigurationstructure
  Common::XMLConfigurationFiles::ModuleConfigurationStructure modconf("tata");
  {
    modconf.addGroupNamed("myVeryDummy");
    Common::XMLConfigurationFiles::GroupConfigurationStructure& groupConf=modconf.getGroupNamed("myVeryDummy");
    groupConf.addAttribute("class","VeryDummyInitializableObject");
  }
  {
    modconf.addGroupNamed("otherVeryDummy");
    Common::XMLConfigurationFiles::GroupConfigurationStructure& groupConf=modconf.getGroupNamed("otherVeryDummy");
    groupConf.addAttribute("class","VeryDummyInitializableObject");
  }
  {
    modconf.addGroupNamed("myNotSoDummy");
    Common::XMLConfigurationFiles::GroupConfigurationStructure& groupConf=modconf.getGroupNamed("myNotSoDummy");
    groupConf.addAttribute("class","NotSoDummyInitializableObject");
  }

  //
  DummyInitializableObject::Manager manager(modconf);

  DummyInitializableObject* mvd=manager.getObject("myVeryDummy");
  if (mvd==0) {
    cerr << "FAILED : Getting VeryDummyInitializableObject failed !!" << endl;
    return -1;
  }
  VeryDummyInitializableObject* vdvd=dynamic_cast<VeryDummyInitializableObject*>(mvd);
  if (vdvd==0) {
    cerr << "FAILED : myVeryDummy is not an objet of class VeryDummyInitializableObject !!" << endl;
    return -1;
  }
  DummyInitializableObject* ovd=manager.getObject("otherVeryDummy");
  if (ovd==0) {
    cerr << "FAILED : Getting VeryDummyInitializableObject failed !!" << endl;
    return -1;
  }
  DummyInitializableObject* mvd2=manager.getObject("myVeryDummy");
  if (mvd!=mvd2) {
    cerr << "FAILED : Getting myVeryDummy has been re-initialized !!" << endl;
    return -1;
  }
  DummyInitializableObject* nsd=manager.getObject("myNotSoDummy");
  if (nsd==0) {
    cerr << "FAILED : Getting myMyNotSoDummy failed !!" << endl;
    return -1;
  }
  cout << "test is OK. Have a good day, and don't forget to smile ;-)" << endl;
  return 0;
}
