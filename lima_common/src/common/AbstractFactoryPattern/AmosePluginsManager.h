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

#include <QtCore>

namespace Lima
{
  
class LIMA_FACTORY_EXPORT AmosePluginsManager :
public Singleton<AmosePluginsManager>
{
  friend class Singleton<AmosePluginsManager>;
public:
  virtual ~AmosePluginsManager() {}
  
  /** Load plugins in the plugins subdir of the semicolon separated config dirs 
   * @param configDirs semicolon separated list of config dirs. If empty, loads a default location
   */
  bool loadPlugins(const QString& configDirs = "");
  
private:
  AmosePluginsManager();

};

}
#endif
