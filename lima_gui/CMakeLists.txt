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
cmake_minimum_required(VERSION 3.4.0)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_testing()

include(GNUInstallDirs)
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
# include(${CMAKE_SOURCE_DIR}/../manageQt5.cmake)
addQt5Modules(Core Test Qml Quick QuickTest Gui Widgets)
include_directories(${Qt5_INCLUDES})

get_target_property(QtQml_location Qt5::Qml LOCATION)
get_target_property(QtQuick_location Qt5::Quick LOCATION)
get_target_property(QtQuickTest_location Qt5::QuickTest LOCATION)
get_target_property(QtGui_location Qt5::Gui LOCATION)
get_target_property(QtWidgets_location Qt5::Widgets LOCATION)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  install(FILES ${QtQml_location}
                ${QtQuick_location}
                ${QtQuickTest_location}
                ${QtGui_location}
                ${QtWidgets_location}
          DESTINATION ${LIB_INSTALL_DIR}
          COMPONENT runtime)
  set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS
    ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
    ${QtQml_location}
    ${QtQuick_location}
    ${QtQuickTest_location}
    ${QtGui_location}
    ${QtWidgets_location}
)
endif ()

add_definitions( -DBOOST_ALL_NO_LIB )
add_definitions( -DBOOST_ALL_DYN_LINK )
set( Boost_USE_STATIC_LIBS OFF )
set( Boost_USE_MULTITHREADED ON )
set( Boost_USE_STATIC_RUNTIME OFF )
set( BOOST_LIB_DIAGNOSTIC )
FIND_PACKAGE(Boost 1.54 REQUIRED COMPONENTS regex filesystem program_options system thread unit_test_framework)
message("Boost_INCLUDE_DIR= ${Boost_INCLUDE_DIR}")
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

add_subdirectory(src)
add_subdirectory(test)

add_subdirectory(conf)

