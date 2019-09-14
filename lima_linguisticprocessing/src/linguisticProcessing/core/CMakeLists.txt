# Copyright 2002-2020 CEA LIST
#
# This file is part of LIMA.
#
# LIMA is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# LIMA is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with LIMA. If not, see <http://www.gnu.org/licenses/>

add_subdirectory(AnalysisDict)
add_subdirectory(AnalysisDumpers)
add_subdirectory(ApplyRecognizer)
add_subdirectory(Automaton)
add_subdirectory(CorefSolving)
add_subdirectory(Dictionary)
add_subdirectory(EntityTracking)
add_subdirectory(EventAnalysis)
add_subdirectory(FlatTokenizer)
add_subdirectory(LinguisticAnalysisStructure)
add_subdirectory(LinguisticProcessors)
add_subdirectory(LinguisticResources)
add_subdirectory(MorphologicAnalysis)
add_subdirectory(PosTagger)
add_subdirectory(RegexMatcher)
add_subdirectory(SemanticAnalysis)
add_subdirectory(SpecificEntities)
add_subdirectory(SyntacticAnalysis)
add_subdirectory(TextSegmentation)
add_subdirectory(WordSenseAnalysis)
add_subdirectory(RegexMatcher)
add_subdirectory(DocumentsReader)
add_subdirectory(TensorFlowTokenizer)

IF(TRE_FOUND)
  add_subdirectory(ApproxMatching)
ENDIF(TRE_FOUND)

if (TensorFlow_FOUND AND Eigen_FOUND)
  add_subdirectory(TensorflowSpecificEntities)
  add_subdirectory(TensorFlowUnits)
endif()

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )
add_definitions( -DLIMA_CORELINGUISTICPROCESSINGCLIENT_EXPORTING )
add_definitions( -DCOREXMLREADERCLIENT_EXPORTING )

########### next target ###############

SET(lima-lp-linguisticprocessing-core_LIB_SRCS
  CoreLinguisticProcessingClient.cpp
)

DECLARE_LIMA_PLUGIN(lima-lp-linguisticprocessing-core)

target_link_libraries(lima-lp-linguisticprocessing-core
  lima-common-misc
  lima-common-time
  lima-lp-linguisticprocessors
  lima-lp-client
)

set_target_properties(lima-lp-linguisticprocessing-core PROPERTIES VERSION ${LIMA_LP_LIB_VERSION} SOVERSION ${LIMA_LP_LIB_SOVERSION})

install(TARGETS lima-lp-linguisticprocessing-core
  DESTINATION ${LIB_INSTALL_DIR}
  COMPONENT runtime)

########### next target ###############
add_definitions( -DCOREXMLREADERCLIENT_EXPORTING )

set(lima-xp-core_LIB_SRCS
  CoreXmlReaderClient.cpp
)

DECLARE_LIMA_PLUGIN(lima-xp-core)

target_link_libraries(lima-xp-core
  lima-common-mediaprocessors
  lima-common-mediaticdata
  lima-common-xmlconfigurationfiles
  lima-common-processunitframework
  lima-common-factory
  lima-common-tools
  lima-common-data
  lima-common-datahandler
  lima-xp-documentsreader
  lima-xp-client
  ${QT_LIBRARIES}
)

set_target_properties(lima-xp-core PROPERTIES VERSION ${LIMA_LP_LIB_VERSION} SOVERSION ${LIMA_LP_LIB_SOVERSION})
install(TARGETS lima-xp-core DESTINATION ${LIB_INSTALL_DIR})

########### install files ###############

install(FILES
    CoreLinguisticProcessingClient.h
    CoreLinguisticProcessingClientExport.h
  DESTINATION include/linguisticProcessing/core
  COMPONENT devel
)
