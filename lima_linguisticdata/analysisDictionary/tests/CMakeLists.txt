#   Copyright 2002-2019 CEA LIST
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
########### next test ###############

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

