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
#include <iostream>

#include "common/LimaCommon.h"
#include "MainFactoriesMap.h"

namespace Lima
{

  class MainFactoriesMap::MainFactoriesMapPrivate
  {

    friend class MainFactoriesMap;

    ~MainFactoriesMapPrivate()
    {
      if (nullptr != s_mainFactoriesMap)
      {
        delete s_mainFactoriesMap;
        s_mainFactoriesMap = nullptr;
        shuttingDown = true;
      }
    }

  private:

    static bool shuttingDown;
    static MainFactoryMap *s_mainFactoriesMap;
  };

  MainFactoryMap* MainFactoriesMap::MainFactoriesMapPrivate::s_mainFactoriesMap = nullptr;
  bool MainFactoriesMap::MainFactoriesMapPrivate::shuttingDown = false;

  MainFactoryMap& MainFactoriesMap::get()
  {
    if (MainFactoriesMap::MainFactoriesMapPrivate::shuttingDown)
      throw LimaException("Access to s_mainFactoriesMap while shutting down");

    if (nullptr == MainFactoriesMap::MainFactoriesMapPrivate::s_mainFactoriesMap)
      MainFactoriesMap::MainFactoriesMapPrivate::s_mainFactoriesMap = new MainFactoryMap;

    return *MainFactoriesMap::MainFactoriesMapPrivate::s_mainFactoriesMap;
  }

}
