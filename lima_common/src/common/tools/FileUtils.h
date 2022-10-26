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

#ifndef LIMA_COMMON_MISC_FILEUTILS_H
#define LIMA_COMMON_MISC_FILEUTILS_H

#include "common/LimaCommon.h"

#include <QtCore/QFile>

#include <iostream>

#ifdef WIN32
#define LIMA_PATH_SEPARATOR ';'
#else
#define LIMA_PATH_SEPARATOR ':'
#endif

namespace Lima {
namespace Common {
namespace Misc {

/**
 * Count the number of lines in the given file from the current position
 *
 * If the last line has no character (no character after the last line break)' it is not counted.
 * After this function, the file is in the same good state and at the same position.
 *
 * @param file the file to count the lines of
 *
 * @return the number of lines of the file
 */
LIMA_COMMONTOOLS_EXPORT uint64_t countLines(std::istream& file);

/**
 * Count the number of lines in the given file from the current position
 *
 * If the last line has no character (no character after the last line break)' it is not counted.
 * After this function, the file is at the same position.
 *
 * @param file the file to count the lines of
 *
 * @return the number of lines of the file
 */
LIMA_COMMONTOOLS_EXPORT uint64_t countLines(QFile& file);


/**
 * @brief Build a list of configuration directories from a list of project
 * names and a list of paths.
 *
 * 1. Add the paths explicitly given
 * 2. Add dirs from LIMA_CONF if no project is given
 * 3. Add dirs from each PROJECT_CONF
 * 4. Add conf dir in XDG_DATA_HOME or ~/.local/share/ after LIMA_CONF but before /usr
 * 5. Then add the *_CONF for each given project and complete if necessary with *_DIST/… or /usr/…
 *
 * In LIMA the @ref projects list will be empty (or just "lima") but
 * projects depending on LIMA will be able to add their own separate
 * configurations.
 *
 * If the environment variable LIMA_SHOW_CONFIG_PATH is defined and non-empty, then the list built is written on stderr
 *
 * @param projects The list of project names to explore
 * @param paths The list of paths to look into first.
 *
 * @return The list of directories that will be searched for configuration files
 */
LIMA_COMMONTOOLS_EXPORT QStringList buildConfigurationDirectoriesList(const QStringList& projects = QStringList(),
                                                               const QStringList& paths = QStringList() );

/**
 * @brief Build a list of resources directories from a list of project names
 * and a list of paths.
 *
 * For each project name "project", try to add the dir from the environment
 * variable $PROJECT_RESOURCES. If it does not exist, try
 * $PROJECT_DIST/share/apps/project/resources. If it does not exist either, try
 * /usr/share/apps/project/resources.
 * Then add existing paths from the given list.
 * In LIMA the projects list will be limited to the single element "lima" but
 * projects depending on LIMA will be able to add their own separate
 * resources.
 *
 * @param projects The list of project names to explore
 * @param paths The list of paths to look into.
 */
LIMA_COMMONTOOLS_EXPORT QStringList buildResourcesDirectoriesList(const QStringList& projects,
                                                           const QStringList& paths = QStringList());

/**
 * Find the given file in the given paths.
 * @param paths the list of concatenated paths to search the file in
 * @param fileName the name of the file  to search into the paths. Can include a relative path. Can be a concatenation
 *        of several possible file names. Stop on the first file found in the first path
 * @param separator the character used to split the list of paths. Defaults to semicolon
 * @return the full path of the found file if found. Empty string otherwise.
 */
LIMA_COMMONTOOLS_EXPORT QString findFileInPaths(const QString& paths, const QString& fileName,
                                                const QChar& separator = QLatin1Char(LIMA_PATH_SEPARATOR));

} // end namespace
} // end namespace
} // end namespace

#endif
