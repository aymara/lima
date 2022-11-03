# Copyright 2002-2019 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )

add_definitions(-DLIMA_POSTAGGER_EXPORTING)
########### next target ###############

SET(lima-lp-postagger_LIB_SRCS
   disambiguationLogger.cpp
   dotGraphWriter.cpp
   frequencyCost.cpp
   integerCost.cpp
   ngramMatrices.cpp
   posTaggingGraphWriter.cpp
   dummyPosTagger.cpp
   greedyPosTagger.cpp
   ViterbiPosTagger.cpp
   DisambiguatedGraphXmlLogger.cpp
)

SET(lima-lp-postagger_LIB_LINK
   lima-lp-linguisticprocessors
   Boost::regex
)

IF(SVMTOOL++_FOUND)
SET(lima-lp-postagger_LIB_SRCS
  ${lima-lp-postagger_LIB_SRCS}
  SvmToolPosTagger.cpp
  DynamicSvmToolPosTagger.cpp
)
SET(lima-lp-postagger_LIB_LINK
  ${lima-lp-postagger_LIB_LINK}
  svmtool
)
ENDIF(SVMTOOL++_FOUND)


DECLARE_LIMA_PLUGIN(lima-lp-postagger)

target_link_libraries(lima-lp-postagger
  ${lima-lp-postagger_LIB_LINK}
)


set_target_properties(lima-lp-postagger PROPERTIES VERSION ${LIMA_LP_LIB_VERSION} SOVERSION ${LIMA_LP_LIB_SOVERSION})

install(TARGETS lima-lp-postagger DESTINATION ${LIB_INSTALL_DIR}
  COMPONENT runtime)


########### install files ###############
