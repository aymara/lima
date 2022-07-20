# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

set(ENV{LC_ALL} "C")

file(GLOB lefffFiles ${CMAKE_CURRENT_SOURCE_DIR}/lefff-ext-?.txt)


if (NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  add_custom_command(
    OUTPUT lefff-ext-simplified.txt
    COMMAND cat ${lefffFiles} > lefff-ext.txt
    COMMAND ${PROJECT_SOURCE_DIR}/lima_linguisticdata/scripts/extract-lefff-forms.sh lefff-ext.txt > lefff-ext-simplified.txt
    COMMAND cat ${CMAKE_CURRENT_SOURCE_DIR}/lefff-amalgames.txt >> lefff-ext-simplified.txt
    COMMAND LC_ALL="C" sort -u -o lefff-ext-simplified.txt lefff-ext-simplified.txt
    DEPENDS 
      ${lefffFiles}
      ${CMAKE_CURRENT_SOURCE_DIR}/lefff-amalgames.txt
      ${PROJECT_SOURCE_DIR}/lima_linguisticdata/scripts/extract-lefff-forms.sh
    COMMENT "Simplify the Lefff"
    VERBATIM
  )
else()
  add_custom_command(
    OUTPUT lefff-ext-simplified.txt
    COMMAND cat ${lefffFiles} > lefff-ext.txt
    COMMAND ${PROJECT_SOURCE_DIR}/lima_linguisticdata/scripts/extract-lefff-forms.sh lefff-ext.txt > lefff-ext-simplified.txt
    COMMAND cat ${CMAKE_CURRENT_SOURCE_DIR}/lefff-amalgames.txt >> lefff-ext-simplified.txt
    COMMAND sort -u -o lefff-ext-simplified.txt lefff-ext-simplified.txt
    DEPENDS 
      ${lefffFiles}
      ${CMAKE_CURRENT_SOURCE_DIR}/lefff-amalgames.txt
      ${PROJECT_SOURCE_DIR}/lima_linguisticdata/scripts/extract-lefff-forms.sh
    COMMENT "Simplify the Lefff"
    VERBATIM
  )
endif()

add_custom_target(
  leffffre
  ALL
  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/lefff-ext-simplified.txt
)


add_dependencies(leffffre codefre)
