#   Copyright 2002-2020 CEA LIST
#
#   This file is part of LIMA.
#
#   LIMA is free software: you can redistribute it and/or modify
#   it under the terms of the GNU Affero General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   LIMA is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU Affero General Public License for more details.
#
#   You should have received a copy of the GNU Affero General Public License
#   along with LIMA.  If not, see <http://www.gnu.org/licenses/>
include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )

add_definitions( -DLIMA_COMMONTOOLS_EXPORTING )
add_definitions( -DLIMA_COMMONQSLOG_EXPORTING )

########### next target ###############

SET(lima-common-tools_LIB_SRCS
    ../QsLog/QsLog.cpp
    ../QsLog/QsDebugOutput.cpp
    ../QsLog/QsLogDest.cpp
    ../QsLog/QsLogCategories.cpp
    LimaFileSystemWatcher.cpp
    LimaMainTaskRunner.cpp
    FileUtils.cpp
  )

add_library(lima-common-tools SHARED ${lima-common-tools_LIB_SRCS})
ADD_DLL_TO_PATH(lima-common-tools)

target_link_libraries(lima-common-tools
  Qt5::Core
)

set_target_properties(lima-common-tools PROPERTIES
  VERSION ${LIMA_COMMON_LIB_VERSION}
  SOVERSION ${LIMA_COMMON_LIB_SOVERSION})

install(TARGETS lima-common-tools
  DESTINATION ${LIB_INSTALL_DIR}
  COMPONENT runtime)

add_subdirectory(tests)

########### install files ###############
install(FILES
    FileUtils.h
    LimaMainTaskRunner.h
    LimaFileSystemWatcher.h
  DESTINATION include/common/tools
  COMPONENT devel
)
