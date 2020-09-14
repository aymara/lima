/*
    Copyright 2002-2020 CEA LIST

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
  ABSTRACTFACTORYPATTERNLOGINIT;
  LERROR << "AmosePluginsManager::loadPlugins" << configDirs;
  std::cerr << "AmosePluginsManager::loadPlugins" << configDirs.toStdString() << std::endl;
  Common::DynamicLibrariesManager::changeable().addSearchPathes(configDirs);

  QStringList forbiddenPlugins;
  QStringList alreadyLoaded;
  //   DynamicLibrariesManager::changeable().addSearchPath("c:\amose\lib");;
  // open LIMA_CONF/plugins file

  QStringList configDirsList = configDirs.split(LIMA_PATH_SEPARATOR);
  if (configDirsList.isEmpty())
  {
    // Look for LIMA_CONF directory.
    configDirsList = buildConfigurationDirectoriesList(QStringList({"lima"}),
                                                       QStringList());
  }
  for(auto it = configDirsList.begin(); it != configDirsList.end(); ++it)
  {
    // Deduce plugins directory.
    QString stdPluginsDir(*it);
    stdPluginsDir.append("/plugins");
    QDir pluginsDir(stdPluginsDir);
    LERROR << "AmosePluginsManager::loadPlugins in folder" << stdPluginsDir;
    std::cerr << "AmosePluginsManager::loadPlugins in folder" << stdPluginsDir.toStdString() << std::endl;

    // For each file under plugins directory, read plugins names and deduce shared libraries to load.
    QStringList pluginsFiles = pluginsDir.entryList(QDir::Files);
    Q_FOREACH(QString pluginsFile, pluginsFiles)
    {
      LERROR << "AmosePluginsManager::loadPlugins loading plugins file "
             << pluginsDir.path()+"/"+pluginsFile.toUtf8().data();
      std::cerr << "AmosePluginsManager::loadPlugins loading plugins file " << (pluginsDir.path()+"/"+pluginsFile.toUtf8().data()).toStdString() << std::endl;
      // Open plugin file.
      QFile file(pluginsDir.path() + "/" + pluginsFile);
      if (!file.open(QIODevice::ReadOnly))
      {
        ABSTRACTFACTORYPATTERNLOGINIT;
        LERROR << "AmosePluginsManager::loadPlugins: cannot open plugins file "
               << pluginsFile.toUtf8().data();
        std::cerr << "AmosePluginsManager::loadPlugins: cannot open plugins file " << pluginsFile.toUtf8().data() << std::endl;
        return false;
      }

      // For each entry, call load library
      while (!file.atEnd())
      {
        // Remove whitespace characters from the start and the end.
        QString line = QString(file.readLine()).trimmed();

        // Allow empty and comment lines.
        if ( !line.isEmpty() && !line.startsWith('#')
          && !forbiddenPlugins.contains(line)
          && !alreadyLoaded.contains(line)
        )
        {
          LERROR << "AmosePluginsManager::loadPlugins loading plugin '" << line.toStdString().c_str() << "'";
          std::cerr << "AmosePluginsManager::loadPlugins loading plugin '" << line.toStdString().c_str() << "'" << std::endl;
          if (!DynamicLibrariesManager::changeable().loadLibrary(line.toStdString().c_str()))
          {
            LERROR << "AmosePluginsManager::loadLibrary(\""
                   << line.toStdString() << "\") failed while handling"
                   << (pluginsDir.path() + "/" + pluginsFile) << ".";
            std::cerr << "AmosePluginsManager::loadLibrary(\""
                      << line.toStdString() << "\") failed while handling"
                      << (pluginsDir.path() + "/" + pluginsFile).toStdString() << "." << std::endl;
            return false;
          }
          else
          {
            alreadyLoaded << line;
          }
        }
        else if (alreadyLoaded.contains(line))
        {
          LERROR << "AmosePluginsManager::loadLibrary plugin" << line
                  << "was alreadyLoaded in another plugins dir. Plugins file:"
                  << pluginsDir.path() + "/" + pluginsFile;
          std::cerr << "AmosePluginsManager::loadLibrary plugin" << line.toStdString()
                    << "was alreadyLoaded in another plugins dir. Plugins file:"
                    << (pluginsDir.path() + "/" + pluginsFile).toStdString() << std::endl;
        }
        else if (forbiddenPlugins.contains(line))
        {
          LERROR << "AmosePluginsManager::loadLibrary plugin" << line
                  << "was set to be forbidden in anothe plugins dir. Plugins file:"
                  << pluginsDir.path() + "/" + pluginsFile;
          std::cerr << "AmosePluginsManager::loadLibrary plugin" << line.toStdString()
                  << "was set to be forbidden in anothe plugins dir. Plugins file:"
                  << (pluginsDir.path() + "/" + pluginsFile).toStdString() << std::endl;
        }
        else if (line.startsWith('#'))
        {
          line.remove(0,1);
          LERROR << "AmosePluginsManager::loadLibrary plugin" << line
                  << "forbidden from now on. Plugins file:"
                  << pluginsDir.path() + "/" + pluginsFile;
          std::cerr << "AmosePluginsManager::loadLibrary plugin" << line.toStdString()
                    << "forbidden from now on. Plugins file:"
                    << (pluginsDir.path() + "/" + pluginsFile).toStdString() << std::endl;
          forbiddenPlugins << line;
        }
      }
    }
  }
  return true;
}
