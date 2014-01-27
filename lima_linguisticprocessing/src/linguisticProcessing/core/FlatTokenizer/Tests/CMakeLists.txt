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

target_link_libraries(CharChartTest0 lima-common-data lima-common-qslog lima-lp-flattokenizer ${Boost_LIBRARIES} ${QT_LIBRARIES})

add_test(CharChartTest0 CharChartTest0 WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

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

target_link_libraries(CharChartTestAra lima-common-data lima-common-qslog lima-lp-flattokenizer ${Boost_LIBRARIES} ${QT_LIBRARIES})

add_test(CharChartTestAra CharChartTestAra WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
