/*
    Copyright 2015 CEA LIST

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
/************************************************************************
 * @file       FileUtils.h
 * @author     Gael de Chalendar
 * @date       Tue Jul  7 2015
 * copyright   Copyright (C) 2015 by CEA LIST
 ***********************************************************************/

#include "FileUtils.h"

#include <QFileInfo>
#include <QDir>

namespace Lima {
namespace Common {
namespace Misc {

uint64_t countLines(std::istream& file)
{
  uint64_t result = 0;
  std::streampos initialPosition = file.tellg();
  int c = file.get();
  while (c != -1)
  {
    while (c != -1 && c != '\n')
    {
      c = file.get();
    }
    result = result + 1;
    c = file.get();
  }
  file.clear();
  file.seekg(initialPosition, std::ios_base::beg);
  return result;
}

uint64_t countLines(QFile& file)
{
  uint64_t result = 0;
  qint64 initialPosition = file.pos();
  char c = '\0';
  while (!file.atEnd())
  {
    while (!file.atEnd() && c != '\n')
    {
      file.getChar(&c);
    }
    result = result + 1;
    file.getChar(&c);
  }
  file.seek(initialPosition);
  return result;
}

QStringList buildConfigurationDirectoriesList(const QStringList& projects, const QStringList& paths)
{
  qDebug() << "buildConfigurationDirectoriesList" << projects << paths;
  QStringList configDirs;
  for (auto path = paths.begin(); path != paths.end(); ++path)
  {
    if (!path->isEmpty() && QDir(*path).exists())
    {
      configDirs << *path;
    }
  }
  for (auto it = projects.begin(); it != projects.end(); ++it)
  {
    QString project = *it;
    QStringList confDirs;
    QString projectConf = QString::fromUtf8(qgetenv((project.toUpper()+"_CONF").toStdString().c_str()).constData());
    if (!projectConf.isEmpty())
      confDirs << projectConf.split(LIMA_PATH_SEPARATOR);
    for (auto configDir = confDirs.begin(); configDir != confDirs.end(); ++configDir)
    {
//       qDebug() << "buildConfigurationDirectoriesList testing given" << *configDir;
      if (!configDir->isEmpty() && QDir(*configDir).exists())
      {
        if (!configDirs.contains(*configDir))
        {
          configDirs << *configDir;
        }
      }
    }
    if (confDirs.isEmpty())
    {
      QString configDir = QString::fromUtf8(qgetenv((project.toUpper()+"_DIST").toStdString().c_str()).constData()) + "/share/config/" + project;
//       qDebug() << "buildConfigurationDirectoriesList testing project dist" << configDir;
      if (!configDir.isEmpty() && QDir( configDir ).exists() )
      {
        if (!configDirs.contains(configDir))
        {
          configDirs << configDir;
        }
      }
      else
      {
        configDir = QString::fromUtf8("/usr/share/config/") + project;
//         qDebug() << "buildConfigurationDirectoriesList testing usr" << configDir
//                   << configDir.isEmpty() << QDir( configDir ).exists();
        if (!configDir.isEmpty() && QDir( configDir ).exists() )
        {
          if (!configDirs.contains(configDir))
          {
            configDirs << configDir;
          }
        }
      }
    }

    // If current project is not lima, try to add a lima config dir for this project
    if (project != "lima")
    {
      QString configDir = QString::fromUtf8(qgetenv((project.toUpper()+"_DIST").toStdString().c_str()).constData()) + "/share/config/lima";
      if (!configDir.isEmpty() && QDir( configDir ).exists())
      {
        if (!configDirs.contains(configDir))
        {
          configDirs << configDir;
        }
      }
      else
      {
        configDir = QString::fromUtf8("/usr/share/config/lima") ;
        if (!configDir.isEmpty() && QDir( configDir ).exists() )
        {
          if (!configDirs.contains(configDir))
          {
            configDirs << configDir;
          }
        }
      }
    }
  }

  qDebug() << "buildConfigurationDirectoriesList result:" << configDirs;
  return configDirs;
}

QStringList buildResourcesDirectoriesList(const QStringList& projects, const QStringList& paths)
{
//   qDebug() << "buildResourcesDirectoriesList" << projects << paths;
  QStringList resourcesDirs;
  for (auto it = projects.begin(); it != projects.end(); ++it)
  {
    QString project = *it;
    QStringList resDirs;
    QString projectRes = QString::fromUtf8(qgetenv((project.toUpper()+"_RESOURCES").toStdString().c_str()).constData());
    if (!projectRes.isEmpty())
      resDirs << projectRes.split(LIMA_PATH_SEPARATOR);
    for (auto resourcesDir = resDirs.begin(); resourcesDir != resDirs.end(); ++resourcesDir)
    {
      if (QDir(*resourcesDir).exists())
      {
        resourcesDirs << *resourcesDir;
      }
    }
    if (resDirs.isEmpty())
    {
      QString resourcesDir = QString::fromUtf8(qgetenv((project.toUpper()+"_DIST").toStdString().c_str()).constData()) + "/share/apps/" + project + "/resources";
      if ( QDir( resourcesDir ).exists() )
      {
        resourcesDirs << resourcesDir;
      }
      else
      {
        resourcesDir = QString::fromUtf8("/usr/share/apps/") + project + "/resources";
        if ( QDir( resourcesDir ).exists() )
        {
          resourcesDirs << resourcesDir;
        }
      }
    }
    // If current project is not lima, try to add a lima resources dir for this project
    if (project != "lima")
    {
      QString resourcesDir = QString::fromUtf8(qgetenv((project.toUpper()+"_DIST").toStdString().c_str()).constData()) + "/share/apps/lima/resources";
      if ( QDir( resourcesDir ).exists() )
      {
        if (!resourcesDirs.contains(resourcesDir))
        {
          resourcesDirs << resourcesDir;
        }
      }
      else
      {
        resourcesDir = QString::fromUtf8("/usr/share/apps/lima/resources");
        if ( QDir( resourcesDir ).exists() )
        {
          if (!resourcesDirs.contains(resourcesDir))
          {
            resourcesDirs << resourcesDir;
          }
        }
      }
    }

  }
  for (auto path = paths.begin(); path != paths.end(); ++path)
  {
    if (!path->isEmpty() && QDir(*path).exists())
      resourcesDirs << *path;
  }

  LOGINIT("FilesReporting");
  LINFO << "Resources directories are:" << resourcesDirs;
  return resourcesDirs;
}

QString findFileInPaths(const QString& paths, const QString& fileName, const QChar& separator)
{
  QStringList pathsList = paths.split(separator);
  Q_FOREACH(QString path, pathsList)
  {
    if (QFileInfo::exists(path+ "/" + fileName))
    {
#ifndef WIN32 // Windows do not support circular dependency between qslog and tools libraries
      {
      LOGINIT("FilesReporting");
      LDEBUG << "File found:" << path+ "/" + fileName;
      }
#endif
      return path+ "/" + fileName;
    }
  }
  std::cerr << "WARNING: findFileInPaths no '" << fileName.toUtf8().constData()
            << "' found in '" << paths.toUtf8().constData()
            << "' separated by '" << separator.toLatin1() << "'" << std::endl;
  return QString();
}


} // end namespace
} // end namespace
} // end namespace
