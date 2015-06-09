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
/** **********************************************************************
 *
 * @file       DynamicLibrariesManager.h
 * @author     Romaric Besancon <romaric.besancon@cea.fr> (initial implementation)
 * @author     Gael de Chalendar <gael.de-chalendar@cea.fr> (port to QLibrary)
 * @date       Wed Feb  6 2008
 * copyright   Copyright (C) 2008-2012 by CEA LIST
 * Project     common
 * 
 * @brief      handle dynamic library opening (wrapper around QLibrary)
 * 
 * 
 ***********************************************************************/

#ifndef DYNAMICLIBRARIES_H
#define DYNAMICLIBRARIES_H

#include "common/AbstractFactoryPattern/AbstractFactoryPatternExport.h"
#include "common/AbstractFactoryPattern/Singleton.h"

#include <QtCore/QLibrary>

#include <string>
#include <map>
#include <vector>

class QString;
namespace Lima {
namespace Common {

class LIMA_FACTORY_EXPORT DynamicLibrariesManager:
    public Singleton<DynamicLibrariesManager>
{
friend class Singleton<DynamicLibrariesManager>;
 public:
  ~DynamicLibrariesManager();
  
  bool isLoaded(const std::string& libName);
  bool loadLibrary(const std::string& libName);
  void addSearchPath(const std::string& searchPath);
  void addSearchPathes(QString searchPathes);

 private:
  DynamicLibrariesManager();

  std::map<std::string,QLibrary*> m_handles;
  // at load time, will try to load the libraries from these paths before the default ones
  std::vector<std::string> m_supplementarySearchPath;
};

} // end namespace
} // end namespace

#endif
