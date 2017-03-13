#   Copyright 2002-2015 CEA LIST
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

find_package(log4cpp)
if (LOG4CPP_FOUND)
  message(STATUS "Use log4cpp logger")
  include_directories(${LOG4CPP_INCLUDE_DIRS})
  add_definitions(-DUSE_LOG4CPP)
else ()
  message(STATUS "Use default logger")
endif()

########### next target ###############

set (lima-common-tools_MOC_HEADERS
    ../QsLog/QsLogCategories.h
    LimaFileSystemWatcher.h
    LimaFileSystemWatcher_p.h
    LimaMainTaskRunner.h)

qt5_wrap_cpp (lima-common-tools_MOC_OUTFILES ${lima-common-tools_MOC_HEADERS})

SET(lima-common-tools_LIB_SRCS 
    ../QsLog/QsLog.cpp
    ../QsLog/QsDebugOutput.cpp
    ../QsLog/QsLogDest.cpp
    ../QsLog/QsLogCategories.cpp
    LimaFileSystemWatcher.cpp
    LimaMainTaskRunner.cpp
    FileUtils.cpp
  )

if (NOT (${CMAKE_SYSTEM_NAME} STREQUAL "Windows"))
SET(lima-common-tools_LIB_SRCS
    ${lima-common-tools_LIB_SRCS}
  )

endif()
add_library(lima-common-tools SHARED ${lima-common-tools_LIB_SRCS} ${lima-common-tools_MOC_OUTFILES})
target_link_libraries(lima-common-tools 
  ${BOOST_LIBRARIES} 
  ${QT_LIBRARIES}
  ${LOG4CPP_LIBRARIES}
)

set_target_properties(lima-common-tools PROPERTIES VERSION ${LIMA_COMMON_LIB_VERSION} SOVERSION ${LIMA_COMMON_LIB_SOVERSION})
install(TARGETS lima-common-tools DESTINATION ${LIB_INSTALL_DIR})

add_subdirectory(tests)

########### install files ###############
install(FILES
    FileUtils.h
    LimaMainTaskRunner.h
    LimaFileSystemWatcher.h
  DESTINATION include/common/tools
  COMPONENT devel
)
