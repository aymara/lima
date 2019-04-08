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
project(LimaLinguisticData)

cmake_minimum_required(VERSION 3.4.0)

if (ENVPATH)
  set(ENV{PATH} "${ENVPATH}:$ENV{PATH}")
endif()
if (ENVPERL5LIB)
  set(ENV{PERL5LIB} "${ENVPERL5LIB}:$ENV{PERL5LIB}")
endif()
message("lima_linguisticdata PERL5LIB=$ENV{PERL5LIB}")
message("lima_linguisticdata PATH=$ENV{PATH}")

# message("CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}")
# set(CMAKE_VERBOSE_MAKEFILE on)

#cmake_policy
# if(POLICY CMP0046)
# cmake_policy(SET CMP0046 OLD)
# endif()

enable_testing()

include(${CMAKE_SOURCE_DIR}/../SystemSpecificInformations.cmake)
message("System name is ${CMAKE_SYSTEM_NAME}")

include(${CMAKE_SOURCE_DIR}/../SetCompilerFlags.cmake)

include(Languages.cmake)
include(cmake/LinguisticData.cmake)
# include(${CMAKE_SOURCE_DIR}/../SystemSpecificInformations.cmake)
# for add_component macro (require 2.8.5)
include(CPackComponent)

<<<<<<< HEAD
=======
SET(optionalLibs)
message (STATUS "CMAKE_SYSTEM_PREFIX_PATH ${CMAKE_SYSTEM_PREFIX_PATH}")
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
    "${CMAKE_INSTALL_PREFIX}"
    "$ENV{LIMA_EXTERNALS}"
)

include(${CMAKE_SOURCE_DIR}/../manageQt5.cmake)
addQt5Modules(Core Xml XmlPatterns Network Test)

# Search Qt5 optional components
find_package(Qt5 COMPONENTS DBus)
if (Qt5DBus_FOUND)
  set(Qt5_INCLUDES ${Qt5_INCLUDES} ${Qt5DBus_INCLUDE_DIRS})
  set(Qt5_LIBRARIES ${Qt5_LIBRARIES} ${Qt5DBus_LIBRARIES})
endif()

include_directories(${Qt5_INCLUDES})
set(QT_LIBRARIES "${Qt5_LIBRARIES}")
message(STATUS "Qt5_INCLUDES=${Qt5_INCLUDES}")
get_target_property(QtNetwork_location Qt5::Network LOCATION)
get_target_property(QtXmlPatterns_location Qt5::XmlPatterns LOCATION)
if ("${QT_DBUSCPP2XML_EXECUTABLE}" STREQUAL "QT_DBUSCPP2XML_EXECUTABLE-NOTFOUND")
  message(WARNING "Dbus tools not found ${QT_DBUSCPP2XML_EXECUTABLE}")
  get_target_property(QtDBus_location Qt5::DBus LOCATION)
  if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    install(FILES ${QtDBus_location}
            DESTINATION ${LIB_INSTALL_DIR})
  endif ()
else ()
endif()

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  install(FILES ${QtNetwork_location}
                ${QtXmlPatterns_location}
          DESTINATION ${LIB_INSTALL_DIR})
endif ()

find_package(PythonLibs 3.4)
if (${PYTHONLIBS_FOUND})
  include_directories(${PYTHON_INCLUDE_DIRS})
  link_directories(${PYTHON_LIBRARIES})
endif()

