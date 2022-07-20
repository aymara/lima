# Copyright 2002-2019 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )


add_definitions(-DLIMA_LINGUISTICANALYSISSTRUCTURE_EXPORTING)

########### next target ###############

SET(lima-lp-linguisticanalysisstructure_LIB_SRCS
   LinguisticGraph.cpp
   Token.cpp
   TStatus.cpp
   MorphoSyntacticData.cpp
   AnalysisGraph.cpp
   MorphoSyntacticDataUtils.cpp
   AgglutinatedToken.cpp
)

DECLARE_LIMA_PLUGIN(lima-lp-linguisticanalysisstructure)

target_link_libraries(lima-lp-linguisticanalysisstructure
  lima-lp-annotationgraph
)

set_target_properties(lima-lp-linguisticanalysisstructure PROPERTIES VERSION ${LIMA_LP_LIB_VERSION} SOVERSION ${LIMA_LP_LIB_SOVERSION})

install(TARGETS lima-lp-linguisticanalysisstructure DESTINATION ${LIB_INSTALL_DIR}
  COMPONENT runtime)


########### install files ###############
install(FILES
    LinguisticAnalysisStructureExport.h
    TStatus.h
    Token.h
    MorphoSyntacticData.h
    LinguisticGraph.h
    AnalysisGraph.h
    MorphoSyntacticDataUtils.h
    ChainIdStruct.h
    AgglutinatedToken.h
  DESTINATION include/linguisticProcessing/core/LinguisticAnalysisStructure
  COMPONENT devel
)
