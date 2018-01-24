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
add_definitions(-DQT_NO_KEYWORDS)
add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0)
if (NOT (${CMAKE_SYSTEM_NAME} STREQUAL "Windows"))
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -DQT_DEPRECATED_WARNINGS")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DQT_COMPILING_QSTRING_COMPAT_CPP -D_SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS")
endif()


# This macro does the find_package with the required Qt5 modules listed as parameters
# and sets Qt5_INCLUDES and Qt5_LIBRARIES variables.
# Those variables can then be used with include_directories and target_link_libraries commands.
macro(addQt5Modules )
  set(_MODULES Core ${ARGV})
  #message("MODULES:${_MODULES}")
  if(NOT "${_MODULES}" STREQUAL "" )
    #Use find_package to get includes and libraries directories
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
    #set(CMAKE_AUTOMOC ON)
<<<<<<< HEAD
    find_package(Qt5 COMPONENTS ${_MODULES})
    if (Qt5_FOUND)
      #Add Qt5 include and libraries paths to the sets
      foreach( _module ${_MODULES})
        message("Adding module ${_module}")
		get_target_property(Qt${_module}_location Qt5::${_module} LOCATION)
        set(Qt5_INCLUDES ${Qt5_INCLUDES} ${Qt5${_module}_INCLUDE_DIRS} )
        set(Qt5_LIBRARIES ${Qt5_LIBRARIES} ${Qt5${_module}_LIBRARIES} )
        #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5${_module}_EXECUTABLE_COMPILE_FLAGS}")
      endforeach()
    else (Qt5_FOUND)

      # FIXME: If Qt5 is not found, try to find Qt4. Modules must be prefixed
      # by Qt (i.e. QtCore, QtGui, etc. instead of Core, Gui, etc.)
      message("Qt5 not found: trying to find Qt4 (old support)")
      foreach(_MODULE ${_MODULES})
        list(APPEND _QT4_MODULES "Qt${_MODULE}")
      endforeach()

      find_package(Qt4 REQUIRED COMPONENTS ${_QT4_MODULES})
      if (Qt4_FOUND)
	  
        # Add Qt4 include and library paths in Qt5 variables to avoid to
        # make many change for a temporary support of old Qt4...
        foreach(_module ${_QT4_MODULES})
          message("Adding module ${_module} (Qt4)")
		  get_target_property(${_module}_location Qt4::${_module} LOCATION)
          string(TOUPPER ${_module} _module)
          list(APPEND Qt5_INCLUDES ${QT_${_module}_INCLUDE_DIR})
          list(APPEND Qt5_LIBRARIES ${QT_${_module}_LIBRARY})
        endforeach()

        # Add Qt base include directory
        list(APPEND Qt5_INCLUDES ${QT_HEADERS_DIR})

      endif (Qt4_FOUND)

    endif (Qt5_FOUND)
=======
    find_package(Qt5 REQUIRED ${_MODULES})
    message("Found Qt5 ${Qt5Core_VERSION}")
    #Add Qt5 include and libraries paths to the sets
    foreach( _module ${_MODULES})
      message("Adding module ${_module}")
      set(Qt5_INCLUDES ${Qt5_INCLUDES} ${Qt5${_module}_INCLUDE_DIRS} )
      set(Qt5_LIBRARIES ${Qt5_LIBRARIES} ${Qt5${_module}_LIBRARIES} )
      #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5${_module}_EXECUTABLE_COMPILE_FLAGS}")
    endforeach()
>>>>>>> master
  endif()
 endmacro()

# FIXME: Wrap macros to avoid to make many change for a temporary
# support of old Qt4...
macro(qt5_wrap_cpp)
  if (Qt5_FOUND)
    qt5_wrap_cpp(${ARGV})
  else (Qt5_FOUND)
    qt4_wrap_cpp(${ARGV})
  endif (Qt5_FOUND)
endmacro(qt5_wrap_cpp)
macro(qt5_wrap_ui)
  if (Qt5_FOUND)
    qt5_wrap_ui(${ARGV})
  else (Qt5_FOUND)
    qt4_wrap_ui(${ARGV})
  endif (Qt5_FOUND)
endmacro(qt5_wrap_ui)
macro(qt5_add_resources)
  if (Qt5_FOUND)
    qt5_add_resources(${ARGV})
  else (Qt5_FOUND)
    qt4_add_resources(${ARGV})
  endif (Qt5_FOUND)
endmacro(qt5_add_resources)
macro(qt5_generate_dbus_interface)
  if (Qt5_FOUND)
    qt5_generate_dbus_interface(${ARGV})
  else (Qt5_FOUND)
    qt4_generate_dbus_interface(${ARGV})
  endif (Qt5_FOUND)
endmacro(qt5_generate_dbus_interface)
macro(qt5_add_dbus_adaptor)
  if (Qt5_FOUND)
    qt5_add_dbus_adaptor(${ARGV})
  else (Qt5_FOUND)
    qt4_add_dbus_adaptor(${ARGV})
  endif (Qt5_FOUND)
endmacro(qt5_add_dbus_adaptor)
