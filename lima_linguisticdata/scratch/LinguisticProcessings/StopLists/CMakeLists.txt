# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

  add_custom_command(
    OUTPUT stoplistsdepends
    COMMAND ${CMAKE_COMMAND} -E touch stoplistsdepends
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/execEnv/resources/LinguisticProcessings/StopLists
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/stopList-eng.dat ${CMAKE_CURRENT_SOURCE_DIR}/stopList-fre.dat ${CMAKE_BINARY_DIR}/execEnv/resources/LinguisticProcessings/StopLists
    DEPENDS stopList-eng.dat stopList-fre.dat
    VERBATIM
  )

  add_custom_target(
    stoplistsdependstarget
    ALL
    DEPENDS stoplistsdepends
  )

install(FILES
    stopList-eng.dat
    stopList-fre.dat
  COMPONENT common DESTINATION share/apps/lima/resources/LinguisticProcessings/StopLists)
