# - Find the native QHttpServer includes and library

# created from FindPNG.cmake (Copyright 2002-2009 Kitware, Inc.)
# by Gael de Chalendar (CEA LIST)

# - Find the native QHttpServer includes and library
#
# This module defines
#  QHTTPSERVER_INCLUDE_DIR, where to find qhttpserver.h, etc.
#  QHTTPSERVER_LIBRARY_LIB, the libraries to link against to use QHttpServer.
#  QHTTPSERVER_FOUND, If false, do not try to use QHttpServer.
# also defined, but not for general use are

#=============================================================================
# Copyright 2002-2009 Kitware, Inc.
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distributed this file outside of CMake, substitute the full
#  License text for the above reference.)

find_path(QHTTPSERVER_INCLUDE_DIR qhttpserver.h
  PATHS $ENV{LIMA_EXTERNALS}/include
)

list(APPEND QHTTPSERVER_NAMES qhttpserver libqhttpserver)
find_library(QHTTPSERVER_LIBRARY_LIB NAMES ${QHTTPSERVER_NAMES}
  PATHS $ENV{LIMA_EXTERNALS}/lib 
)

# handle the QUIETLY and REQUIRED arguments and set QHTTPSERVER_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(QHTTPSERVER QHTTPSERVER_INCLUDE_DIR QHTTPSERVER_LIBRARY_LIB)

mark_as_advanced(QHTTPSERVER_INCLUDE_DIR QHTTPSERVER_LIBRARY_LIB)
