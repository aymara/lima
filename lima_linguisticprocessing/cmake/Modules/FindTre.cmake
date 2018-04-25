# - Find the native QHttpServer includes and library

# created from FindPNG.cmake (Copyright 2002-2009 Kitware, Inc.)
# by Gael de Chalendar (CEA LIST)

# - Find the native QHttpServer includes and library
#
# This module defines
#  TREECPP_INCLUDE_DIR, where to find soci.h, etc.
#  TRECPP_LIBRARY_DIRS, the libraries to link against to use QHttpServer.
#  QHTTPSERVER_DEFINITIONS - You should add_definitons(${QHTTPSERVER_DEFINITIONS}) before compiling code that includes png library files.
#  QHTTPSERVER_FOUND, If false, do not try to use QHttpServer.
# also defined, but not for general use are
#  TRECPP_LIBRARY_DIRS, where to find the QHttpServer library.

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


find_path(TREECPP_INCLUDE_DIR tre/tre.h
  PATHS $ENV{LIMA_EXTERNALS}/include
)

set(TRECPP_NAMES ${TRECPP_NAMES} tre)
find_library(TRECPP_LIBRARIES NAMES ${TRECPP_NAMES}
  PATHS $ENV{LIMA_EXTERNALS}/lib 
)

# handle the QUIETLY and REQUIRED arguments and set QHTTPSERVER_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Tre  DEFAULT_MSG  TRECPP_LIBRARIES TREECPP_INCLUDE_DIR)

mark_as_advanced(TREECPP_INCLUDE_DIR TRECPP_LIBRARIES)

