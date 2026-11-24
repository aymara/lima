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

#ifdef Q_OS_WIN
#include <windows.h>
#elif defined(Q_OS_MACOS)
#include <dlfcn.h>
#endif

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
#ifdef Q_OS_WIN
    // As Python's os.path.expanduser (used by lima_models.py to install the
    // models): HOME is usually not set on Windows.
    dataHome = QString::fromUtf8(qgetenv("USERPROFILE"));
#else
    dataHome = QString::fromUtf8(qgetenv("HOME"));
#endif
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

// Install prefix deduced from the location of this library
// (<prefix>/bin/lima-common-tools.dll, <prefix>/lib/liblima-common-tools.dylib),
// or an empty string. Used on Windows and macOS, where LIMA is not installed in
// a system location like /usr, so that an installed LIMA
// works without LIMA_CONF/LIMA_RESOURCES, whatever the executable (e.g. the
// Python interpreter for the Python bindings).
QString libraryInstallPrefix()
{
#ifdef Q_OS_WIN
  HMODULE module = nullptr;
  if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS
                           | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                         reinterpret_cast<LPCWSTR>(&libraryInstallPrefix),
                         &module))
  {
    wchar_t path[4096];
    auto length = GetModuleFileNameW(module, path, sizeof(path) / sizeof(path[0]));
    if (length > 0 && length < sizeof(path) / sizeof(path[0]))
    {
      QDir binDir = QFileInfo(QString::fromWCharArray(path, length)).absoluteDir();
      return QDir::cleanPath(binDir.absoluteFilePath(".."));
    }
  }
#elif defined(Q_OS_MACOS)
  // <prefix>/lib/liblima-common-tools.dylib
  Dl_info info;
  if (dladdr(reinterpret_cast<const void*>(&libraryInstallPrefix), &info) != 0
      && info.dli_fname != nullptr)
  {
    QDir libDir = QFileInfo(QString::fromUtf8(info.dli_fname)).absoluteDir();
    return QDir::cleanPath(libDir.absoluteFilePath(".."));
  }
#endif
  return QString();
}

// Appends the project directory under $<PROJECT>_DIST if it exists, else
// under the install prefix of this library (Windows) if it exists, else under
// /usr.
void appendFirstOrSecondForProject(QStringList& target,
                                   const QString& project,
                                   const QString& path_prefix,
                                   const QString& path_postfix = QString(""))
{
  QString systemPath = QString::fromUtf8("/usr/") + path_prefix + project + path_postfix;
  QString prefix = libraryInstallPrefix();
  if (!prefix.isEmpty()
      && QDir(prefix + path_prefix + project + path_postfix).exists())
  {
    systemPath = prefix + path_prefix + project + path_postfix;
  }
  appendFirstOrSecond(target,
                      getEnvVar(project + "_DIST") + path_prefix + project + path_postfix,
                      systemPath);
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
  FILESREPORTLOGINIT;
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
          FILESREPORTLOGINIT;
        LDEBUG << "File found:" << path + "/" + file;
        }
  #endif
        return path + "/" + file;
      }
    }
  }
  FILESREPORTLOGINIT;
  LINFO << "findFileInPaths no '" << fileName << "' found in '" << paths
            << "' separated by '" << separator << "'";
  return QString();
}


} // end namespace
} // end namespace
} // end namespace
