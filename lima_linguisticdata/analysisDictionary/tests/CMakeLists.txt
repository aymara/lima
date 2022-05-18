# Copyright 2002-2019 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

set(EnhancedAnalysisDictionaryTest0_SRCS
  EnhancedAnalysisDictionaryTest0.cpp
)

set (EnhancedAnalysisDictionaryTest0_MOC_HEADERS
  EnhancedAnalysisDictionaryTest0.h
)

add_executable(EnhancedAnalysisDictionaryTest0
  ${EnhancedAnalysisDictionaryTest0_SRCS}
  ${EnhancedAnalysisDictionaryTest0_MOC_OUTFILES}
)

target_link_libraries(EnhancedAnalysisDictionaryTest0
  lima-lp-morphologicanalysis
  Qt5::Test
)

add_dependencies(EnhancedAnalysisDictionaryTest0 pretest-execEnv)

add_test(NAME EnhancedAnalysisDictionaryTest0
  COMMAND ${CMAKE_COMMAND} -E env
    LIMA_CONF=${CMAKE_BINARY_DIR}/execEnv/config
    LIMA_RESOURCES=${CMAKE_BINARY_DIR}/execEnv/resources
    LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}/execEnv/lib/:$ENV{LD_LIBRARY_PATH}
    ./EnhancedAnalysisDictionaryTest0
)


########### next test ###############

set(EnhancedAnalysisDictionaryTest1_SRCS
  EnhancedAnalysisDictionaryTest1.cpp
)

set (EnhancedAnalysisDictionaryTest1_MOC_HEADERS
  EnhancedAnalysisDictionaryTest1.h
)

add_executable(EnhancedAnalysisDictionaryTest1
  ${EnhancedAnalysisDictionaryTest1_SRCS}
  ${EnhancedAnalysisDictionaryTest1_MOC_OUTFILES}
)

target_link_libraries(EnhancedAnalysisDictionaryTest1
  lima-lp-morphologicanalysis
  Qt5::Test
)

add_test(NAME EnhancedAnalysisDictionaryTest1
  COMMAND ${CMAKE_COMMAND} -E env
    LIMA_CONF=${CMAKE_BINARY_DIR}/execEnv/config
    LIMA_RESOURCES=${CMAKE_BINARY_DIR}/execEnv/resources
    LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}/execEnv/lib/:$ENV{LD_LIBRARY_PATH}
    ./EnhancedAnalysisDictionaryTest1
)


########### next test ###############

set (DictionaryDataTest1_MOC_HEADERS
  DictionaryDataTest1.h
)

SET(DictionaryDataTest1_SRCS
  DictionaryDataTest1.cpp
)

add_executable(DictionaryDataTest1
  ${DictionaryDataTest1_SRCS}
  ${DictionaryDataTest1_MOC_OUTFILES}
)

target_link_libraries(DictionaryDataTest1
  lima-lp-morphologicanalysis
  Qt5::Test
)

add_test(NAME DictionaryDataTest1
  COMMAND ${CMAKE_COMMAND} -E env
    LIMA_CONF=${CMAKE_BINARY_DIR}/execEnv/config
    LIMA_RESOURCES=${CMAKE_BINARY_DIR}/execEnv/resources
    LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}/execEnv/lib/:$ENV{LD_LIBRARY_PATH}
    ./DictionaryDataTest1
)

