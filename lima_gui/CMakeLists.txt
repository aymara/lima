#   Copyright 2017 CEA LIST
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
project(lima_gui)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG_LIMA_GUI" )

enable_testing()

include(GNUInstallDirs)
include(${CMAKE_SOURCE_DIR}/../SystemSpecificInformations.cmake)
message("System name is ${CMAKE_SYSTEM_NAME}")

include(${CMAKE_SOURCE_DIR}/../SetCompilerFlags.cmake)

# Find*.cmake will be searched in the following dirs if not found in system dirs
set(CMAKE_MODULE_PATH
    "${CMAKE_SOURCE_DIR}/cmake/Modules" # for those available in this project
    "${CMAKE_INSTALL_PREFIX}/share/apps/lima/cmake/Modules" # for those available in the destination dir
    "$ENV{LIMA_EXTERNALS}/share/apps/cmake/Modules" # for those available in the externals dir
    "$ENV{LIMA_EXTERNALS}/share/cmake/Modules" # for those available in the externals dir
    "/usr/share/apps/lima/cmake/Modules" # for those available on the system
    "/usr/share/cmake/Modules" # for those available on the system
)

# find_path and find_library will search in these dirs too
set(CMAKE_PREFIX_PATH
    "${CMAKE_PREFIX_PATH}"
    "${CMAKE_INSTALL_PREFIX}"
    "$ENV{LIMA_EXTERNALS}"
)


add_definitions(-D QT_NO_KEYWORDS)
include(${CMAKE_SOURCE_DIR}/../manageQt5.cmake)
addQt5Modules(Core Test Qml Quick QuickTest Gui )
include_directories(${Qt5_INCLUDES})

# if (Qt5Core_VERSION VERSION_LESS 5.6.0)
#     message("Minimum supported Qt5 version for LIMA GUI is 5.6! LIMA GUI will not be built.")
# else()

  FIND_PACKAGE(LIMACOMMON PATHS ${CMAKE_INSTALL_PREFIX}/share/apps/lima/cmake/Modules /usr/share/apps/lima/cmake/Modules)
  IF(LIMACOMMON_FOUND)
    include_directories( AFTER ${LIMACOMMON_INCLUDE_DIR})
    link_directories( ${LIMACOMMON_LIBRARY_DIRS})
  ELSE (LIMACOMMON_FOUND)
    MESSAGE(FATAL_ERROR "Could not find lima_common")
  ENDIF(LIMACOMMON_FOUND)


  FIND_PACKAGE(LIMALP PATHS PATHS ${CMAKE_INSTALL_PREFIX}/share/apps/lima/cmake/Modules /usr/share/apps/lima/cmake/Modules)
  IF(LIMALP_FOUND)
    include_directories( AFTER ${LIMALP_INCLUDE_DIR})
    link_directories( ${LIMALP_LIBRARY_DIRS})
  ELSE (LIMALP_FOUND)
    MESSAGE(FATAL_ERROR "Could not find lima_linguisticprocessing")
  ENDIF(LIMALP_FOUND)

  add_subdirectory(src)
  add_subdirectory(test)

# endif()

add_subdirectory(conf)
########### packaging ###############
include(InstallRequiredSystemLibraries)

# set(CPACK_MONOLITHIC_INSTALL 1)
set(CPACK_PACKAGE_NAME "limagui")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "LIMA Graphical User Interface")
set(CPACK_PACKAGE_VENDOR "CEA LIST")
set(CPACK_PACKAGE_CONTACT "Gaël de Chalendar <Gael.de-Chalendar@cea.fr>")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
SET(CPACK_PACKAGE_VERSION_MAJOR "${LIMA_VERSION_MAJOR}")
SET(CPACK_PACKAGE_VERSION_MINOR "${LIMA_VERSION_MINOR}")
SET(CPACK_PACKAGE_VERSION_PATCH "${LIMA_VERSION_RELEASE}")
SET(CPACK_PACKAGE_VERSION "${LIMA_VERSION_MAJOR}.${LIMA_VERSION_MINOR}.${LIMA_VERSION_RELEASE}")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "Lima-${LIMA_VERSION_MAJOR}.${LIMA_VERSION_MINOR}")

set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")

# set of commands to define component based packaging
# notify cpack of component all names
set(CMAKE_INSTALL_DEFAULT_COMPONENT_NAME "runtime")
SET(CPACK_COMPONENTS_ALL devel runtime)
set(CPACK_RPM_COMPONENT_INSTALL ON)
set(CPACK_DEB_COMPONENT_INSTALL ON)

SET(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
SET(CPACK_NSIS_MODIFY_PATH "ON")
SET(CPACK_STRIP_FILES "")
SET(CPACK_SOURCE_STRIP_FILES "")
if ("x$ENV{LIMA_DISABLE_CPACK_DEBIAN_PACKAGE_SHLIBDEPS}" STREQUAL "x")
  set(CPACK_DEBIAN_runtime_PACKAGE_SHLIBDEPS ON)
endif()
set(CPACK_GENERATOR ${SPECIFIC_SYSTEM_PREFERED_CPACK_GENERATOR})

INCLUDE(CPack)


