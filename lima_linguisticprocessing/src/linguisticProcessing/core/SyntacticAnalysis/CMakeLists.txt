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

add_definitions(-DLIMA_SYNTACTICANALYSIS_EXPORTING)
########### next target ###############

LIMALP_WRAP_HEADERS(syntacticanalysis
  SyntagmaticMatrix.h
  SelectionalPreferences.h
)

SET(lima-lp-syntacticanalysis_LIB_SRCS
   SyntacticRelationsLogger.cpp
   SyntacticAnalysisXmlLogger.cpp
   ChainsDisambiguator.cpp
   DebugSyntacticAnalysisLogger.cpp
   DependencyGraph.cpp
   DotDependencyGraphWriter.cpp
   HomoSyntagmaticConstraints.cpp
   SimplificationConstraints.cpp
   SimplificationData.cpp
   SyntacticAnalysisTools.cpp
   SyntacticAnalyzer-chains.cpp
   SyntacticAnalyzer-nochains.cpp
   SyntacticAnalyzer-deps.cpp
   SyntacticAnalyzer-disamb.cpp
   SyntacticAnalyzer-hetero.cpp
   SyntacticAnalyzer-simplify.cpp
   SyntacticData.cpp
   SyntagmaticMatrix.cpp
   TokenFilter.cpp
   XmlSyntagmaticMatrixFileHandler.cpp
   SelectionalRestrictionsConstraints.cpp
   SelectionalPreferences.cpp
   CoordinationConstraints.cpp
)

DECLARE_LIMA_PLUGIN(lima-lp-syntacticanalysis)

target_link_libraries(lima-lp-syntacticanalysis 
    lima-common-data 
    lima-common-fsaaccess 
    lima-common-misc 
    lima-common-factory 
    lima-common-mediaprocessors 
    lima-common-mediaticdata 
    lima-common-processunitframework 
    lima-common-time 
    lima-common-xmlconfigurationfiles 
    lima-lp-annotationgraph 
    lima-lp-automaton 
    lima-lp-bagofwords
    lima-lp-linguisticanalysisstructure 
    lima-lp-linguisticprocessors 
    lima-lp-linguisticresources 
    lima-lp-postagger 
    lima-lp-textsegmentation 
)

set_target_properties(lima-lp-syntacticanalysis PROPERTIES VERSION ${LIMA_LP_LIB_VERSION} SOVERSION ${LIMA_LP_LIB_SOVERSION})

install(TARGETS lima-lp-syntacticanalysis DESTINATION ${LIB_INSTALL_DIR}
  COMPONENT runtime)


########### install files ###############
install(FILES  SyntacticRelationsLogger.h   
    ChainsDisambiguator.h    
    CoordinationConstraints.h  
    DebugSyntacticAnalysisLogger.h 
    DependencyGraph.h 
    DotDependencyGraphWriter.h 
    DotDependencyGraphWriter.tcc 
    HomoSyntagmaticConstraints.h    
    SimplificationConstraints.h   
    SimplificationData.h 
    SyntacticAnalyzer-chains.h 
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
    XmlSyntagmaticMatrixFileHandler.h 
  DESTINATION include/linguisticProcessing/core/SyntacticAnalysis
  COMPONENT devel
)
