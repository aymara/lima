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
#include <QStandardPaths>

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

// Appends paths from source to target if following conditions are met:
// - path isn't empty
// - path isn't appended previously
// - path exists
void appendNonEmptyDirs(QStringList& target, const QStringList& source)
{
  for (const auto& path: source)
  {
    if ( !path.isEmpty() && !target.contains(path) && QDir(path).exists() )
    {
      target << path;
    }
  }
}

QString getEnvVar(const QString& name)
{
  return QString::fromUtf8(qgetenv((name.toUpper()).toStdString().c_str()).constData());
}

QString getDataHome()
{
  QString dataHome = QString::fromUtf8(qgetenv("XDG_DATA_HOME"));
  if (dataHome.isEmpty())
  {
    dataHome = QString::fromUtf8(qgetenv("HOME"));
    if (!dataHome.isEmpty())
    {
      dataHome += "/.local/share/";
    }
  }
  return dataHome;
}

// Appends path in case it exists. Otherwise appends alt_path
void appendFirstOrSecond(QStringList& target, const QString& path, const QString& alt_path)
{
  if ( QDir( path ).exists() )
  {
    appendNonEmptyDirs(target, QStringList(path));
  }
  else
  {
    appendNonEmptyDirs(target, QStringList(alt_path));
  }
}

// Calls appendFirstOrSecond for given project
void appendFirstOrSecondForProject(QStringList& target,
                                   const QString& project,
                                   const QString& path_prefix,
                                   const QString& path_postfix = QString(""))
{
  appendFirstOrSecond(target,
                      getEnvVar(project + "_DIST") + path_prefix + project + path_postfix,
                      QString::fromUtf8("/usr/") + path_prefix + project + path_postfix);
}

void buildDirectoriesListForProject(QStringList& target,
                                    const QString& project,
                                    const QString& env_postfix,               // "_CONF" or "_RESOURCES"
                                    const QString& path_prefix,               // "/share/config/" or "/share/apps"
                                    const QString& path_postfix = QString("") // "" or "/resources
                                    )
{
  QStringList dirs;
  QString var = getEnvVar(project + env_postfix);
  if ( !var.isEmpty() )
  {
    dirs << var.split(LIMA_PATH_SEPARATOR);
  }

  appendNonEmptyDirs(target, dirs);

  if ( dirs.isEmpty() )
  {
    appendFirstOrSecondForProject(target, project, path_prefix, path_postfix);
  }

}

QStringList buildConfigurationDirectoriesList(const QStringList& projects,
                                              const QStringList& paths)
{
  //qDebug() << "buildConfigurationDirectoriesList" << projects << paths;
  QStringList configDirs;

  QString dataHome = getDataHome();

  appendNonEmptyDirs(configDirs, QStringList(dataHome + "/lima/config/"));

  appendNonEmptyDirs(configDirs, paths);

  for (const auto& project: projects)
  {
    buildDirectoriesListForProject(configDirs, project, "_CONF", "/share/config/");
  }

  // If current project is not lima, try to add a lima config dir for this project
  if (!projects.contains("lima"))
  {
    appendFirstOrSecondForProject(configDirs, QString("lima"), "_CONF", "/share/config/");
  }

  //qDebug() << "buildConfigurationDirectoriesList result:" << configDirs;
  return configDirs;
}

QStringList buildResourcesDirectoriesList(const QStringList& projects,
                                          const QStringList& paths)
{
  // qDebug() << "buildResourcesDirectoriesList" << projects << paths;
  QStringList resourcesDirs;
  QString dataHome = getDataHome();

  appendNonEmptyDirs(resourcesDirs, QStringList(dataHome + "/lima/resources/"));

  for (const auto& project: projects)
  {
    buildDirectoriesListForProject(resourcesDirs, project, "_RESOURCES",
                                   "/share/apps/", "/resources");
  }

  appendNonEmptyDirs(resourcesDirs, paths);

  LOGINIT("FilesReporting");
  LINFO << "Resources directories are:" << resourcesDirs;
  return resourcesDirs;
}

QString findFileInPaths(const QString& paths, const QString& fileName, const QChar& separator)
{
  QStringList pathsList = paths.split(separator);
  Q_FOREACH(QString path, pathsList)
  {
    if (QFileInfo::exists(path + "/" + fileName))
    {
#ifndef WIN32 // Windows do not support circular dependency between qslog and tools libraries
      {
      LOGINIT("FilesReporting");
      LDEBUG << "File found:" << path + "/" + fileName;
      }
#endif
      return path + "/" + fileName;
    }
  }
  LOGINIT("FilesReporting");
  LWARN << "findFileInPaths no '" << fileName << "' found in '" << paths
            << "' separated by '" << separator << "'";
  return QString();
}


} // end namespace
} // end namespace
} // end namespace
