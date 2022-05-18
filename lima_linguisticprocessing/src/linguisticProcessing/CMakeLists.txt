# Copyright 2002-2019 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

add_subdirectory(common)
add_subdirectory(client)
add_subdirectory(core)

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )

add_definitions( -DLIMA_LP_EXPORTING )

add_library(lima-linguisticprocessing SHARED
  limaSimpleClient.cpp
  limaSimpleClientDelegate.cpp
)
ADD_DLL_TO_PATH(lima-linguisticprocessing)

target_link_libraries(lima-linguisticprocessing
  lima-lp-annotationgraph
  lima-lp-lineartextrepresentation
  lima-lp-misc
  lima-lp-tgv
  lima-lp-linguisticdata
  lima-lp-bagofwords
  lima-lp-propertycode
  lima-lp-regexmatcher
  lima-lp-wordsensetagger
  lima-lp-linguisticprocessing-core
  lima-lp-analysisdict
  lima-lp-eventanalyzer
  lima-lp-corefsolver
  lima-lp-specificentities
  lima-lp-syntacticanalysis
  lima-lp-morphologicanalysis
  lima-lp-dictionary
  lima-lp-easyxmldumper
  lima-lp-analysisdumpers
  lima-lp-textsegmentation
  lima-lp-linguisticanalysisstructure
  lima-lp-semanticanalysis
  lima-lp-linguisticresources
  lima-lp-entitytracker
  lima-lp-flattokenizer
  lima-lp-applyrecognizer
  lima-lp-linguisticprocessors
  lima-lp-postagger
  lima-lp-automaton
  lima-lp-analysishandlers
  lima-lp-client
  Boost::thread
)

# if (TensorFlow_FOUND)
#   target_link_libraries(lima-linguisticprocessing
#     lima-lp-tensorflowspecificentities
#   )
# endif()

if(NOT ${SYSTEM_NAME} STREQUAL "Windows")
  target_link_libraries(lima-linguisticprocessing
    lima-lp-specificentitiesCrf
  )
endif()

set_target_properties(lima-linguisticprocessing PROPERTIES
  VERSION ${LIMA_LP_LIB_VERSION}
  SOVERSION ${LIMA_LP_LIB_SOVERSION})

install(TARGETS lima-linguisticprocessing
  DESTINATION ${LIB_INSTALL_DIR}
  COMPONENT runtime)


########### install files ###############
install(FILES
    LinguisticProcessingCommon.h
    limaSimpleClient.h
  DESTINATION include/linguisticProcessing
  COMPONENT devel
)
