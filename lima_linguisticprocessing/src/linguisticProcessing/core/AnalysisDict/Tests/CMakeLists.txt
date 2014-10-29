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
########### next test ###############

SET(EnhancedAnalysisDictionaryTest0_SRCS
   EnhancedAnalysisDictionaryTest0.cpp
)

add_executable(EnhancedAnalysisDictionaryTest0 ${EnhancedAnalysisDictionaryTest0_SRCS})

target_link_libraries(EnhancedAnalysisDictionaryTest0 lima-common-data lima-common-qslog ${Boost_LIBRARIES} ${QT_LIBRARIES})

add_test(EnhancedAnalysisDictionaryTest0 EnhancedAnalysisDictionaryTest0)

########### next test ###############

configure_file(EnhancedAnalysisDictionaryTest1-src.cpp ${CMAKE_CURRENT_SOURCE_DIR}/EnhancedAnalysisDictionaryTest1.cpp @ONLY)

add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/EnhancedAnalysisDictionaryTest1-src.cpp
#  COMMAND ${CMAKE_COMMAND} -P
#    ${CMAKE_CURRENT_BINARY_DIR}/configure_json_file.cmake
  COMMAND configure_file(${CMAKE_CURRENT_SOURCE_DIR}/EnhancedAnalysisDictionaryTest1-src.cpp ${CMAKE_CURRENT_SOURCE_DIR}/EnhancedAnalysisDictionaryTest1.cpp @ONLY)
  DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/EnhancedAnalysisDictionaryTest1-src.cpp
  COMMENT "generate EnhancedAnalysisDictionaryTest1.cpp"
  VERBATIM)
add_custom_target(EnhancedAnalysisDictionaryTest1-src.cpp ALL
  DEPENDS EnhancedAnalysisDictionaryTest1-src.cpp)

# add_custom_target(configure_json ALL
#  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/json_file.json)

configure_file(lima-lp-res-Enhanced.xml lima-lp-res-Enhanced.xml COPYONLY )

SET(EnhancedAnalysisDictionaryTest1_SRCS
   EnhancedAnalysisDictionaryTest1.cpp
)

add_executable(EnhancedAnalysisDictionaryTest1 ${EnhancedAnalysisDictionaryTest1_SRCS})

target_link_libraries(EnhancedAnalysisDictionaryTest1 lima-common-data lima-common-mediaticdata lima-lp-linguisticdata lima-common-fsaaccess lima-common-qslog lima-common-xmlconfigurationfiles lima-lp-linguisticresources lima-lp-analysisdict  lima-lp-morphologicanalysis lima-lp-linguisticanalysisstructure ${Boost_LIBRARIES} ${QT_LIBRARIES})

add_test(EnhancedAnalysisDictionaryTest1 EnhancedAnalysisDictionaryTest1)

