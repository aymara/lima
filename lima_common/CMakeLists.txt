#   Copyright 2002-2013 CEA LIST
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
project(limacommon)
cmake_minimum_required(VERSION 3.4.0)

#cmake_policy
if(POLICY CMP0020)
cmake_policy(SET CMP0020 OLD)
endif()

# Use these cmake parameters (adapt to your setup) to compile under Windows
# -DCMAKE_INSTALL_PREFIX="c:/amose" -DCMAKE_BUILD_TYPE=Release
# -DCMAKE_INSTALL_PREFIX="c:/amose-debug" -DCMAKE_BUILD_TYPE=RelWithDebInfo

# In debug mode, adds the -DDEBUG_CD to the compiler command line.
# Allows to build with conditionally built debug output
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG_CD -DDEBUG_FACTORIES" )
set(CMAKE_USE_RELATIVE_PATHS OFF)
enable_testing()

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
addQt5Modules(Core Xml Test)
include_directories(${Qt5_INCLUDES})
message("\n\n\n{Qt5_LIBRARIES}=${Qt5_LIBRARIES} ")
get_target_property(QtCore_location Qt5::Core LOCATION)
get_target_property(QtXml_location Qt5::Xml LOCATION)
get_target_property(QtTest_location Qt5::Test LOCATION)
set(QT_LIBRARIES "${Qt5_LIBRARIES}")

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  install(FILES ${QtCore_location}
                ${QtXml_location}
                ${QtTest_location}
          DESTINATION ${LIB_INSTALL_DIR})
endif ()

add_definitions( -DBOOST_ALL_NO_LIB )
add_definitions( -DBOOST_ALL_DYN_LINK )
set( Boost_USE_STATIC_LIBS OFF )
set( Boost_USE_MULTITHREADED ON )
set( Boost_USE_STATIC_RUNTIME OFF )
set( BOOST_LIB_DIAGNOSTIC )
find_package(Boost 1.54 REQUIRED COMPONENTS unit_test_framework system)
message("Boost_INCLUDE_DIR= ${Boost_INCLUDE_DIR}")
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  set (Boost_SYSTEM_LIBRARY ${Boost_SYSTEM_LIBRARY_RELEASE})
  string(REPLACE ".lib" ".dll" Boost_SYSTEM_LIBRARY_dll ${Boost_SYSTEM_LIBRARY_RELEASE})
  string(REPLACE ".lib" ".dll" Boost_UNIT_TEST_FRAMEWORK_LIBRARY_dll ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY_RELEASE})
  install(FILES ${Boost_SYSTEM_LIBRARY_dll}
                ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY_dll}
          DESTINATION ${LIB_INSTALL_DIR})
endif ()

configure_file(LIMACOMMONConfig-src.cmake LIMACOMMONConfig.cmake @ONLY)
find_package(LIMACOMMON PATHS ${CMAKE_CURRENT_BINARY_DIR})
if(LIMACOMMON_FOUND)
  include_directories( ${LIMACOMMON_INCLUDE_DIR})
  link_directories( ${LIMACOMMON_LIBRARY_DIRS})
  set(LIMA_COMMON_LIB_VERSION ${LIMA_GENERIC_LIB_VERSION})
  set(LIMA_COMMON_LIB_SOVERSION ${LIMA_GENERIC_LIB_SOVERSION})
else ()
  message(FATAL_ERROR "Could not find lima_common")
endif ()

add_subdirectory(conf)
add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(tools)


########### install files ###############
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/LIMACOMMONConfig.cmake DESTINATION share/apps/lima/cmake/Modules COMPONENT devel)

########### documentation ###############

# add a target to generate API documentation with Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
add_custom_target(doc
${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
COMMENT "Generating API documentation with Doxygen" VERBATIM
)
endif(DOXYGEN_FOUND)

########### packaging ###############
set(CMAKE_INSTALL_SYSTEM_RUNTIME_COMPONENT "runtime")
include(InstallRequiredSystemLibraries)

# set(CPACK_MONOLITHIC_INSTALL 1)
set(CPACK_PACKAGE_NAME "limacommon")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "LIMA Common libraries")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_SYSTEM_NAME}")

# set of commands to define component based packaging
# notify cpack of component all names
set(CMAKE_INSTALL_DEFAULT_COMPONENT_NAME "runtime")
SET(CPACK_COMPONENTS_ALL devel runtime)
set(CPACK_RPM_COMPONENT_INSTALL ON)
set(CPACK_DEB_COMPONENT_INSTALL ON)

SET(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
SET(CPACK_NSIS_MODIFY_PATH "ON")
SET(CPACK_STRIP_FILES "bin/testDict16;bin/testLexicon")
SET(CPACK_SOURCE_STRIP_FILES "")
if ("x$ENV{LIMA_DISABLE_CPACK_DEBIAN_PACKAGE_SHLIBDEPS}" STREQUAL "x")
  set(CPACK_DEBIAN_runtime_PACKAGE_SHLIBDEPS ON)
endif()
set(CPACK_GENERATOR ${SPECIFIC_SYSTEM_PREFERED_CPACK_GENERATOR})

list(APPEND CPACK_NSIS_EXTRA_INSTALL_COMMANDS "WriteRegExpandStr \\\${env_hklm} LIMA_DIST $INSTDIR")
list(APPEND CPACK_NSIS_EXTRA_INSTALL_COMMANDS "WriteRegExpandStr \\\${env_hklm} LIMA_CONF $INSTDIR\\\\share\\\\config\\\\lima")
list(APPEND CPACK_NSIS_EXTRA_INSTALL_COMMANDS "WriteRegExpandStr \\\${env_hklm} LIMA_RESOURCES $INSTDIR\\\\share\\\\apps\\\\lima\\\\resources")
list(APPEND CPACK_NSIS_EXTRA_INSTALL_COMMANDS "SendMessage \\\${HWND_BROADCAST} \\\${WM_WININICHANGE} 0 \\\"STR:Environment\\\" /TIMEOUT=5000")
string (REPLACE ";" "\n" CPACK_NSIS_EXTRA_INSTALL_COMMANDS "${CPACK_NSIS_EXTRA_INSTALL_COMMANDS}")

list(APPEND CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "DeleteRegValue \\\${env_hklm} LIMA_DIST")
list(APPEND CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "DeleteRegValue \\\${env_hklm} LIMA_CONF")
list(APPEND CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "DeleteRegValue \\\${env_hklm} LIMA_RESOURCES")
list(APPEND CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "SendMessage \\\${HWND_BROADCAST} \\\${WM_WININICHANGE} 0 \\\"STR:Environment\\\" /TIMEOUT=5000")
string (REPLACE ";" "\n" CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "${CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS}")

INCLUDE(CPack)

