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
file(GLOB lefffFiles ${CMAKE_CURRENT_SOURCE_DIR}/lefff-ext-?.dic)


add_custom_command(
  OUTPUT lefff-ext-simplified.txt
  COMMAND cat ${lefffFiles}> lefff-ext.txt
  COMMAND ${PROJECT_SOURCE_DIR}/scripts/extract-lefff-forms.sh lefff-ext.txt > lefff-ext-simplified.txt
  COMMAND cat ${CMAKE_CURRENT_SOURCE_DIR}/lefff-amalgames.txt >> lefff-ext-simplified.txt
  COMMAND LC_ALL="C" sort -u -o lefff-ext-simplified.txt lefff-ext-simplified.txt
  DEPENDS 
    ${lefffFiles}
    ${CMAKE_CURRENT_SOURCE_DIR}/lefff-amalgames.txt
    ${PROJECT_SOURCE_DIR}/scripts/extract-lefff-forms.sh
  COMMENT "Simplify the Lefff"
  VERBATIM
)

add_custom_target(
  leffffre
  ALL
  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/lefff-ext-simplified.txt
)


add_dependencies(leffffre codefre)
