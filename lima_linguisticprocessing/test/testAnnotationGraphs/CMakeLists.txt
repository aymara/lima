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


########### next target ###############

SET(lima-lp-test-annotationgraphs_LIB_SRCS
   AnnotationGraphTestProcessUnit.cpp
)

DECLARE_LIMA_PLUGIN(lima-lp-test-annotationgraphs)
ADD_DLL_TO_PATH(lima-lp-test-annotationgraphs)

target_link_libraries(lima-lp-test-annotationgraphs  lima-lp-linguisticprocessing-core lima-lp-analysishandlers lima-lp-bagofwords lima-lp-propertycode lima-lp-linguisticdata lima-lp-misc lima-lp-easyxmldumper lima-lp-analysisdumpers lima-lp-applyrecognizer lima-lp-automaton lima-lp-linguisticprocessors lima-lp-linguisticresources lima-lp-morphologicanalysis lima-lp-postagger lima-lp-eventanalyzer lima-lp-specificentities lima-lp-syntacticanalysis lima-common-datahandler lima-common-fsaaccess ${Boost_LIBRARIES} lima-common-data lima-common-factory lima-common-misc lima-common-datahandler ${Qt5_LIBRARIES} lima-common-tools lima-common-mediaprocessors lima-common-mediaticdata lima-common-processunitframework lima-common-factory lima-common-xmlconfigurationfiles lima-lp-client lima-lp-linguisticanalysisstructure lima-lp-analysisdict lima-lp-textsegmentation lima-lp-annotationgraph lima-lp-flattokenizer lima-lp-regexmatcher ${optionalLibs})

set_target_properties(lima-lp-test-annotationgraphs PROPERTIES VERSION ${LIMA_LP_LIB_VERSION} SOVERSION ${LIMA_LP_LIB_SOVERSION})

install(TARGETS lima-lp-test-annotationgraphs DESTINATION ${LIB_INSTALL_DIR}
  COMPONENT runtime)


########### install files ###############
