# Copyright 2017 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

########### next target ###############

set (test_gui_SRCS
  main.cpp
)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS QuickTest)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS QuickTest)


message("Found Qt ${Qt${QT_VERSION_MAJOR}Core_VERSION}")
#Add Qt include and libraries paths to the sets

get_target_property(QtModule_location Qt${QT_VERSION_MAJOR}::QuickTest LOCATION)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  install(FILES ${QtModule_location}
          DESTINATION ${LIB_INSTALL_DIR}
          COMPONENT runtime)
  set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS
    ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
    ${QtModule_location}
    )
endif ()

add_executable(test_gui
  ${test_gui_SRCS}
)
target_link_libraries(test_gui
  ${Qt_LIBRARIES}
  Qt${QT_VERSION_MAJOR}::QuickTest
)

install(TARGETS test_gui DESTINATION bin
  COMPONENT runtime)