#add_definitions( -DBOOST_ALL_NO_LIB )
add_definitions( -DBOOST_ALL_DYN_LINK )
set( Boost_USE_STATIC_LIBS OFF )
set( Boost_USE_STATIC_RUNTIME OFF )
set( BOOST_LIB_DIAGNOSTIC )
FIND_PACKAGE(Boost 1.54 REQUIRED COMPONENTS regex filesystem program_options system thread unit_test_framework)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set (Boost_SYSTEM_LIBRARY ${Boost_SYSTEM_LIBRARY_DEBUG})
    string(REPLACE ".lib" ".dll" Boost_REGEX_LIBRARY_dll ${Boost_REGEX_LIBRARY_DEBUG})
    string(REPLACE ".lib" ".dll" Boost_FILESYSTEM_LIBRARY_dll ${Boost_FILESYSTEM_LIBRARY_DEBUG})
    string(REPLACE ".lib" ".dll" Boost_PROGRAM_OPTIONS_LIBRARY_dll ${Boost_PROGRAM_OPTIONS_LIBRARY_DEBUG})
    string(REPLACE ".lib" ".dll" Boost_THREAD_LIBRARY_dll ${Boost_THREAD_LIBRARY_DEBUG})
  else()
    string(REPLACE ".lib" ".dll" Boost_REGEX_LIBRARY_dll ${Boost_REGEX_LIBRARY_RELEASE})
    string(REPLACE ".lib" ".dll" Boost_FILESYSTEM_LIBRARY_dll ${Boost_FILESYSTEM_LIBRARY_RELEASE})
    string(REPLACE ".lib" ".dll" Boost_PROGRAM_OPTIONS_LIBRARY_dll ${Boost_PROGRAM_OPTIONS_LIBRARY_RELEASE})
    string(REPLACE ".lib" ".dll" Boost_THREAD_LIBRARY_dll ${Boost_THREAD_LIBRARY_RELEASE})
  endif()
  install(FILES ${Boost_REGEX_LIBRARY_dll}
                ${Boost_FILESYSTEM_LIBRARY_dll}
                ${Boost_PROGRAM_OPTIONS_LIBRARY_dll}
                ${Boost_THREAD_LIBRARY_dll}
          DESTINATION ${LIB_INSTALL_DIR})
endif ()


FIND_PACKAGE(LIMACOMMON
  PATHS
    $ENV{LIMA_DIST}/share/apps/lima/cmake/Modules
    ${CMAKE_INSTALL_PREFIX}/share/apps/lima/cmake/Modules
    /usr/share/apps/lima/cmake/Modules share/apps/lima/cmake/Modules)
IF(LIMACOMMON_FOUND)
  include_directories(${LIMACOMMON_INCLUDE_DIR})
  link_directories(${LIMACOMMON_LIBRARY_DIRS})
ELSE (LIMACOMMON_FOUND)
  MESSAGE(FATAL_ERROR "Could not find limacommon")
ENDIF(LIMACOMMON_FOUND)

FIND_PACKAGE(LIMALP
  PATHS
    $ENV{LIMA_DIST}/share/apps/lima/cmake/Modules
    ${CMAKE_INSTALL_PREFIX}/share/apps/lima/cmake/Modules
    /usr/share/apps/lima/cmake/Modules share/apps/lima/cmake/Modules)
IF(LIMALP_FOUND)
  include_directories(${LIMALP_INCLUDE_DIR})
  link_directories(${LIMALP_LIBRARY_DIRS})
ELSE (LIMALP_FOUND)
  MESSAGE(FATAL_ERROR "Could not find limalp")
ENDIF(LIMALP_FOUND)

>>>>>>> Compile dico related unit tests in linguisticdata
configure_file(LIMALDATAConfig-src.cmake LIMALDATAConfig.cmake @ONLY)

LIMA_GENERIC_CONFIGENV(eng)
LIMA_GENERIC_CONFIGENV(eng.ud)
LIMA_GENERIC_CONFIGENV(fre)
LIMA_GENERIC_CONFIGENV(por)

add_custom_target(pretest-execEnv)

add_subdirectory(cmake)
add_subdirectory(scratch)
add_subdirectory(analysisDictionary)
add_subdirectory(disambiguisationMatrices)
add_subdirectory(rules-idiom)
add_subdirectory(SpecificEntities)
add_subdirectory(syntacticAnalysis)
add_subdirectory(SRLIntegration)

LIMA_PRETEST_CONFIGENV(eng)
LIMA_PRETEST_CONFIGENV(eng.ud)
LIMA_PRETEST_CONFIGENV(fre)
LIMA_PRETEST_CONFIGENV(por)

########### install files ###############
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/LIMALDATAConfig.cmake DESTINATION share/apps/lima/cmake/Modules COMPONENT devel)

