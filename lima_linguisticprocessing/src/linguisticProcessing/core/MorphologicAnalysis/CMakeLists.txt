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
include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )

add_definitions(-DLIMA_MORPHOLOGICANALYSIS_EXPORTING)

########### next target ###############

SET(lima-lp-morphologicanalysis_LIB_SRCS
   AbbreviationSplitAlternatives.cpp
   HyphenWordAlternatives.cpp
   SimpleWord.cpp
   UnknownWordLogger.cpp
   IdiomaticAlternativesConstraints.cpp
   SimpleDefaultProperties.cpp
   MorphoSyntacticDataHandler.cpp
   ConcatenatedDataHandler.cpp
   AlternativesReader.cpp
   DefaultProperties.cpp
   SequenceEntryHandler.cpp
   IdiomaticExpressionAnnotation.cpp
   AccentedConcatenatedDataHandler.cpp
)

if (ENCHANT_FOUND)
SET(lima-lp-morphologicanalysis_LIB_SRCS
  ${lima-lp-morphologicanalysis_LIB_SRCS}
  EnchantSpellingAlternatives.cpp
)
endif (ENCHANT_FOUND)

if (HUNSPELL_FOUND)
SET(lima-lp-morphologicanalysis_LIB_SRCS
  ${lima-lp-morphologicanalysis_LIB_SRCS}
  HunspellSpellingAlternatives.cpp
)
endif ()

DECLARE_LIMA_PLUGIN(lima-lp-morphologicanalysis)

target_link_libraries(lima-lp-morphologicanalysis
    lima-common-factory
    lima-common-misc
    lima-common-data
    lima-common-fsaaccess
    lima-common-mediaticdata
    lima-common-time
    lima-common-xmlconfigurationfiles
    lima-common-processunitframework
    lima-common-mediaprocessors
    lima-lp-linguisticprocessors
    lima-lp-linguisticresources
    lima-lp-annotationgraph
    lima-lp-analysisdict
    lima-lp-linguisticanalysisstructure
    lima-lp-applyrecognizer
    lima-lp-automaton
    ${optionalLibs}
    ${Boost_LIBRARIES}
    ${QT_LIBRARIES}
)

set_target_properties(lima-lp-morphologicanalysis PROPERTIES VERSION ${LIMA_LP_LIB_VERSION} SOVERSION ${LIMA_LP_LIB_SOVERSION})

install(TARGETS lima-lp-morphologicanalysis DESTINATION lib)

install(FILES
  MorphologicAnalysisExport.h
  MorphoSyntacticDataHandler.h
  AlternativesReader.h
  DesagglutinationResources.h
  ConcatenatedDataHandler.h
  SequenceEntryHandler.h
  AccentedConcatenatedDataHandler.h
DESTINATION include/linguisticProcessing/core/MorphologicAnalysis)


########### install files ###############
