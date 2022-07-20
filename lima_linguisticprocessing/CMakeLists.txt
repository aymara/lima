# Copyright 2002-2019 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

configure_file(LIMALPConfig-src.cmake LIMALPConfig.cmake @ONLY)

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(tools)
add_subdirectory(conf)
add_subdirectory(data)


########### install files ###############
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/LIMALPConfig.cmake
  DESTINATION share/apps/lima/cmake/Modules
  COMPONENT devel)
install(FILES ${CMAKE_BINARY_DIR}/execEnv/config/plugins/${CMAKE_PROJECT_NAME}
  DESTINATION share/config/lima/plugins
  COMPONENT runtime)

