# Copyright 2002-2019 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

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
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DQT_COMPILING_QSTRING_COMPAT_CPP /D_SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS")
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# This macro does the find_package with the required Qt5 modules listed as
# parameters. Note that the Qt5_INCLUDES variable is not necessary anymore as
# the inclusion of variables Qt5::Component in the target_link_libraries call
# now automatically add the necessary include path, compiler settings and
# several other things. In the same way, the Qt5_LIBRARIES variable now is just
# a string with the Qt5::Component elements. It can be use in
# target_link_libraries calls to simplify its writing.
macro(addQt5Modules)
  set(_MODULES Core ${ARGV})
  #message("MODULES:${_MODULES}")
  if(NOT "${_MODULES}" STREQUAL "")
    # Use find_package to get includes and libraries directories
    find_package(Qt5 REQUIRED ${_MODULES})
    message("Found Qt5 ${Qt5Core_VERSION}")
    #Add Qt5 include and libraries paths to the sets
    foreach( _module ${_MODULES})
      message("Adding module ${_module}")
      set(Qt5_LIBRARIES ${Qt5_LIBRARIES} Qt5::${_module} )

      get_target_property(QtModule_location Qt5::${_module} LOCATION)

      if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
        install(FILES ${QtModule_location}
                DESTINATION ${LIB_INSTALL_DIR}
                COMPONENT runtime)
        set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS
          ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
          ${QtModule_location}
          )
      endif ()

    endforeach()
  endif()
endmacro()
