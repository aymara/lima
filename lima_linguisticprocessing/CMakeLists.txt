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
cmake_minimum_required(VERSION 3.4.0)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#cmake_policy
if(POLICY CMP0020)
cmake_policy(SET CMP0020 OLD)
endif()

file(WRITE ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}-plugins "")
macro (DECLARE_LIMA_PLUGIN _plugin)
  file (APPEND ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}-plugins "${_plugin}\n")
  add_library(${_plugin} SHARED ${${_plugin}_LIB_SRCS} ${${_plugin}_MOC_OUTFILES})
endmacro ()

# Use these cmake parameters (adapt to your setup) to compile under Windows
# -DCMAKE_INSTALL_PREFIX="e:\mose" -DCMAKE_BUILD_TYPE=Release
# -DCMAKE_INSTALL_PREFIX="c:\mose-debug" -DCMAKE_BUILD_TYPE=RelWithDebInfo

enable_testing()

include(${CMAKE_SOURCE_DIR}/../SystemSpecificInformations.cmake)
message("System name is ${CMAKE_SYSTEM_NAME}")

include(${CMAKE_SOURCE_DIR}/../SetCompilerFlags.cmake)

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
#   set(Qt5_INCLUDES ${Qt5_INCLUDES} ${Qt5DBus_INCLUDE_DIRS})
  set(Qt5_LIBRARIES ${Qt5_LIBRARIES} ${Qt5DBus_LIBRARIES})
endif()
#
# include_directories(${Qt5_INCLUDES})
# set(Qt5_LIBRARIES "${Qt5_LIBRARIES}")
# message(STATUS "Qt5_INCLUDES=${Qt5_INCLUDES}")
get_target_property(QtNetwork_location Qt5::Network LOCATION)
get_target_property(QtXmlPatterns_location Qt5::XmlPatterns LOCATION)
if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  install(FILES ${QtNetwork_location}
                ${QtXmlPatterns_location}
          DESTINATION ${LIB_INSTALL_DIR}
          COMPONENT runtime)
  set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS
    ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
    ${QtNetwork_location}
    ${QtXmlPatterns_location})
endif ()
if ("${QT_DBUSCPP2XML_EXECUTABLE}" STREQUAL "QT_DBUSCPP2XML_EXECUTABLE-NOTFOUND")
  message(WARNING "Dbus tools not found ${QT_DBUSCPP2XML_EXECUTABLE}")
  get_target_property(QtDBus_location Qt5::DBus LOCATION)
  if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    install(FILES ${QtDBus_location}
            DESTINATION ${LIB_INSTALL_DIR}
          COMPONENT runtime)
    set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS
      ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
      ${QtDBus_location})
  endif ()
else ()
endif()

set(Python_ADDITIONAL_VERSIONS 3.4 3.5 3.6 3.7)
find_package(PythonLibs 3)
if (${PYTHONLIBS_FOUND})
  include_directories(${PYTHON_INCLUDE_DIRS})
  if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    set(PYTHON_DLLS "")
    foreach(PYTHON_LIB ${PYTHON_LIBRARIES})
      string(REPLACE ".lib" ".dll" PYTHON_DLL ${PYTHON_LIB})
      string(REGEX MATCH "python[0-9]+\.dll" PURE_PYTHON_DLL_NAME ${PYTHON_DLL})
      execute_process(COMMAND cmd "/C" "where ${PURE_PYTHON_DLL_NAME}" OUTPUT_VARIABLE PYTHON_DLL_LOCATIONS_STR)
      string(REPLACE "\n" ";" PYTHON_DLL_LOCATIONS ${PYTHON_DLL_LOCATIONS_STR})
      foreach(ONE_LOCATION ${PYTHON_DLL_LOCATIONS})
        string(REPLACE "\\" "\\\\" ESCAPED_PATH_TO_PYTHON_DLL ${ONE_LOCATION})
        list(APPEND PYTHON_DLLS ${ESCAPED_PATH_TO_PYTHON_DLL})
        break()
      endforeach()
    endforeach()

    install(FILES ${PYTHON_DLLS}
            DESTINATION ${LIB_INSTALL_DIR}
            COMPONENT runtime)
    set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS
        ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
        ${PYTHON_DLLS})

    install(FILES ${PYTHON_LIBRARIES}
            DESTINATION ${LIB_INSTALL_DIR}
            COMPONENT runtime)
    set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS
        ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
        ${PYTHON_LIBRARIES})
  endif ()
endif ()

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
          DESTINATION ${LIB_INSTALL_DIR}
          COMPONENT runtime)
  set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS
      ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
      ${Boost_REGEX_LIBRARY_dll}
      ${Boost_FILESYSTEM_LIBRARY_dll}
      ${Boost_PROGRAM_OPTIONS_LIBRARY_dll}
      ${Boost_THREAD_LIBRARY_dll})

endif ()

find_package(SVMTOOL++)
if(SVMTOOL++_FOUND)
  set(optionalLibs ${optionalLibs} ${SVMTOOL++_NAMES})
  include_directories(${SVMTOOL++CPP_INCLUDE_DIR})
  link_directories(${SVMTOOL++CPP_LIBRARY_DIRS})
  message(STATUS "Hi, Found SVMTool++ ${SVMTOOL++CPP_INCLUDE_DIR}.")
  if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    install(FILES ${SVMTOOL++_NAMES}
          DESTINATION ${LIB_INSTALL_DIR}
          COMPONENT runtime)
    set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS
      ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
      ${SVMTOOL++_NAMES})
  endif()
else ()
  message(STATUS "Hi, Could not find SVMTool++. SVM-base PoS tagger will not be available.")
endif()

