# Copyright 2002-2019 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )

add_definitions(-DLIMA_AUTOMATON_EXPORTING)

########### next target ###############

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
)
ADD_DLL_TO_PATH(lima-lp-automaton)

target_link_libraries(lima-lp-automaton
  lima-lp-flattokenizer
  lima-lp-linguisticanalysisstructure
  lima-lp-linguisticprocessors
  lima-lp-linguisticresources
)

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
