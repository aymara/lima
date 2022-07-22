# Copyright 2002-2019 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

################### tests ########################

########### next test ###############

SET(CharChartTest0_SRCS
  CharChartTest0.cpp
)

add_custom_target(
  charcharttestdata
  ALL
  DEPENDS testcharchart.chars.tok
)

file(COPY testcharchart.chars.tok DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_executable(CharChartTest0 ${CharChartTest0_SRCS})

target_link_libraries(CharChartTest0
  lima-lp-flattokenizer
  Boost::unit_test_framework
)

add_test(NAME CharChartTest0 COMMAND CharChartTest0 WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

########### next test ###############

SET(CharChartTestAra_SRCS
  CharChartTest-ara.cpp
)

add_custom_target(
  charcharttestaradata
  ALL
  DEPENDS testcharchart-ara.chars.tok
)

file(COPY testcharchart-ara.chars.tok DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_executable(CharChartTestAra ${CharChartTestAra_SRCS})

target_link_libraries(CharChartTestAra
  lima-lp-flattokenizer
  Boost::unit_test_framework
)

add_test(NAME CharChartTestAra COMMAND CharChartTestAra WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
