# - Find the native soci includes and library

# created from FindPNG.cmake (Copyright 2002-2009 Kitware, Inc.)
# by Gael de Chalendar (CEA LIST)

# - Find the native SOCI includes and library
#
# This module defines
#  SOCICPP_INCLUDE_DIR, where to find soci.h, etc.
#  SOCICPP_LIBRARY_DIRS, the libraries to link against to use SOCI.
#  SOCI_DEFINITIONS - You should add_definitons(${SOCI_DEFINITIONS}) before compiling code that includes png library files.
#  SOCI_FOUND, If false, do not try to use SOCI.
# also defined, but not for general use are
#  SOCICPP_LIBRARY_DIRS, where to find the SOCI library.
# None of the above will be defined unles zlib can be found.
# SOCI depends on Zlib

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


find_path(SOCICPP_INCLUDE_DIR soci/soci.h
  PATHS $ENV{MM_EXTERNALS}/include NO_DEFAULT_PATH
)

set(SOCI_NAMES ${SOCI_NAMES} soci_core-3)
find_library(SOCICPP_LIBRARY_DIRS NAMES ${SOCI_NAMES}
  PATHS $ENV{MM_EXTERNALS}/lib NO_DEFAULT_PATH
)

if (SOCICPP_LIBRARY_DIRS AND SOCICPP_INCLUDE_DIR)
    if (CYGWIN)
      if(BUILD_SHARED_LIBS)
          # No need to define SOCI_USE_DLL here, because it's default for Cygwin.
      else(BUILD_SHARED_LIBS)
        SET (SOCI_DEFINITIONS -DSOCI_STATIC)
      endif(BUILD_SHARED_LIBS)
    endif (CYGWIN)

endif (SOCICPP_LIBRARY_DIRS AND SOCICPP_INCLUDE_DIR)

# handle the QUIETLY and REQUIRED arguments and set SOCI_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SOCI  DEFAULT_MSG  SOCICPP_LIBRARY_DIRS SOCICPP_INCLUDE_DIR)

mark_as_advanced(SOCICPP_INCLUDE_DIR SOCICPP_LIBRARY_DIRS )


# SET(SOCICPP_INCLUDE_DIR $ENV{MM_EXTERNALS}/include)
# SET(SOCICPP_LIBRARY_DIRS $ENV{MM_EXTERNALS}/lib )

