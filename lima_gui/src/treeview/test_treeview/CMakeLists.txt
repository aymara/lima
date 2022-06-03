#   Copyright 2017 CEA LIST
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


########### next target ###############
set (test_treeview_SRCS
  main.cpp
  treeitem.cpp
  treemodel.cpp
)
qt_add_resources(test_treeview_SRCS qml.qrc)

add_executable(test_treeview 
  ${test_treeview_SRCS}
)
target_link_libraries(test_treeview 
  ${Qt_LIBRARIES}
)

install(TARGETS test_treeview DESTINATION bin)

########### install files ###############
