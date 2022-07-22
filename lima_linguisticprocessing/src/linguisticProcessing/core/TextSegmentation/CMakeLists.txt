# Copyright 2002-2020 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )

add_definitions(-DLIMA_TEXTSEGMENTATION_EXPORTING)

########### next target ###############

SET(lima-lp-textsegmentation_LIB_SRCS
   SegmentationData.cpp
   SegmentationDataXmlLogger.cpp
   SegmentationResultsLoader.cpp
   SegmentFeatureExtractor.cpp
   SegmentFeaturesDumper.cpp
   SentenceBoundariesFinder.cpp
   ParagraphBoundariesFinder.cpp
   SentenceBoundariesXmlLogger.cpp
   SentenceBoundariesTransfer.cpp
   SentenceBoundariesUpdater.cpp
# TemporalSegmentationFeatures has to be moved to a new library just because otherwise it does not compile under Windows
#   TemporalSegmentationFeatures.cpp
)

DECLARE_LIMA_PLUGIN(lima-lp-textsegmentation)

target_link_libraries(lima-lp-textsegmentation
  lima-lp-linguisticprocessors
  lima-lp-linguisticresources
  lima-lp-annotationgraph
  lima-lp-linguisticanalysisstructure
  lima-lp-helpers
)

set_target_properties(lima-lp-textsegmentation PROPERTIES VERSION ${LIMA_LP_LIB_VERSION} SOVERSION ${LIMA_LP_LIB_SOVERSION})

install(TARGETS lima-lp-textsegmentation DESTINATION ${LIB_INSTALL_DIR}
  COMPONENT runtime)

########### install files ###############

install(FILES
    SegmentationData.h
    SegmentationDataXmlLogger.h
    SegmentationResultsLoader.h
    SegmentFeatureExtractor.h
    SegmentFeaturesDumper.h
    SentenceBoundariesFinder.h
    ParagraphBoundariesFinder.h
    SentenceBoundariesXmlLogger.h
    TemporalSegmentationFeatures.h
    TextSegmentationExport.h
    SentenceBoundariesTransfer.h
    SentenceBoundariesUpdater.h
  DESTINATION include/linguisticProcessing/core/TextSegmentation
  COMPONENT devel
)