find_package (Enchant)
if (ENCHANT_FOUND)
  set(optionalLibs ${optionalLibs} ${ENCHANT_LIBRARIES})
  include_directories(${ENCHANT_INCLUDE_DIRS})
  if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    install(FILES ${ENCHANT_LIBRARIES}
          DESTINATION ${LIB_INSTALL_DIR}
          COMPONENT runtime)
    set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS
      ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
      ${ENCHANT_LIBRARIES})
  endif ()

else ()
  message(STATUS "Could not find Enchant ${ENCHANT_INCLUDE_DIRS}. Spelling correction will not be built.")
endif ()

find_package (Tre)
if (TRE_FOUND)
  set(optionalLibs ${optionalLibs} ${TRECPP_LIBRARIES})
  include_directories(${TRECPP_INCLUDE_DIRS})
  message(STATUS "Found tre library. Module ApproxStringMatcher will be built")
  if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    install(FILES ${TRECPP_LIBRARIES}
          DESTINATION ${LIB_INSTALL_DIR}
          COMPONENT runtime)
    set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS
      ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
      ${TRECPP_LIBRARIES})
  endif ()
else (TRECPP_FOUND)
  message(STATUS "Could not find tre ${TRECPP_INCLUDE_DIRS}. ApproxStringMatcher module will not be built.")
endif (TRE_FOUND)

# QHttpServer is necessary for limaserver HTTP server
find_package(QHttpServer QUIET)
if (NOT QHTTPSERVER_FOUND)
  message(STATUS "QHttpServer Not found. Lima HTTP server will NOT be built")
else ()
  message(STATUS "Found QHttpServer. lima HTTP server will be built with ${QHTTPSERVERCPP_INCLUDE_DIR} and ${QHTTPSERVERCPP_LIBRARY}")
  include_directories(${QHTTPSERVERCPP_INCLUDE_DIR})
  if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    install(FILES ${QHTTPSERVERCPP_LIBRARY}
          DESTINATION ${LIB_INSTALL_DIR}
          COMPONENT runtime)
    set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS
      ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
      ${QHTTPSERVERCPP_LIBRARY})
  endif ()
endif ()

message(STATUS "TF_SOURCES_PATH = ${TF_SOURCES_PATH}")

find_package(TensorFlow)
find_package(Eigen)
find_package(Protobuf)

if (TensorFlow_FOUND)
  message(STATUS "Found TensorFlow. ${TensorFlow_INCLUDE_DIRS} ${TensorFlow_LIBRARIES} ")
else ()
  message(STATUS "TensorFlow not found.")
endif ()

if (Eigen_FOUND)
  message(STATUS "Found Eigen. ${Eigen_INCLUDE_DIRS} ${Eigen_LIBRARIES} ")
else ()
  message(STATUS "Eigen not found.")
endif ()

if (TensorFlow_FOUND AND Eigen_FOUND)
  message(STATUS "Found TensorFlow. ${TensorFlow_INCLUDE_DIRS} ${TensorFlow_LIBRARIES} ")

  include_directories(${TensorFlow_INCLUDE_DIRS} ${Eigen_INCLUDE_DIRS} ${Protobuf_INCLUDE_DIRS})
  if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    install(FILES ${TensorFlow_LIBRARIES}
      ${Eigen_LIBRARIES}
          DESTINATION ${LIB_INSTALL_DIR}
          COMPONENT runtime)
    set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS
      ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
      ${Eigen_LIBRARIES}
      ${TensorFlow_LIBRARIES})
  endif ()

else()
  message(STATUS "TensorFlow and/or Eigen modules NOT Found. TensorFlow based modules will no be built.")
endif()

find_package(LIMACOMMON PATHS ${CMAKE_INSTALL_PREFIX}/share/apps/lima/cmake/Modules /usr/share/apps/lima/cmake/Modules)
if(LIMACOMMON_FOUND)
  include_directories( ${LIMACOMMON_INCLUDE_DIR})
  link_directories( ${LIMACOMMON_LIBRARY_DIRS})
  set(LIMA_LP_LIB_VERSION ${LIMA_GENERIC_LIB_VERSION})
  set(LIMA_LP_LIB_SOVERSION ${LIMA_GENERIC_LIB_SOVERSION})
else ()
  message(FATAL_ERROR "Could not find lima_common")
endif ()

configure_file(LIMALPConfig-src.cmake LIMALPConfig.cmake @ONLY)
find_package(LIMALP PATHS ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(tools)
add_subdirectory(conf)
add_subdirectory(data)
add_subdirectory(cmake)


########### install files ###############
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/LIMALPConfig.cmake
  DESTINATION share/apps/lima/cmake/Modules
  COMPONENT devel)
install(FILES ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}-plugins
  DESTINATION share/config/lima/plugins
  COMPONENT runtime)

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
SET(CPACK_PACKAGE_NAME "limalinguisticprocessing")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "LIMA linguistic processing libraries and programs")
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
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "Lima-${LIMA_VERSION_MAJOR}.${LIMA_VERSION_MINOR}")
SET(CPACK_NSIS_MODIFY_PATH "ON")
SET(CPACK_STRIP_FILES "bin/analyzeText;bin/readBowFile")
SET(CPACK_SOURCE_STRIP_FILES "")
if ("x$ENV{LIMA_DISABLE_CPACK_DEBIAN_PACKAGE_SHLIBDEPS}" STREQUAL "x")
  set(CPACK_DEBIAN_runtime_PACKAGE_SHLIBDEPS ON)
endif()
set(CPACK_GENERATOR ${SPECIFIC_SYSTEM_PREFERED_CPACK_GENERATOR})
set(CPACK_RPM_PACKAGE_REQUIRES "limacommon-runtime")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "limacommon-runtime")

INCLUDE(CPack)
