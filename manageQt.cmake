# Copyright 2002-2019 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

# It is necessary to define Qt_INSTALL_DIR in your environment.
set(CMAKE_PREFIX_PATH
  "$ENV{Qt_INSTALL_DIR}"
  "${CMAKE_PREFIX_PATH}"
)

# Add definitions and flags
add_definitions(-DQT_NO_KEYWORDS)
add_compile_definitions(QT_DISABLE_DEPRECATED_BEFORE=0x050F00)
if (NOT (${CMAKE_SYSTEM_NAME} STREQUAL "Windows"))
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -DQT_DEPRECATED_WARNINGS")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DQT_COMPILING_QSTRING_COMPAT_CPP /D_SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS")
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# This macro does the find_package with the required Qt modules listed as
# parameters. Note that the Qt_INCLUDES variable is not necessary anymore as
# the inclusion of variables Qt${QT_VERSION_MAJOR}::Component in the target_link_libraries call
# now automatically add the necessary include path, compiler settings and
# several other things. In the same way, the Qt_LIBRARIES variable now is just
# a string with the Qt${QT_VERSION_MAJOR}::Component elements. It can be use in
# target_link_libraries calls to simplify its writing.
macro(addQtModules)
  set(_MODULES Core ${ARGV})
  #message("MODULES:${_MODULES}")
  if(NOT "${_MODULES}" STREQUAL "")
    # Use find_package to get includes and libraries directories
    find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS ${_MODULES})
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS ${_MODULES})


    message("Found Qt ${Qt${QT_VERSION_MAJOR}Core_VERSION}")
    #Add Qt include and libraries paths to the sets
    foreach( _module ${_MODULES})
      message("Adding module ${_module}")
      set(Qt_LIBRARIES ${Qt_LIBRARIES} Qt${QT_VERSION_MAJOR}::${_module} )

      get_target_property(QtModule_location Qt${QT_VERSION_MAJOR}::${_module} LOCATION)

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
