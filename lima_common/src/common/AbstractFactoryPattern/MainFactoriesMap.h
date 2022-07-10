// Copyright 2002-2019 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef MAINFACTORIESMAP_H
#define MAINFACTORIESMAP_H

#include "common/AbstractFactoryPattern/AbstractFactoryPatternExport.h"

#include <map>
#include <string>

namespace Lima
{

  typedef std::map<std::string, void*> MainFactoryMap;

  class LIMA_FACTORY_EXPORT MainFactoriesMap
  {

  public:

    static MainFactoryMap& get();

  private:

    class MainFactoriesMapPrivate;
  };

}

#endif
