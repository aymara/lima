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
include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )


add_definitions(-DLIMA_FLATTOKENIZER_EXPORTING)

########### next target ###############

LIMALP_WRAP_HEADERS(flattokenizer
  Automaton.h
  CharChart.h
  TokenizerAutomaton.h
)

SET(lima-lp-flattokenizer_LIB_SRCS
   Automaton.cpp
   Char.cpp
   CharChart.cpp
   CharClass.cpp
   Condition.cpp
   Events.cpp
   ReturnStatus.cpp
   SpiritTokenizerLoader.cpp
   State.cpp
   Text.cpp
   Tokenizer.cpp
   TokenizerAutomaton.cpp
   Transition.cpp
)

DECLARE_LIMA_PLUGIN(lima-lp-flattokenizer)

target_link_libraries(lima-lp-flattokenizer 
    lima-lp-linguisticdata
    lima-lp-linguisticresources 
    lima-lp-annotationgraph 
    lima-lp-linguisticanalysisstructure 
    lima-common
    ${Boost_LIBRARIES} 
    ${QT_LIBRARIES} 
)

set_target_properties(lima-lp-flattokenizer PROPERTIES VERSION ${LIMA_LP_LIB_VERSION} SOVERSION ${LIMA_LP_LIB_SOVERSION})

install(TARGETS lima-lp-flattokenizer DESTINATION ${LIB_INSTALL_DIR}
  COMPONENT runtime)

########### next target ###############

SET(limatokenizerparser_SRCS
LimaTokenizerParser.cpp
)

add_executable(limatokenizerparser ${limatokenizerparser_SRCS})

target_link_libraries(limatokenizerparser ${Boost_LIBRARIES})

install(TARGETS limatokenizerparser DESTINATION bin
  COMPONENT runtime)

########### next target ###############


SET(limacharchartparser_SRCS
SpiritCharChartParser.cpp
)

add_executable(limacharchartparser ${limacharchartparser_SRCS})

target_link_libraries(limacharchartparser ${Boost_LIBRARIES})

install(TARGETS limacharchartparser DESTINATION bin
  COMPONENT runtime)

########### install files ###############
install(FILES
    CharChart.h 
    FlatTokenizerExport.h
  DESTINATION include/linguisticProcessing/core/FlatTokenizer
  COMPONENT devel
)


add_subdirectory(Tests)
