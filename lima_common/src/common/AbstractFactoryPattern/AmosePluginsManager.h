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
#ifndef AMOSEPLUGINMANAGERS_H
#define AMOSEPLUGINMANAGERS_H

#include "common/AbstractFactoryPattern/AbstractFactoryPatternExport.h"
#include "common/AbstractFactoryPattern/Singleton.h"

#include <QtCore/QString>

namespace Lima
{

class LIMA_FACTORY_EXPORT AmosePluginsManager :
public Singleton<AmosePluginsManager>
{
  friend class Singleton<AmosePluginsManager>;
public:
  virtual ~AmosePluginsManager() {}

  /**
   * @brief Load plugins in the 'plugins' subdir of the semicolon separated
   * config dirs
   *
   * If a plugin is present in several config dirs, then only the first
   * occurrence is loaded.
   *
   * If a plugin is commented out (line starting with the '#' symbol), then it
   * is considered as being forbidenned and then it will be ignored if found
   * again in subsequent directories.
   *
   * @param configDirs semicolon separated list of config dirs. If empty, loads
   * a default location.
   *
   * @return true if there is no error and false otherwise. The function return
   * at the first error.
   */
  bool loadPlugins(const QString& configDirs = "");

private:
  AmosePluginsManager();

};

}
#endif
