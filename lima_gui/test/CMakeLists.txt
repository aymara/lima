# Copyright 2017 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

########### next target ###############

set (test_gui_SRCS
  main.cpp
)

add_executable(test_gui 
  ${test_gui_SRCS}
)
target_link_libraries(test_gui 
  ${Qt5_LIBRARIES}
)

install(TARGETS test_gui DESTINATION bin)
