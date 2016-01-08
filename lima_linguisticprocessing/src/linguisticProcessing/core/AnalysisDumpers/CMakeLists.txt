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
add_subdirectory(EasyXmlDumper)

add_definitions(-DLIMA_ANALYSISDUMPERS_EXPORTING)

include_directories( BEFORE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/src/linguisticProcessing/core/AnalysisDumpers/EasyXmlDumperProcessor )


########### next target ###############

SET(lima-lp-analysisdumpers_LIB_SRCS
   AnnotationGraphXmlDumper.cpp
   BowDumper.cpp
   DepTripleDumper.cpp
   LTRTextBuilder.cpp
   NullDumper.cpp
   StopList.cpp
   TextDumper.cpp
   ConllDumper.cpp
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
)

DECLARE_LIMA_PLUGIN(lima-lp-analysisdumpers)

target_link_libraries(lima-lp-analysisdumpers
  lima-common
  lima-lp-linguisticprocessors
  lima-lp-linguisticresources
  lima-lp-annotationgraph
  lima-lp-syntacticanalysis
  lima-lp-bagofwords
  lima-lp-linguisticanalysisstructure
  lima-lp-lineartextrepresentation
  lima-lp-textsegmentation
  lima-lp-semanticanalysis
)

set_target_properties(lima-lp-analysisdumpers PROPERTIES VERSION ${LIMA_LP_LIB_VERSION} SOVERSION ${LIMA_LP_LIB_SOVERSION})

install(TARGETS lima-lp-analysisdumpers DESTINATION ${LIB_INSTALL_DIR})


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
DESTINATION include/linguisticProcessing/core/AnalysisDumpers)
