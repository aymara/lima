# Copyright 2021 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

macro (TEST_RULES_DEPLOY)

  add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/execEnv/tests/src/rules/test.rules ${CMAKE_BINARY_DIR}/execEnv/tests/src/petit.lst
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/execEnv/tests/src
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/execEnv/tests/src
    DEPENDS src/rules/test.rules src/petit.lst
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    VERBATIM
  )

  add_custom_target(
    test_rules_deploy
    ALL
    DEPENDS
      ${CMAKE_BINARY_DIR}/execEnv/tests/src/rules/test.rules
      ${CMAKE_BINARY_DIR}/execEnv/tests/src/petit.lst
  )

endmacro ()
########### next test ###############
TEST_RULES_DEPLOY()

SET(AutomatonCompilerUseTest0_SRCS
   AutomatonCompilerUseTest0.cpp
)

add_executable(AutomatonCompilerUseTest0 ${AutomatonCompilerUseTest0_SRCS} ${AutomatonCompilerUseTest0_MOC_OUTFILES})

target_link_libraries(AutomatonCompilerUseTest0
  Qt${QT_VERSION_MAJOR}::Test
)

if (WIN32)
  set(_lima_path_sep "$<SEMICOLON>")
else()
  set(_lima_path_sep ":")
endif()
add_test(
  NAME AutomatonCompilerUseTest0
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/execEnv
  COMMAND  ${CMAKE_COMMAND} -E env
    LIMA_PWD=${CMAKE_BINARY_DIR}/execEnv
    COMPILE_RULES=$<TARGET_FILE:compile-rules>
    LIMA_CONF=${CMAKE_BINARY_DIR}/execEnv/src/conf${_lima_path_sep}${CMAKE_BINARY_DIR}/execEnv/config
    LIMA_RESOURCES=${CMAKE_BINARY_DIR}/execEnv/resources
    LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}/execEnv/lib/:${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}:$ENV{LD_LIBRARY_PATH}
    $<TARGET_FILE:AutomatonCompilerUseTest0>
  )

