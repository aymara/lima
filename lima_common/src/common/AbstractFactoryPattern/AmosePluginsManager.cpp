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
#include "AmosePluginsManager.h"
#include "common/LimaCommon.h"
#include "common/AbstractFactoryPattern/DynamicLibrariesManager.h"

#include <iostream>
#include <QFile>
#include <QDir>

using namespace Lima;
using namespace Lima::Common;

AmosePluginsManager::AmosePluginsManager()
{
  loadPlugins();
}

bool AmosePluginsManager::loadPlugins()
{
  ABSTRACTFACTORYPATTERNLOGINIT;
  LINFO << "AmosePluginsManager::loadPlugins";
//   DynamicLibrariesManager::changeable().addSearchPath("c:\amose\lib");;
  // open LIMA_CONF/plugins file
  
  // Look for LIMA_CONF directory.
  std::string configDir = qgetenv("LIMA_CONF").constData()==0?"":qgetenv("LIMA_CONF").constData();
  if (configDir.empty())
  {
    configDir = "/usr/share/config/lima/";
  }
  
  // Deduce plugins directory.
  std::string stdPluginsDir(configDir);
  stdPluginsDir.append("/plugins");
  QDir pluginsDir(QString::fromUtf8(stdPluginsDir.c_str()));
  
  // For each file under plugins directory, read plugins names and deduce shared libraries to load.
  QStringList pluginsFiles = pluginsDir.entryList(QDir::Files);
  Q_FOREACH(QString pluginsFile, pluginsFiles)
  {
#ifdef DEBUG_CD
   LDEBUG << "AmosePluginsManager::loadPlugins loading plugins file " << pluginsFile.toUtf8().data();
#endif
    // Open plugin file.
    QFile file(pluginsDir.path() + "/" + pluginsFile);
    if (!file.open(QIODevice::ReadOnly)) {
      LERROR << "AmosePluginsManager::loadPlugins: cannot open plugins file " << pluginsFile.toUtf8().data();
      return false;
    }
    
    // For each entry, call load library
    while (!file.atEnd()) 
    {
      // Remove whitespace characters from the start and the end.
      QString line = QString(file.readLine()).trimmed();
      
      // Allow empty and comment lines.
      if ( !line.isEmpty() && !line.startsWith('#') )
      {
#ifdef DEBUG_CD
        LDEBUG << "AmosePluginsManager::loadPlugins loading plugin '" << line.toStdString().c_str() << "'";
#endif
        DynamicLibrariesManager::changeable().loadLibrary(line.toStdString().c_str());
      }
    }
  }
  return true;
}
