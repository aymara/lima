// Copyright 2002-2019 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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

using namespace Lima;

SimpleFactory<DummyInitializableObject,VeryDummyInitializableObject> veryDummyFactory("VeryDummyInitializableObject");
SimpleFactory<DummyInitializableObject,NotSoDummyInitializableObject> notSoDummyFactory("NotSoDummyInitializableObject");

//****************************************************************************
#include "common/tools/LimaMainTaskRunner.h"
#include "common/AbstractFactoryPattern/AmosePluginsManager.h"
#include <QtCore/QTimer>

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

  std::cout << "dummy program, just to instantiate templates from AbstractFactoryPattern" << std::endl;

  // build fake moduleconfigurationstructure
  Common::XMLConfigurationFiles::ModuleConfigurationStructure modconf(QLatin1String("tata"));
  {
    modconf.addGroupNamed(QLatin1String("myVeryDummy"));
    Common::XMLConfigurationFiles::GroupConfigurationStructure& groupConf=modconf.getGroupNamed("myVeryDummy");
    groupConf.addAttribute("class","VeryDummyInitializableObject");
  }
  {
    modconf.addGroupNamed(QLatin1String("otherVeryDummy"));
    Common::XMLConfigurationFiles::GroupConfigurationStructure& groupConf=modconf.getGroupNamed("otherVeryDummy");
    groupConf.addAttribute("class","VeryDummyInitializableObject");
  }
  {
    modconf.addGroupNamed(QLatin1String("myNotSoDummy"));
    Common::XMLConfigurationFiles::GroupConfigurationStructure& groupConf=modconf.getGroupNamed("myNotSoDummy");
    groupConf.addAttribute("class","NotSoDummyInitializableObject");
  }

  //
  DummyInitializableObject::Manager manager(modconf);

  auto mvd = manager.getObject("myVeryDummy");
  if (mvd==nullptr) {
    std::cerr << "FAILED : Getting VeryDummyInitializableObject failed !!" << std::endl;
    return -1;
  }
  auto vdvd = std::dynamic_pointer_cast<VeryDummyInitializableObject>(mvd);
  if (vdvd==0) {
    std::cerr << "FAILED : myVeryDummy is not an objet of class VeryDummyInitializableObject !!" << std::endl;
    return -1;
  }
  auto ovd = manager.getObject("otherVeryDummy");
  if (ovd==0) {
    std::cerr << "FAILED : Getting VeryDummyInitializableObject failed !!" << std::endl;
    return -1;
  }
  auto mvd2 = manager.getObject("myVeryDummy");
  if (mvd!=mvd2) {
    std::cerr << "FAILED : Getting myVeryDummy has been re-initialized !!" << std::endl;
    return -1;
  }
  auto nsd = manager.getObject("myNotSoDummy");
  if (nsd==0) {
    std::cerr << "FAILED : Getting myMyNotSoDummy failed !!" << std::endl;
    return -1;
  }
  std::cout << "test is OK. Have a good day, and don't forget to smile ;-)" << std::endl;
  return 0;
}
