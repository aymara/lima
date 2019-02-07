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
project(limalinguisticprocessing)
cmake_minimum_required(VERSION 3.4.0)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#cmake_policy
if(POLICY CMP0020)
cmake_policy(SET CMP0020 OLD)
endif()

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/execEnv/config/plugins)
file(WRITE ${CMAKE_BINARY_DIR}/execEnv/config/plugins/${CMAKE_PROJECT_NAME} "")

macro (DECLARE_LIMA_PLUGIN _plugin)
  file (APPEND ${CMAKE_BINARY_DIR}/execEnv/config/plugins/${CMAKE_PROJECT_NAME} "${_plugin}\n")
  add_library(${_plugin} SHARED ${${_plugin}_LIB_SRCS} ${${_plugin}_MOC_OUTFILES})
  ADD_DLL_TO_PATH(${_plugin})
endmacro ()

# Use these cmake parameters (adapt to your setup) to compile under Windows
# -DCMAKE_INSTALL_PREFIX="e:\mose" -DCMAKE_BUILD_TYPE=Release
# -DCMAKE_INSTALL_PREFIX="c:\mose-debug" -DCMAKE_BUILD_TYPE=RelWithDebInfo

enable_testing()

# include(${CMAKE_SOURCE_DIR}/../SystemSpecificInformations.cmake)
message("System name is ${CMAKE_SYSTEM_NAME}")

SET(optionalLibs)

addQt5Modules(Core Xml XmlPatterns Test)

# Search Qt5 optional components
find_package(Qt5 COMPONENTS DBus)
if (Qt5DBus_FOUND)
  addQt5Modules(DBus)
endif()

get_target_property(QtXmlPatterns_location Qt5::XmlPatterns LOCATION)
if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  install(FILES ${QtXmlPatterns_location}
          DESTINATION ${LIB_INSTALL_DIR}
          COMPONENT runtime)
  set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS
    ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
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

if (DEFINED ENV{PYTHON_VERSION})
  set(Python_ADDITIONAL_VERSIONS $ENV{PYTHON_VERSION})
else ()
  set(Python_ADDITIONAL_VERSIONS 3.5 3.6 3.7)
endif ()

find_package(PythonLibs 3)
if (${PYTHONLIBS_FOUND})
  include_directories(${PYTHON_INCLUDE_DIRS})
  if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    set(PYTHON_DLLS "")
    foreach(PYTHON_LIB ${PYTHON_LIBRARIES})
      if (NOT EXISTS ${PYTHON_LIB})
        continue()
      endif()
      get_filename_component(PYTHON_LIB_FILE_NAME, ${PYTHON_LIB}, NAME)
      string(LENGTH "${PYTHON_LIB_FILE_NAME}" PYTHON_LIB_FILE_NAME_LEN)
      if ("${PYTHON_LIB_FILE_NAME_LEN}" LESS 6)
        continue()
      endif()
      message("Python lib name: ${PYTHON_LIB_FILE_NAME}")
      string(SUBSTRING ${PYTHON_LIB_FILE_NAME} 0, 6, FIRST_SIX_CHARS_OF_LIB_NAME)
#      message("First six chars: ${FIRST_SIX_CHARS_OF_LIB_NAME}")
      if (NOT ${FIRST_SIX_CHARS_OF_LIB_NAME} STREQUAL "python")
         continue()
      endif()
      string(REPLACE ".lib" ".dll" PYTHON_DLL ${PYTHON_LIB})
      string(REPLACE "libs" "" PYTHON_DLL ${PYTHON_DLL})
      message("Python dll: ${PYTHON_DLL} / Python lib: ${PYTHON_LIB}")
      if(EXISTS ${PYTHON_DLL})
        string(REPLACE "\\" "\\\\" ESCAPED_PATH_TO_PYTHON_DLL ${PYTHON_DLL})
        list(APPEND PYTHON_DLLS ${ESCAPED_PATH_TO_PYTHON_DLL})
        break()
      else()
        message(FATAL_ERROR "Could not find Python ( ${PYTHON_DLL} / ${PYTHON_LIB} )")
      endif()
    endforeach()
    message("PYTHON_DLLS = ${PYTHON_DLLS} / PYTHON_LIBRARIES = ${PYTHON_LIBRARIES}")

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
    string(REPLACE ".lib" ".dll" QHTTPSERVERCPP_LIBRARY_DLL ${QHTTPSERVERCPP_LIBRARY})
    install(FILES ${QHTTPSERVERCPP_LIBRARY} ${QHTTPSERVERCPP_LIBRARY_DLL}
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

  include_directories(SYSTEM ${TensorFlow_INCLUDE_DIRS} ${Eigen_INCLUDE_DIRS} ${Protobuf_INCLUDE_DIRS})
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

set(LIMA_LP_LIB_VERSION ${LIMA_GENERIC_LIB_VERSION})
set(LIMA_LP_LIB_SOVERSION ${LIMA_GENERIC_LIB_SOVERSION})

configure_file(LIMALPConfig-src.cmake LIMALPConfig.cmake @ONLY)

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(tools)
add_subdirectory(conf)
add_subdirectory(data)


########### install files ###############
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/LIMALPConfig.cmake
  DESTINATION share/apps/lima/cmake/Modules
  COMPONENT devel)
install(FILES ${CMAKE_BINARY_DIR}/execEnv/config/plugins/${CMAKE_PROJECT_NAME}
  DESTINATION share/config/lima/plugins
  COMPONENT runtime)

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
#
