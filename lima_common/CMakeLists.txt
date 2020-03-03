#   Copyright 2002-2019 CEA LIST
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

enable_testing()

# include(${CMAKE_SOURCE_DIR}/../SystemSpecificInformations.cmake)
message("System name is ${CMAKE_SYSTEM_NAME}")

# include(${CMAKE_SOURCE_DIR}/../SetCompilerFlags.cmake)

# Find*.cmake will be searched in the following dirs if not found in system dirs
set(CMAKE_MODULE_PATH
    "${CMAKE_SOURCE_DIR}/cmake/Modules" # for those available in this project
    "$ENV{LIMA_EXTERNALS}/share/apps/cmake/Modules" # for those available in the externals dir
    "$ENV{LIMA_EXTERNALS}/share/cmake/Modules" # for those available in the externals dir
)

# find_path and find_library will search in these dirs too
set(CMAKE_PREFIX_PATH
    "${CMAKE_PREFIX_PATH}"
    "$ENV{LIMA_EXTERNALS}"
)

add_definitions(-D QT_NO_KEYWORDS)
include(${CMAKE_SOURCE_DIR}/manageQt5.cmake)
addQt5Modules(Core Xml Test)
# include_directories(${Qt5_INCLUDES})
message("\n\n\n{Qt5_LIBRARIES}=${Qt5_LIBRARIES} ")
get_target_property(QtCore_location Qt5::Core LOCATION)
get_target_property(QtXml_location Qt5::Xml LOCATION)
get_target_property(QtTest_location Qt5::Test LOCATION)
# set(Qt5_LIBRARIES "${Qt5_LIBRARIES}")

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  install(FILES ${QtCore_location}
                ${QtXml_location}
                ${QtTest_location}
          DESTINATION ${LIB_INSTALL_DIR}
          COMPONENT runtime)
endif ()

add_definitions( -DBoost_DEBUG=ON )
add_definitions( -DBOOST_ALL_NO_LIB )
add_definitions( -DBOOST_ALL_DYN_LINK )
set( Boost_USE_STATIC_LIBS OFF )
set( Boost_USE_MULTITHREADED ON )
set( Boost_USE_STATIC_RUNTIME OFF )
set( BOOST_LIB_DIAGNOSTIC )
find_package(Boost 1.54 REQUIRED
  COMPONENTS unit_test_framework system filesystem)
message("Boost_INCLUDE_DIR= ${Boost_INCLUDE_DIR}")
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set (Boost_SYSTEM_LIBRARY ${Boost_SYSTEM_LIBRARY_DEBUG})
    string(REPLACE ".lib" ".dll" Boost_SYSTEM_LIBRARY_dll ${Boost_SYSTEM_LIBRARY_DEBUG})
    string(REPLACE ".lib" ".dll" Boost_UNIT_TEST_FRAMEWORK_LIBRARY_dll ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY_DEBUG})
  else()
    set (Boost_SYSTEM_LIBRARY ${Boost_SYSTEM_LIBRARY_RELEASE})
    string(REPLACE ".lib" ".dll" Boost_SYSTEM_LIBRARY_dll ${Boost_SYSTEM_LIBRARY_RELEASE})
    string(REPLACE ".lib" ".dll" Boost_UNIT_TEST_FRAMEWORK_LIBRARY_dll ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY_RELEASE})
  endif()
  install(FILES ${Boost_SYSTEM_LIBRARY_dll}
                ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY_dll}
          DESTINATION ${LIB_INSTALL_DIR}
          COMPONENT runtime)
endif ()

configure_file(LIMACOMMONConfig-src.cmake LIMACOMMONConfig.cmake @ONLY)

set(LIMA_COMMON_LIB_VERSION ${LIMA_GENERIC_LIB_VERSION})
set(LIMA_COMMON_LIB_SOVERSION ${LIMA_GENERIC_LIB_SOVERSION})

add_subdirectory(conf)
add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(tools)

########### install files ###############
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/LIMACOMMONConfig.cmake
  DESTINATION share/apps/lima/cmake/Modules
  COMPONENT devel)

# ########### documentation ###############
#
# # add a target to generate API documentation with Doxygen
# find_package(Doxygen)
# if(DOXYGEN_FOUND)
# configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
# add_custom_target(doc
# ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
# WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
# COMMENT "Generating API documentation with Doxygen" VERBATIM
# )
# endif(DOXYGEN_FOUND)

