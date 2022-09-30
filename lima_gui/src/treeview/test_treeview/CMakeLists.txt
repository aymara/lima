# Copyright 2022 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

########### next target ###############
set (test_treeview_SRCS
  main.cpp
  treeitem.cpp
  treemodel.cpp
)

message("Qt${QT_VERSION_MAJOR}Core_VERSION ${Qt${QT_VERSION_MAJOR}Core_VERSION}")
if(Qt${QT_VERSION_MAJOR}Core_VERSION VERSION_GREATER 5.14.0)
  qt_add_resources(test_treeview_SRCS qml.qrc)
else()
  qt5_add_resources(test_treeview_SRCS qml.qrc)
endif()

add_executable(test_treeview
  ${test_treeview_SRCS}
)
target_link_libraries(test_treeview
  ${Qt_LIBRARIES}
)

install(TARGETS test_treeview DESTINATION bin
  COMPONENT runtime)

########### install files ###############
