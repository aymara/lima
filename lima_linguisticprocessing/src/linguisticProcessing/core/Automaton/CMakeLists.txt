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

add_definitions(-DLIMA_AUTOMATON_EXPORTING)

########### next target ###############

LIMALP_WRAP_HEADERS(automaton
  recognizer.h
)

SET(lima-lp-automaton_LIB_SRCS
   SpecificEntityAnnotation.cpp
   recognizerData.cpp
   andTransition.cpp
   automaton.cpp
   constraintCheckList.cpp
   entityProperties.cpp
   epsilonTransition.cpp
   lemmaTransition.cpp
   numericTransition.cpp
   posTransition.cpp
   recognizer.cpp
   rule.cpp
   setTransition.cpp
   starTransition.cpp
   transition.cpp
   transitionUnit.cpp
   tstatusTransition.cpp
   wordTransition.cpp
   gazeteerTransition.cpp
   constraint.cpp
   recognizerMatch.cpp
   deaccentuatedTransition.cpp
   constraintFunctionFactory.cpp
   constraintFunctionManager.cpp
   basicConstraintFunctions.cpp
   automatonReaderWriter.cpp
   automatonCommon.cpp
   searchGraph.cpp
   EntityFeatures.cpp
   entityGroupTransition.cpp
   entityTransition.cpp
)

add_library(lima-lp-automaton SHARED 
  ${lima-lp-automaton_LIB_SRCS} 
  ${lima-lp-automaton_MOC_OUTFILES}
)

target_link_libraries(lima-lp-automaton lima-common-factory lima-common-misc lima-common-data lima-common-fsaaccess lima-common-mediaticdata lima-common-time lima-common-factory lima-common-xmlconfigurationfiles lima-common-processunitframework lima-common-mediaprocessors lima-lp-linguisticprocessors lima-lp-linguisticresources lima-lp-annotationgraph lima-lp-linguisticanalysisstructure lima-lp-flattokenizer
 ${Boost_LIBRARIES})

set_target_properties(lima-lp-automaton PROPERTIES VERSION ${LIMA_LP_LIB_VERSION} SOVERSION ${LIMA_LP_LIB_SOVERSION})

install(TARGETS lima-lp-automaton DESTINATION ${LIB_INSTALL_DIR}
  COMPONENT runtime)


########### install files ###############
install(FILES
    AutomatonExport.h
    constraintFunction.h
    transition.h
    rule.h
    transitionSearchStructure.h
    setTransition.h
    deaccentuatedTransition.h
    recognizer.h
    EntityFeatures.h
    posTransition.h
    andTransition.h
    starTransition.h
    constraint.h
    constraintFunctionFactory.h
    tstatusTransition.h
    epsilonTransition.h
    automatonReaderWriter.h
    entityTransition.h
    entityGroupTransition.h
    entityProperties.h
    searchGraph.h
    transitionUnit.h
    automaton.h
    lemmaTransition.h
    numericTransition.h
    basicConstraintFunctions.h
    constraintCheckList.h
    automatonCommon.h
    transitionSearchStructure.tcc
    recognizerMatch.h
    constraintFunctionManager.h
    wordTransition.h
    gazeteerTransition.h
    SpecificEntityAnnotation.h
    recognizerData.h
  DESTINATION include/linguisticProcessing/core/Automaton
  COMPONENT devel
)
