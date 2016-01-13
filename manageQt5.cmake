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
   
# It is necessary to define Qt5_INSTALL_DIR in your environment.
set(CMAKE_PREFIX_PATH  
  "$ENV{Qt5_INSTALL_DIR}"
  "${CMAKE_PREFIX_PATH}" 
)

# Add definitions and flags
add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0)
if (NOT (${CMAKE_SYSTEM_NAME} STREQUAL "Windows"))
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -lpthread")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DQT_COMPILING_QSTRING_COMPAT_CPP -D_SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS")
endif()


# This macro does the find_package with the required Qt5 modules listed as parameters
# and sets Qt5_INCLUDES and Qt5_LIBRARIES variables.
# Those variables can then be used with include_directories and target_link_libraries commands.
macro(addQt5Modules )
  set(_MODULES ${ARGV})
  #message("MODULES:${_MODULES}")
  if(NOT "${_MODULES}" STREQUAL "" )
    #Use find_package to get includes and libraries directories
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
    #set(CMAKE_AUTOMOC ON)
    find_package(Qt5 REQUIRED ${_MODULES})

    #Add Qt5 include and libraries paths to the sets
    foreach( _module ${_MODULES})
      message("Adding module ${_module}")
      set(Qt5_INCLUDES ${Qt5_INCLUDES} ${Qt5${_module}_INCLUDE_DIRS} )
      set(Qt5_LIBRARIES ${Qt5_LIBRARIES} ${Qt5${_module}_LIBRARIES} )
      #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5${_module}_EXECUTABLE_COMPILE_FLAGS}")
    endforeach()
  endif()
endmacro()
