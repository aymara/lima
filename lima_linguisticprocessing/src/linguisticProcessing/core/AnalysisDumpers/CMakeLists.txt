# Copyright 2002-2019 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

add_definitions(-DLIMA_ANALYSISDUMPERS_EXPORTING)

#include_directories( BEFORE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/src/linguisticProcessing/core/AnalysisDumpers/EasyXmlDumperProcessor )


########### next target ###############

SET(lima-lp-analysisdumpers_LIB_SRCS
   AnnotationGraphXmlDumper.cpp
   BowDumper.cpp
   DepTripleDumper.cpp
   LTRTextBuilder.cpp
   NullDumper.cpp
   StopList.cpp
   TextDumper.cpp
   fullXmlDumper.cpp
   linearTextRepresentationDumper.cpp
   linearTextRepresentationLogger.cpp
   posGraphXmlDumper.cpp
   SimpleXmlDumper.cpp
   WordFeatureExtractor.cpp
   BoWFeatureExtractor.cpp
   TextFeaturesDumper.cpp
   DepTripletLogger.cpp
   GenericXmlDumper.cpp
   BowGeneration.cpp
   CompoundsXmlLogger.cpp
   AbstractIEDumper.cpp
   BratDumper.cpp
   BratJSONDumper.cpp
   XmlIEDumper.cpp
   OffsetMapping.cpp
)

DECLARE_LIMA_PLUGIN(lima-lp-analysisdumpers)

target_link_libraries(lima-lp-analysisdumpers
  lima-lp-linguisticprocessors
  lima-lp-annotationgraph
  lima-lp-syntacticanalysis
  lima-lp-semanticanalysis
  lima-lp-textsegmentation
  lima-lp-eventanalyzer
  lima-lp-compounds
)

set_target_properties(lima-lp-analysisdumpers PROPERTIES VERSION ${LIMA_LP_LIB_VERSION} SOVERSION ${LIMA_LP_LIB_SOVERSION})

install(TARGETS lima-lp-analysisdumpers DESTINATION ${LIB_INSTALL_DIR}
  COMPONENT runtime)


########### install files ###############
install(FILES
    AnalysisDumpersExport.h
    DepTripleDumper.h
    AnnotationGraphXmlDumper.h
    fullXmlDumper.h TextDumper.h
    BowDumper.h
    linearTextRepresentationLogger.h
    LTRTextBuilder.h
    StopList.h
    linearTextRepresentationDumper.h
    SimpleXmlDumper.h
    WordFeatureExtractor.h
    BoWFeatureExtractor.h
    TextFeaturesDumper.h
    DepTripletLogger.h
    BowGeneration.h
    CompoundsXmlLogger.h
    AbstractIEDumper.h
    BratDumper.h
    BratJSONDumper.h
    XmlIEDumper.h
    OffsetMapping.h
  DESTINATION include/linguisticProcessing/core/AnalysisDumpers
  COMPONENT devel
)
