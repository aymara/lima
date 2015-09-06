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
project(limalinguisticprocessing)
cmake_minimum_required(VERSION 2.8.3)

#cmake_policy
if(POLICY CMP0017)
cmake_policy(SET CMP0017 NEW)

endif()

if(POLICY CMP0020)
cmake_policy(SET CMP0020 OLD)
endif()

if(POLICY CMP0038)
cmake_policy(SET CMP0038 NEW)
endif()


# In debug mode, adds the -DDEBUG_LP to the compiler command line.
# Allows to build with conditionally built debug output
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG_LP -DDEBUG_CD" )

file(WRITE ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}-plugins "")
macro (DECLARE_LIMA_PLUGIN _plugin)
  file (APPEND ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}-plugins "${_plugin}\n")
  add_library(${_plugin} SHARED ${${_plugin}_LIB_SRCS})
endmacro (DECLARE_LIMA_PLUGIN)

# Use these cmake parameters (adapt to your setup) to compile under Windows
# -DCMAKE_INSTALL_PREFIX="e:mose" -DCMAKE_BUILD_TYPE=Release
# -DCMAKE_INSTALL_PREFIX="c:mose-debug" -DCMAKE_BUILD_TYPE=RelWithDebInfo

enable_testing()

include(SystemSpecificInformations.cmake)

message("System name is ${CMAKE_SYSTEM_NAME}")
if (NOT (${CMAKE_SYSTEM_NAME} STREQUAL "Windows"))
    message("Linux flags")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fopenmp" )

    # Flags needed for the LIMA plugins mechanism to work: our libs are dynamically loaded by
    # factories, thus their symbols must be known even if not used by the binary
    set(CMAKE_EXE_LINKER_FLAGS "-Wl,--no-as-needed")
    set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-as-needed")
    set(CMAKE_MODULE_LINKER_FLAGS "-Wl,--no-as-needed")

    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG(-std=c++0x HAVE_STDCPP0X)
    CHECK_CXX_COMPILER_FLAG(-std=c++11 HAVE_STDCPP11)

    if (HAVE_STDCPP11)
      message("C++11 supported")
      set(CMAKE_CXX_FLAGS "-std=c++11 -DBOOST_NO_HASH")
    elseif (HAVE_STDCPP0X)
      message("C++0x supported")
      set(CMAKE_CXX_FLAGS "-std=c++0x -DBOOST_NO_HASH")
    else ()
      message("Neither C++11 nor C++0x supported")
      set(CMAKE_CXX_FLAGS "-DNO_STDCPP0X")
    endif ()
    set(CMAKE_CXX_FLAGS "-W -Wall ${CMAKE_CXX_FLAGS}")
else (NOT (${CMAKE_SYSTEM_NAME} STREQUAL "Windows"))
  message("Windows flags")
  add_definitions(-D WIN32)

  # By default, do not warn when built on machines using only VS Express:
  IF(NOT DEFINED CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_NO_WARNINGS)
    SET(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_NO_WARNINGS ON)
  ENDIF(NOT DEFINED CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_NO_WARNINGS)

  set(CMAKE_C_FLAGS "/Zc:wchar_t- /EHsc /GR ${CMAKE_C_FLAGS}")
  set(CMAKE_CXX_FLAGS "/Zc:wchar_t- /EHsc /GR /W4 /MP /FAu ${CMAKE_CXX_FLAGS}")

endif (NOT (${CMAKE_SYSTEM_NAME} STREQUAL "Windows"))



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
addQt5Modules(Core Xml XmlPatterns Network DBus Test)
include_directories(${Qt5_INCLUDES})
set(QT_LIBRARIES "${Qt5_LIBRARIES}")
message(STATUS "Qt5_INCLUDES=${Qt5_INCLUDES}")
if ("${QT_DBUSCPP2XML_EXECUTABLE}" STREQUAL "QT_DBUSCPP2XML_EXECUTABLE-NOTFOUND")
message(WARNING "Dbus tools not found ${QT_DBUSCPP2XML_EXECUTABLE}")
endif()

#add_definitions( -DBOOST_ALL_NO_LIB )
add_definitions( -DBOOST_ALL_DYN_LINK )
set( Boost_USE_STATIC_LIBS OFF )
set( Boost_USE_STATIC_RUNTIME OFF )
set( BOOST_LIB_DIAGNOSTIC )
FIND_PACKAGE(Boost 1.46 REQUIRED COMPONENTS regex filesystem program_options system thread unit_test_framework)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

