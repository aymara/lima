# Copyright 2017 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

########### next target ###############
set (test_treeview_SRCS
  main.cpp
  treeitem.cpp
  treemodel.cpp
)
qt5_add_resources(test_treeview_SRCS qml.qrc)

add_executable(test_treeview 
  ${test_treeview_SRCS}
)
target_link_libraries(test_treeview 
  ${Qt5_LIBRARIES}
)

install(TARGETS test_treeview DESTINATION bin)

########### install files ###############
