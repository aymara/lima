# Copyright 2002-2020 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

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
  Qt${QT_VERSION_MAJOR}::Core
)

set_target_properties(lima-common-tools PROPERTIES
  VERSION ${LIMA_COMMON_LIB_VERSION}
  SOVERSION ${LIMA_COMMON_LIB_SOVERSION})

install(TARGETS lima-common-tools
  DESTINATION ${LIB_INSTALL_DIR}
  COMPONENT runtime)

IF(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  add_subdirectory(tests)
endif ()

########### install files ###############
install(FILES
    FileUtils.h
    LimaMainTaskRunner.h
    LimaFileSystemWatcher.h
  DESTINATION include/common/tools
  COMPONENT devel
)
