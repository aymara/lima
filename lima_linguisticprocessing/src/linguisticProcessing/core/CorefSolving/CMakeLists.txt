# Copyright 2002-2019 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )


add_definitions(-DLIMA_COREFSOLVING_EXPORTING)

########### next target ###############

SET(lima-lp-corefsolver_LIB_SRCS
   corefSolver.cpp coreferentAnnotation.cpp CorefSolvingXmlLogger.cpp CorefSolvingNormalizedXmlLogger.cpp
)

DECLARE_LIMA_PLUGIN(lima-lp-corefsolver)

target_link_libraries(lima-lp-corefsolver
   lima-lp-linguisticanalysisstructure
   lima-lp-syntacticanalysis
)

set_target_properties(lima-lp-corefsolver PROPERTIES VERSION ${LIMA_LP_LIB_VERSION} SOVERSION ${LIMA_LP_LIB_SOVERSION})

install(TARGETS lima-lp-corefsolver DESTINATION ${LIB_INSTALL_DIR}
  COMPONENT runtime)


########### install files ###############
install(FILES
    coreferentAnnotation.h
    CorefSolvingXmlLogger.h
    CorefSolvingNormalizedXmlLogger.h
  DESTINATION include/linguisticProcessing/core/CorefSolving
  COMPONENT devel
)
