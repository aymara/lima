# Copyright 2002-2019 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )

add_definitions( -DLIMA_AUTOMATONCOMPILER_EXPORTING )

########### next target ###############

SET(lima-lp-automatoncompiler_LIB_SRCS
   automatonCompiler.cpp
   automatonString.cpp
   compilerExceptions.h
   gazeteer.cpp
   recognizerCompiler.cpp
   ruleCompiler.cpp
   ruleString.cpp
   subAutomaton.cpp
   subPartIndex.cpp
   transitionCompiler.cpp
   tstring.cpp
)

add_library(lima-lp-automatoncompiler SHARED ${lima-lp-automatoncompiler_LIB_SRCS})
ADD_DLL_TO_PATH(lima-lp-automatoncompiler)

target_link_libraries(lima-lp-automatoncompiler
  lima-lp-automaton
  lima-lp-analysisdict
  lima-xp-core
)

set_target_properties(lima-lp-automatoncompiler PROPERTIES VERSION ${LIMA_LP_LIB_VERSION} SOVERSION ${LIMA_LP_LIB_SOVERSION})

install(TARGETS lima-lp-automatoncompiler DESTINATION ${LIB_INSTALL_DIR}
  COMPONENT runtime)


########### install files ###############
