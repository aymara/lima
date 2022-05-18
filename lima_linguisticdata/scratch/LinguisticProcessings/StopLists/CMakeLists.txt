# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

  add_custom_command(
    OUTPUT stoplistsdepends
    COMMAND touch stoplistsdepends
    COMMAND install -d ${CMAKE_BINARY_DIR}/execEnv/resources/LinguisticProcessings/StopLists
    COMMAND install ${CMAKE_CURRENT_SOURCE_DIR}/stopList-eng.dat ${CMAKE_CURRENT_SOURCE_DIR}/stopList-fre.dat ${CMAKE_BINARY_DIR}/execEnv/resources/LinguisticProcessings/StopLists
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
