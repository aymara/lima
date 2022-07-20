// Copyright 2002-2019 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
