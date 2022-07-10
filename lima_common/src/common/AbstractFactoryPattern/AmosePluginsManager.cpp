// Copyright 2002-2020 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
  LINFO << "AmosePluginsManager::loadPlugins" << configDirs;

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
    LINFO << "AmosePluginsManager::loadPlugins in folder" << stdPluginsDir;

    // For each file under plugins directory, read plugins names and deduce shared libraries to load.
    QStringList pluginsFiles = pluginsDir.entryList(QDir::Files);
    Q_FOREACH(QString pluginsFile, pluginsFiles)
    {
      LINFO << "AmosePluginsManager::loadPlugins loading plugins file "
             << pluginsDir.path()+"/"+pluginsFile.toUtf8().data();
      // Open plugin file.
      QFile file(pluginsDir.path() + "/" + pluginsFile);
      if (!file.open(QIODevice::ReadOnly))
      {
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
        if ( !line.isEmpty() && !line.startsWith('#')
          && !forbiddenPlugins.contains(line)
          && !alreadyLoaded.contains(line)
        )
        {
          LDEBUG << "AmosePluginsManager::loadPlugins loading plugin '" << line.toStdString().c_str() << "'";

          if (!DynamicLibrariesManager::changeable().loadLibrary(line.toStdString().c_str()))
          {
            LERROR << "AmosePluginsManager::loadLibrary(\""
                   << line.toStdString() << "\") failed while handling"
                   << (pluginsDir.path() + "/" + pluginsFile) << ".";
            return false;
          }
          else
          {
            alreadyLoaded << line;
          }
        }
        else if (alreadyLoaded.contains(line))
        {
          LDEBUG << "AmosePluginsManager::loadLibrary plugin" << line
                 << "was alreadyLoaded in another plugins dir. Plugins file:"
                 << pluginsDir.path() + "/" + pluginsFile;
        }
        else if (forbiddenPlugins.contains(line))
        {
          LDEBUG << "AmosePluginsManager::loadLibrary plugin" << line
                 << "was set to be forbidden in anothe plugins dir. Plugins file:"
                 << pluginsDir.path() + "/" + pluginsFile;
        }
        else if (line.startsWith('#'))
        {
          line.remove(0,1);
          LDEBUG << "AmosePluginsManager::loadLibrary plugin" << line
                 << "forbidden from now on. Plugins file:"
                 << pluginsDir.path() + "/" + pluginsFile;
          forbiddenPlugins << line;
        }
      }
    }
  }
  return true;
}
