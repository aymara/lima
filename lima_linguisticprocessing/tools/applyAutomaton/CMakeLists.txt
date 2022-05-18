# Copyright 2002-2019 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/. ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/src/linguisticProcessing/core )


########### next target ###############

SET(apply-rules_SRCS
  lima-lp-applyrecognizer.cpp
  apply-rules.cpp
)

add_executable(apply-rules ${apply-rules_SRCS})

target_link_libraries(apply-rules
  misc
  data
  tokenizer
  phenixGraph
  general
  lp-core-annotationGraph
  lima-lp-automatoncompiler
  lima-lp-syntacticanalysis
  lima-lp-specificentities
  lima-lp-postagger
  lima-lp-morphologicanalysis
  lima-lp-linguisticresources
  lima-lp-linguisticprocessors
  lima-lp-dictionary
  lima-lp-automaton
  lima-lp-analysisdumpers
  lima-lp-linguisticprocessing-core
  lima-lp-analysishandlers
  lima-lp-client
  lima-lp-applyrecognizer
  lima-lp-analysisdict
  lima-lp-propertycode
  lima-lp-linguisticdata
  lima-lp-bagofwords
  lima-common-data
  lima-common-fsaaccess
  lima-common-processunitframework
  lima-common-xmlconfigurationfiles
  ${optionalLibs}
)

install(TARGETS apply-rules DESTINATION bin
  COMPONENT runtime)

########### install files ###############
