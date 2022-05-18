# Copyright 2002-2019 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

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

DECLARE_LIMA_PLUGIN(lima-lp-morphologicanalysis)

target_link_libraries(lima-lp-morphologicanalysis
    lima-common-misc
    lima-common-xmlconfigurationfiles
    lima-lp-linguisticresources
    lima-lp-analysisdict
    lima-lp-linguisticanalysisstructure
    lima-lp-applyrecognizer
    Boost::regex
    ${optionalLibs}
)

set_target_properties(lima-lp-morphologicanalysis PROPERTIES VERSION ${LIMA_LP_LIB_VERSION} SOVERSION ${LIMA_LP_LIB_SOVERSION})

install(TARGETS lima-lp-morphologicanalysis DESTINATION ${LIB_INSTALL_DIR}
  COMPONENT runtime)

install(FILES
    MorphologicAnalysisExport.h
    MorphoSyntacticDataHandler.h
    AlternativesReader.h
    DesagglutinationResources.h
    ConcatenatedDataHandler.h
    SequenceEntryHandler.h
    AccentedConcatenatedDataHandler.h
  DESTINATION include/linguisticProcessing/core/MorphologicAnalysis
  COMPONENT devel
)


########### install files ###############
