# Copyright 2002-2019 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )

add_definitions(-DLIMA_SEMANTICANALYSIS_EXPORTING)

########### next target ###############

SET(lima-lp-semanticanalysis_LIB_SRCS
   ConstraintFunction.cpp
   SemanticRelationAnnotation.cpp
   SemanticRelationData.cpp
   SemanticRoleLabelingLoader.cpp
   LimaConllTokenIdMapping.cpp
   SemanticRelationsXmlLogger.cpp
   ConllDumper.cpp
#    BowGeneration.cpp
)

if (${PYTHONLIBS_FOUND})
  SET(lima-lp-semanticanalysis_LIB_SRCS
    KnowledgeBasedSemanticRoleLabeler.cpp
    ${lima-lp-semanticanalysis_LIB_SRCS}
  )
endif()
DECLARE_LIMA_PLUGIN(lima-lp-semanticanalysis)

#add_library(lima-lp-semanticanalysis SHARED ${lima-lp-semanticanalysis_LIB_SRCS})


target_link_libraries(lima-lp-semanticanalysis
    lima-lp-linguisticprocessors
    lima-lp-linguisticanalysisstructure
    lima-lp-textsegmentation
    lima-lp-syntacticanalysis
    lima-lp-automaton
    ${optionalLibs}
)

if (${PYTHONLIBS_FOUND})
  target_link_libraries(lima-lp-semanticanalysis
    ${PYTHON_LIBRARIES}
  )
endif()

set_target_properties(lima-lp-semanticanalysis PROPERTIES VERSION ${LIMA_LP_LIB_VERSION} SOVERSION ${LIMA_LP_LIB_SOVERSION})

install(TARGETS lima-lp-semanticanalysis DESTINATION ${LIB_INSTALL_DIR}
  COMPONENT runtime)


########### install files ###############

install(FILES
    SemanticAnnotation.h
    SemanticRelationAnnotation.h
    SemanticRelationData.h
    SemanticRelationsXmlLogger.h
    SemanticAnalysisExport.h
  DESTINATION include/linguisticProcessing/core/SemanticAnalysis
  COMPONENT devel
)