find_package(SVMTOOL++) 
if(SVMTOOL++_FOUND)
  set(optionalLibs ${optionalLibs} ${SVMTOOL++_NAMES})
  include_directories(${SVMTOOL++CPP_INCLUDE_DIR})
  link_directories(${SVMTOOL++CPP_LIBRARY_DIRS})
  message(STATUS "Found SVMTool++ ${SVMTOOL++CPP_INCLUDE_DIR}.")
else ()
  message(STATUS "Could not find SVMTool++. SVM-base PoS tagger will not be available.")
endif()

find_package (Enchant) 
if (ENCHANT_FOUND)
  set(optionalLibs ${optionalLibs} ${ENCHANT_LIBRARIES})
  include_directories(${ENCHANT_INCLUDE_DIRS})
else (ENCHANT_FOUND)
  message(STATUS "Could not find Enchant ${ENCHANT_INCLUDE_DIRS}. Spelling correction will not be built.")
endif (ENCHANT_FOUND)

# QHttpServer is necessary for limaserver HTTP server
find_package(QHttpServer QUIET)
if (NOT QHTTPSERVER_FOUND)
  message(STATUS "QHttpServer Not found. Lima HTTP server will NOT be built")
else(NOT QHTTPSERVER_FOUND)
  message(STATUS "Found QHttpServer. lima HTTP server will be built")
  include_directories($ENV{LIMA_EXTERNALS}/include)
  link_directories($ENV{LIMA_EXTERNALS}/lib)
endif (NOT QHTTPSERVER_FOUND)


FIND_PACKAGE(LIMACOMMON PATHS ${CMAKE_INSTALL_PREFIX}/share/apps/lima/cmake/Modules /usr/share/apps/lima/cmake/Modules)
IF(LIMACOMMON_FOUND)
  include_directories( ${LIMACOMMON_INCLUDE_DIR})
  link_directories( ${LIMACOMMON_LIBRARY_DIRS})
  set(LIMA_LP_LIB_VERSION ${LIMA_GENERIC_LIB_VERSION})
  set(LIMA_LP_LIB_SOVERSION ${LIMA_GENERIC_LIB_SOVERSION})
ELSE (LIMACOMMON_FOUND)
  MESSAGE(FATAL_ERROR "Could not find lima_common")
ENDIF(LIMACOMMON_FOUND)

configure_file(LIMALPConfig-src.cmake LIMALPConfig.cmake @ONLY)
find_package(LIMALP PATHS ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(tools)
add_subdirectory(conf)
add_subdirectory(data)
add_subdirectory(cmake)


########### install files ###############
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/LIMALPConfig.cmake DESTINATION share/apps/lima/cmake/Modules)
install(FILES ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}-plugins DESTINATION share/config/lima/plugins)

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
INCLUDE(InstallRequiredSystemLibraries)

set(CPACK_MONOLITHIC_INSTALL 1)
SET(CPACK_PACKAGE_NAME "limalinguisticprocessing")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "LIMA linguistic processing libraries and programs")
SET(CPACK_PACKAGE_VENDOR "CEA LIST")
SET(CPACK_PACKAGE_CONTACT "Gael de Chalendar <Gael.de-Chalendar@cea.fr>")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
SET(CPACK_PACKAGE_VERSION_MAJOR "${LIMA_VERSION_MAJOR}")
SET(CPACK_PACKAGE_VERSION_MINOR "${LIMA_VERSION_MINOR}")
SET(CPACK_PACKAGE_VERSION_PATCH "${LIMA_VERSION_RELEASE}")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "CMake ${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}")
SET(CPACK_STRIP_FILES "bin/analyzeText;bin/readBowFile")
SET(CPACK_SOURCE_STRIP_FILES "")
if ("x$ENV{LIMA_DISABLE_CPACK_DEBIAN_PACKAGE_SHLIBDEPS}" STREQUAL "x")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
endif()
set(CPACK_GENERATOR ${SPECIFIC_SYSTEM_PREFERED_CPACK_GENERATOR})
set(CPACK_RPM_PACKAGE_REQUIRES "limacommon")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "limacommon")

INCLUDE(CPack)
