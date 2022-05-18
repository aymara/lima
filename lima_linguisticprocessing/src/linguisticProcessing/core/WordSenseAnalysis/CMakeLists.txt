# Copyright 2002-2019 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )

add_definitions(-DLIMA_WORDSENSEANALYSIS_EXPORTING)

########### next target ###############
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra  -Wall -Wno-return-type -g  ")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Wall -g  ")


SET(lima-lp-wordsensetagger_LIB_SRCS
  KnnSearcher.cpp
  WordUnit.cpp
  WordSenseXmlLogger.cpp
  WordSenseAnnotation.cpp
  WordSenseDisambiguator.cpp
  WordSenseUnit.cpp
  lshsignature.cpp
  hasheddb.cpp
)

DECLARE_LIMA_PLUGIN(lima-lp-wordsensetagger)

target_link_libraries(lima-lp-wordsensetagger
  lima-lp-linguisticprocessors
  lima-lp-annotationgraph
  lima-lp-syntacticanalysis
)

set_target_properties(lima-lp-wordsensetagger PROPERTIES VERSION ${LIMA_LP_LIB_VERSION} SOVERSION ${LIMA_LP_LIB_SOVERSION})


########### install files ###############

install(TARGETS lima-lp-wordsensetagger DESTINATION ${LIB_INSTALL_DIR}
  COMPONENT runtime)
