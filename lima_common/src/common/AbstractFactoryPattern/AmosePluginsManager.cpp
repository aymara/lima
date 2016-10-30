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
#include "common/tools/FileUtils.h"

#include <iostream>
#include <QFile>
#include <QDir>

using namespace Lima;
using namespace Lima::Common;
using namespace Lima::Common::Misc;

AmosePluginsManager::AmosePluginsManager()
{
//   loadPlugins();
}

bool AmosePluginsManager::loadPlugins(const QString& configDirs)
{
#ifdef DEBUG_CD
  ABSTRACTFACTORYPATTERNLOGINIT;
  LINFO << "AmosePluginsManager::loadPlugins" << configDirs;
#endif
  //   DynamicLibrariesManager::changeable().addSearchPath("c:\amose\lib");;
  // open LIMA_CONF/plugins file
  
  QStringList configDirsList = configDirs.split(LIMA_PATH_SEPARATOR);
  if (configDirsList.isEmpty())
  {
    // Look for LIMA_CONF directory.
    configDirsList = buildConfigurationDirectoriesList(QStringList() << "lima", QStringList());
  }
  for(auto it = configDirsList.begin(); it != configDirsList.end(); ++it)
  {
    // Deduce plugins directory.
    QString stdPluginsDir(*it);
    stdPluginsDir.append("/plugins");
    QDir pluginsDir(stdPluginsDir);
#ifdef DEBUG_CD
    ABSTRACTFACTORYPATTERNLOGINIT;
    LDEBUG << "AmosePluginsManager::loadPlugins in folder" << stdPluginsDir;
#endif
    
    // For each file under plugins directory, read plugins names and deduce shared libraries to load.
    QStringList pluginsFiles = pluginsDir.entryList(QDir::Files);
    Q_FOREACH(QString pluginsFile, pluginsFiles)
    {
#ifdef DEBUG_CD
     LDEBUG << "AmosePluginsManager::loadPlugins loading plugins file " 
            << pluginsDir.path()+"/"+pluginsFile.toUtf8().data();
#endif
      // Open plugin file.
      QFile file(pluginsDir.path() + "/" + pluginsFile);
      if (!file.open(QIODevice::ReadOnly)) {
        ABSTRACTFACTORYPATTERNLOGINIT;
        LERROR << "AmosePluginsManager::loadPlugins: cannot open plugins file " 
                << pluginsFile.toUtf8().data();
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
  }
  return true;
}
