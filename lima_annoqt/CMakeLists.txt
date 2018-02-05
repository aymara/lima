project(annoqt)
cmake_minimum_required(VERSION 3.4.0)

enable_testing()

include(${CMAKE_SOURCE_DIR}/../SystemSpecificInformations.cmake)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/modules ${CMAKE_INSTALL_PREFIX})
include(${CMAKE_SOURCE_DIR}/../manageQt5.cmake)
addQt5Modules(Core Widgets Xml)
include_directories(SYSTEM ${Qt5_INCLUDES})
set(QT_LIBRARIES "${Qt5_LIBRARIES}")

FIND_PACKAGE(LIMACOMMON PATHS ${CMAKE_INSTALL_PREFIX}/share/apps/lima/cmake/Modules /usr/share/apps/lima/cmake/Modules)
IF(LIMACOMMON_FOUND)
  include_directories(SYSTEM ${LIMACOMMON_INCLUDE_DIR})
  link_directories( ${LIMACOMMON_LIBRARY_DIRS})
  set(LIMA_LP_LIB_VERSION ${LIMA_GENERIC_LIB_VERSION})
  set(LIMA_LP_LIB_SOVERSION ${LIMA_GENERIC_LIB_SOVERSION})
ELSE (LIMACOMMON_FOUND)
  MESSAGE(FATAL_ERROR "Could not find lima_common")
ENDIF(LIMACOMMON_FOUND)


add_subdirectory(src)


########### install files ###############


########### packaging ###############
INCLUDE(InstallRequiredSystemLibraries)

set(CPACK_MONOLITHIC_INSTALL 1)
SET(CPACK_PACKAGE_NAME "limaannoqt")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A text annotation GUI in LIMA")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_SYSTEM_NAME}")
SET(CPACK_STRIP_FILES "bin/testDict16;bin/testLexicon")
SET(CPACK_SOURCE_STRIP_FILES "")
set(CPACK_GENERATOR ${SPECIFIC_SYSTEM_PREFERED_CPACK_GENERATOR})

INCLUDE(CPack)
