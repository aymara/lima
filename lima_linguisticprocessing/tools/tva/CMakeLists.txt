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
include_directories(BEFORE ${CMAKE_SOURCE_DIR}/. ${CMAKE_SOURCE_DIR}/src )

########### next target ###############

SET(tva_SRCS
   tva.cpp
   AnalysisTestCase.cpp
)

add_executable(tva ${tva_SRCS})

target_link_libraries(tva lima-lp-client lima-lp-linguisticprocessing-core lima-lp-analysisdict lima-common-datahandler lima-lp-annotationgraph lima-lp-linguisticanalysisstructure  lima-lp-analysishandlers lima-lp-bagofwords lima-lp-propertycode lima-lp-linguisticdata lima-lp-misc lima-lp-tgv lima-lp-analysisdumpers lima-lp-applyrecognizer lima-lp-automaton lima-lp-linguisticprocessors lima-lp-linguisticresources lima-lp-morphologicanalysis lima-lp-postagger lima-lp-specificentities lima-lp-syntacticanalysis lima-lp-flattokenizer lima-common-fsaaccess lima-common-factory lima-common-misc ${Boost_LIBRARIES} lima-common-data  ${QT_LIBRARIES} lima-common-qslog lima-common-mediaprocessors lima-common-mediaticdata lima-common-processunitframework lima-lp-propertycode ${LIBXML2_LIBRARIES} lima-common-factory lima-common-xmlconfigurationfiles lima-lp-easyxmldumper lima-lp-eventanalyzer lima-lp-textsegmentation ${optionalLibs}
)

install(TARGETS tva DESTINATION bin)


########### install files ###############
