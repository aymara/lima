cmake_minimum_required(VERSION 3.0)

enable_testing()

add_definitions(-D QT_NO_KEYWORDS)
include(${CMAKE_SOURCE_DIR}/../manageQt5.cmake)
addQt5Modules(Core Test Qml Gui)
include_directories(${Qt5_INCLUDES})

FIND_PACKAGE(LIMACOMMON PATHS ${CMAKE_INSTALL_PREFIX}/share/apps/lima/cmake/Modules)
IF(LIMACOMMON_FOUND)
  include_directories( AFTER ${LIMACOMMON_INCLUDE_DIR})
  link_directories( ${LIMACOMMON_LIBRARY_DIRS})
ELSE (LIMACOMMON_FOUND)
  MESSAGE(FATAL_ERROR "Could not find lima_common")
ENDIF(LIMACOMMON_FOUND)


FIND_PACKAGE(LIMALP PATHS ${CMAKE_INSTALL_PREFIX}/share/apps/lima/cmake/Modules)
IF(LIMALP_FOUND)
  include_directories( AFTER ${LIMALP_INCLUDE_DIR})
  link_directories( ${LIMALP_LIBRARY_DIRS})
ELSE (LIMALP_FOUND)
  MESSAGE(FATAL_ERROR "Could not find lima_linguisticprocessing")
ENDIF(LIMALP_FOUND)

add_subdirectory(src)

add_subdirectory(test)
