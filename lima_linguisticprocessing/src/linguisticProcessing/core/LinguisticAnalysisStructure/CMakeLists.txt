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


add_definitions(-DLIMA_LINGUISTICANALYSISSTRUCTURE_EXPORTING)

########### next target ###############

SET(lima-lp-linguisticanalysisstructure_LIB_SRCS
   LinguisticGraph.cpp
   Token.cpp 
   TStatus.cpp
   MorphoSyntacticData.cpp 
   AnalysisGraph.cpp 
   FullTokenXmlLogger.cpp 
   MorphoSyntacticDataUtils.cpp 
   AgglutinatedToken.cpp 
)

DECLARE_LIMA_PLUGIN(lima-lp-linguisticanalysisstructure)

target_link_libraries(lima-lp-linguisticanalysisstructure lima-common-factory lima-common-misc lima-common-data lima-common-fsaaccess lima-common-mediaticdata lima-common-time lima-common-factory lima-common-xmlconfigurationfiles lima-common-processunitframework lima-common-mediaprocessors lima-lp-linguisticprocessors lima-lp-linguisticresources lima-lp-annotationgraph
)

set_target_properties(lima-lp-linguisticanalysisstructure PROPERTIES VERSION ${LIMA_LP_LIB_VERSION} SOVERSION ${LIMA_LP_LIB_SOVERSION})

install(TARGETS lima-lp-linguisticanalysisstructure DESTINATION lib)


########### install files ###############
install(FILES  
  LinguisticAnalysisStructureExport.h
  TStatus.h 
  Token.h 
  MorphoSyntacticData.h 
  LinguisticGraph.h 
  AnalysisGraph.h 
  FullTokenXmlLogger.h 
  MorphoSyntacticDataUtils.h 
  ChainIdStruct.h 
  AgglutinatedToken.h
  DESTINATION include/linguisticProcessing/core/LinguisticAnalysisStructure)
