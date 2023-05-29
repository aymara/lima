# Copyright 2002-2019 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )

add_definitions(-DLIMA_SYNTACTICANALYSIS_EXPORTING)
########### next target ###############

SET(lima-lp-syntacticanalysis_LIB_SRCS
   SyntacticRelationsLogger.cpp
   SyntacticAnalysisXmlLogger.cpp
   # ChainsDisambiguator.cpp
   DebugSyntacticAnalysisLogger.cpp
   DependencyGraph.cpp
   DotDependencyGraphWriter.cpp
   HomoSyntagmaticConstraints.cpp
   SimplificationConstraints.cpp
   SimplificationData.cpp
   SyntacticAnalysisTools.cpp
   # SyntacticAnalyzer-chains.cpp
   # SyntacticAnalyzer-nochains.cpp
   SyntacticAnalyzer-deps.cpp
   SyntacticAnalyzer-disamb.cpp
   SyntacticAnalyzer-hetero.cpp
   SyntacticAnalyzer-simplify.cpp
   SyntacticData.cpp
   SyntagmaticMatrix.cpp
   TokenFilter.cpp
   XmlSyntagmaticMatrixFileReader.cpp
   SelectionalRestrictionsConstraints.cpp
   SelectionalPreferences.cpp
   CoordinationConstraints.cpp
)

DECLARE_LIMA_PLUGIN(lima-lp-syntacticanalysis)

target_link_libraries(lima-lp-syntacticanalysis
    lima-lp-automaton
    lima-lp-linguisticprocessors
    lima-lp-postagger
    lima-lp-textsegmentation
)

set_target_properties(lima-lp-syntacticanalysis PROPERTIES VERSION ${LIMA_LP_LIB_VERSION} SOVERSION ${LIMA_LP_LIB_SOVERSION})

install(TARGETS lima-lp-syntacticanalysis DESTINATION ${LIB_INSTALL_DIR}
  COMPONENT runtime)


########### install files ###############
install(FILES  SyntacticRelationsLogger.h
    # ChainsDisambiguator.h
    CoordinationConstraints.h
    DebugSyntacticAnalysisLogger.h
    DependencyGraph.h
    DotDependencyGraphWriter.h
    DotDependencyGraphWriter.tcc
    HomoSyntagmaticConstraints.h
    SimplificationConstraints.h
    SimplificationData.h
    # SyntacticAnalyzer-chains.h
    SyntacticAnalyzer-deps.h
    SyntacticAnalyzer-disamb.h
    SyntacticAnalyzer-hetero.h
    SyntacticAnalyzer-simplify.h
    SyntacticAnalysisExport.h
    SyntacticAnalysisTools.h
    SyntacticAnalysisXmlLogger.h
    SyntacticData.h
    SyntagmaticMatrix.h
    TokenFilter.h
    XmlSyntagmaticMatrixFileExceptions.h
    XmlSyntagmaticMatrixFileReader.h
  DESTINATION include/linguisticProcessing/core/SyntacticAnalysis
  COMPONENT devel
)
