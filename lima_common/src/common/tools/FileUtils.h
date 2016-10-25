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

#ifndef LIMA_COMMON_MISC_FILEUTILS_H
#define LIMA_COMMON_MISC_FILEUTILS_H

#include "common/LimaCommon.h"

#include <QFile>
#include <QStringList>

#include <iostream>

#ifdef WIN32
#define LIMA_PATH_SEPARATOR QChar(';')
#else
#define LIMA_PATH_SEPARATOR QChar(':')
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
 * For each project name "project", try to add the dir from the environment 
 * variable $PROJECT_CONF. If it does not exist, try 
 * $PROJECT_DIST/share/config/project. If it does not exist either, try 
 * /usr/share/config/project.
 * Then add existing paths from the given list.
 * In LIMA the projects list will be limited to the single element "lima" but 
 * projects depending on LIMA will be able to add their own separate 
 * configurations.
 * 
 * @param projects The list of project names to explore 
 * @param paths The list of paths to look into.
 */
LIMA_COMMONTOOLS_EXPORT QStringList buildConfigurationDirectoriesList(const QStringList& projects, 
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
 * @param paths the list of concatenated paths to search th file in
 * @param fileName the name of the file  to search into the paths. Can include a relative path
 * @param separator the character used to split the list of paths. Defaults to semicolon
 * @return the full path of the found file if found. Empty string otherwise.
 */
LIMA_COMMONTOOLS_EXPORT QString findFileInPaths(const QString& paths, const QString& fileName, const QChar& separator = LIMA_PATH_SEPARATOR);

} // end namespace
} // end namespace
} // end namespace

#endif
