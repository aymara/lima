#   Copyright 2002-2013 CEA LIST
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
########### install files ###############
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
