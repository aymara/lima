# Copyright 2002-2019 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )

########### next target ###############

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

target_compile_definitions(lima-lp-flattokenizer PRIVATE "-DLIMA_FLATTOKENIZER_EXPORTING")

target_link_libraries(lima-lp-flattokenizer
  lima-common-processunitframework
  lima-lp-linguisticdata
  lima-lp-linguisticresources
  lima-lp-annotationgraph
  lima-lp-linguisticanalysisstructure
)

set_target_properties(lima-lp-flattokenizer PROPERTIES VERSION ${LIMA_LP_LIB_VERSION} SOVERSION ${LIMA_LP_LIB_SOVERSION})

install(TARGETS lima-lp-flattokenizer DESTINATION ${LIB_INSTALL_DIR}
  COMPONENT runtime)

########### next target ###############

SET(limatokenizerparser_SRCS
  LimaTokenizerParser.cpp
)

add_executable(limatokenizerparser ${limatokenizerparser_SRCS})

target_link_libraries(limatokenizerparser)

install(TARGETS limatokenizerparser DESTINATION bin
  COMPONENT runtime)

########### next target ###############

SET(limacharchartparser_SRCS
  SpiritCharChartParser.cpp
)

add_executable(limacharchartparser ${limacharchartparser_SRCS})

target_link_libraries(limacharchartparser
  Boost::program_options
)

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
