# Copyright 2002-2019 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )

add_subdirectory(AnalysisHandlers)
add_subdirectory(xmlreader)

add_definitions(-DLIMA_LINGUISTICPROCESSIONGCLIENT_EXPORTING)

########### next target ###############

SET(lima-lp-analyzer_LIB_SRCS
    lima.cpp
)

add_library(lima-lp-analyzer SHARED ${lima-lp-analyzer_LIB_SRCS})
ADD_DLL_TO_PATH(lima-lp-analyzer)


target_link_libraries(lima-lp-analyzer
    lima-linguisticprocessing
    lima-common-datahandler
    lima-common-data
    lima-common-factory
    lima-common-fsaaccess
    lima-common-mediaprocessors
    lima-common-mediaticdata
    lima-common-misc
    lima-common-processunitframework
    lima-common
    lima-common-time
    lima-common-tools
    lima-common-xmlconfigurationfiles
    lima-linguisticprocessing
    lima-lp-analysisdict
    lima-lp-analysisdumpers
    lima-lp-analysishandlers
    lima-lp-applyrecognizer
    lima-lp-automatoncompiler
    lima-lp-automaton
    lima-lp-bagofwords
    lima-lp-client
    lima-lp-compounds
    lima-lp-conllureader
    lima-lp-corefsolver
    lima-lp-dictionary
    lima-lp-entitytracker
    lima-lp-eventanalyzer
    lima-lp-flattokenizer
    lima-lp-helpers
    lima-lp-langdetector
    lima-lp-lineartextrepresentation
    lima-lp-linguisticanalysisstructure
    lima-lp-linguisticdata
    lima-lp-linguisticprocessing-core
    lima-lp-linguisticprocessors
    lima-lp-linguisticresources
    lima-lp-misc
    lima-lp-morphologicanalysis
    lima-lp-postagger
    lima-lp-propertycode
    lima-lp-regexmatcher
    lima-lp-se-datetime
    lima-lp-semanticanalysis
    lima-lp-se-number
    lima-lp-se-person
    lima-lp-specificentities
    lima-lp-syntacticanalysis
    lima-lp-textsegmentation
    lima-lp-tgv
    lima-lp-wordsensetagger
    lima-xp-client
    lima-xp-core
    lima-xp-documentsreader
    lima-xp-qtsgmlentities
)

set_target_properties(lima-lp-analyzer PROPERTIES VERSION ${LIMA_LP_LIB_VERSION} SOVERSION ${LIMA_LP_LIB_SOVERSION})

install(TARGETS lima-lp-analyzer DESTINATION ${LIB_INSTALL_DIR}
  COMPONENT runtime)


########### next target ###############

SET(lima-lp-client_LIB_SRCS
    LinguisticProcessingClientFactory.cpp
)

add_library(lima-lp-client SHARED ${lima-lp-client_LIB_SRCS})
ADD_DLL_TO_PATH(lima-lp-client)


target_link_libraries(lima-lp-client
  lima-common-factory
)

set_target_properties(lima-lp-client PROPERTIES VERSION ${LIMA_LP_LIB_VERSION} SOVERSION ${LIMA_LP_LIB_SOVERSION})

install(TARGETS lima-lp-client DESTINATION ${LIB_INSTALL_DIR}
  COMPONENT runtime)


########### install files ###############
install(FILES
    LinguisticProcessingClientExport.h
    AbstractLinguisticProcessingClient.h
    LinguisticProcessingException.h
    LinguisticProcessingClientFactory.h
  DESTINATION include/linguisticProcessing/client
  COMPONENT devel
)
