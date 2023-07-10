# Copyright 2021 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

macro (TEST_RULES_DEPLOY)

  add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/execEnv/src/rules/test.rules
    COMMAND cp -r ${CMAKE_CURRENT_SOURCE_DIR}/src/ ${CMAKE_BINARY_DIR}/execEnv
    COMMAND cp petit.lst ${CMAKE_BINARY_DIR}/execEnv/src/rules
    DEPENDS src/rules/test.rules
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    VERBATIM
  )

  add_custom_target(
    test_rules_deploy
    ALL
    DEPENDS
      ${CMAKE_BINARY_DIR}/execEnv/src/rules/test.rules
  )

endmacro ()
########### next test ###############
TEST_RULES_DEPLOY()

SET(SearchTest0_SRCS
   SearchTest0.cpp
)

add_executable(SearchTest0 ${SearchTest0_SRCS} ${SearchTest0_MOC_OUTFILES})

target_link_libraries(SearchTest0
  Qt${QT_VERSION_MAJOR}::Test
)

add_test(
  NAME SearchTest0
  COMMAND SearchTest0
  WORKING_DIRECTORY execEnv)

