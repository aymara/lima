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
