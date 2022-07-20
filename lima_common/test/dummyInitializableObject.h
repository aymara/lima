// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004 by CEA LIST                      *
 *                                                                         *
 ***************************************************************************/

#ifndef DUMMYINITALIZABLEOBJECT_H
#define DUMMYINITALIZABLEOBJECT_H

#include <iostream>
#include "common/XMLConfigurationFiles/groupConfigurationStructure.h"
#include "common/AbstractFactoryPattern/InitializableObject.h"
// #include "common/AbstractFactoryPattern/InitializableObjectManager.h"
#include "common/XMLConfigurationFiles/groupConfigurationStructure.h"
#include "common/XMLConfigurationFiles/moduleConfigurationStructure.h"

namespace Lima
{

class DummyInitializableObject : public InitializableObject<DummyInitializableObject>
{
public:

  virtual void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override
  {
    LIMA_UNUSED(unitConfiguration);
    LIMA_UNUSED(manager);
    std::cout << "Dummy:init" << std::endl;
  };

};

class VeryDummyInitializableObject : public DummyInitializableObject
{
public:
  virtual void init(
      Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
      Manager* manager) override
  {
    LIMA_UNUSED(unitConfiguration);
    LIMA_UNUSED(manager);
    std::cout << "VeryDummy:init" << std::endl;
  };
};

class NotSoDummyInitializableObject : public DummyInitializableObject
{
public:
  virtual void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override
  {
    LIMA_UNUSED(unitConfiguration);
    LIMA_UNUSED(manager);
    std::cout << "NotSoDummy:init" << std::endl;
  };
};

} // Lima

#endif
