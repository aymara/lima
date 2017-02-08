#   Copyright 2002-2017 CEA LIST
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
#
# Try to find log4cpp. Once done this will define:
#  LOG4CPP_FOUND - System has log4cpp
#  LOG4CPP_INCLUDE_DIRS - The log4cpp include directories
#  LOG4CPP_LIBRARIES - The libraries needed to use log4cpp

find_path(LOG4CPP_INCLUDE_DIR log4cpp/FileAppender.hh
          PATHS $ENV{LIMA_EXTERNALS}/include)

find_library(LOG4CPP_LIBRARY NAMES log4cpp
             PATHS $ENV{LIMA_EXTERNALS}/lib)

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LOG4CPP_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(log4cpp DEFAULT_MSG LOG4CPP_LIBRARY LOG4CPP_INCLUDE_DIR)

mark_as_advanced(LOG4CPP_INCLUDE_DIR LOG4CPP_LIBRARY)

set(LOG4CPP_LIBRARIES ${LOG4CPP_LIBRARY})
set(LOG4CPP_INCLUDE_DIRS ${LOG4CPP_INCLUDE_DIR})
