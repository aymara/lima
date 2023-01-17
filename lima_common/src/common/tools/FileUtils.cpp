// Copyright 2015 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
  // qDebug() << "buildConfigurationDirectoriesList" << projects << paths;
  QStringList configDirs;

 // 1. Add the paths explicitly given
  appendNonEmptyDirs(configDirs, paths);

  // 2. Add dirs from LIMA_CONF if no project is given
  if (projects.empty())
  {
    QString var = getEnvVar(QString::fromStdString("lima") + "_CONF");
    if ( !var.isEmpty() )
    {
      configDirs << var.split(LIMA_PATH_SEPARATOR);
    }
  }
  else
  {
    // 3. Add dirs from each PROJECT_CONF
    for (const auto& project: projects)
    {
      QString var = getEnvVar(project.toUpper() + "_CONF");
      if ( !var.isEmpty() )
      {
        configDirs << var.split(LIMA_PATH_SEPARATOR);
      }
    }
  }
  // 4. Add conf dir in XDG_DATA_HOME or ~/.local/share/ after LIMA_CONF but before /usr
  QString dataHome = getDataHome();
  appendNonEmptyDirs(configDirs, QStringList(dataHome + "/lima/config/"));

  // 5. Then add the *_CONF for each given project and complete if necessary with *_DIST/… or /usr/…
  if ( configDirs.isEmpty() )
  {
    for (const auto& project: projects)
    {
        appendFirstOrSecondForProject(configDirs, project, "/share/config/", "");
    }
  }

  QString var = getEnvVar(QString::fromStdString("LIMA_SHOW_CONFIG_PATH"));
  if ( !var.isEmpty() )
  {
    qDebug() << "LIMA Configuration directories list built is:" << configDirs;
  }

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

  QString var = getEnvVar(QString::fromStdString("LIMA_SHOW_CONFIG_PATH"));
  if ( !var.isEmpty() )
  {
    qDebug() << "LIMA Resources directories list built is:" << resourcesDirs;
  }
  LOGINIT("FilesReporting");
  LINFO << "LIMA Resources directories list built is:" << resourcesDirs;
  return resourcesDirs;
}

QString findFileInPaths(const QString& paths, const QString& fileName, const QChar& separator)
{
  auto pathsList = paths.split(separator);
  auto filesList =fileName.split(separator);
  for (const auto& path: pathsList)
  {
    for (const auto& file: filesList)
    {
      if (QFileInfo::exists(path + "/" + file))
      {
  #ifndef WIN32 // Windows do not support circular dependency between qslog and tools libraries
        {
        LOGINIT("FilesReporting");
        LDEBUG << "File found:" << path + "/" + file;
        }
  #endif
        return path + "/" + file;
      }
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
