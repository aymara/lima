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

SET(normalizeTerm_SRCS
   normalizeTerm.cpp
)

add_executable(normalizeTerm ${normalizeTerm_SRCS})

target_link_libraries(normalizeTerm lima-lp-client lima-lp-linguisticprocessing-core lima-common-datahandler lima-lp-analysisdict lima-lp-linguisticanalysisstructure lima-lp-annotationgraph lima-lp-analysishandlers lima-lp-bagofwords lima-lp-propertycode lima-lp-linguisticdata lima-lp-misc lima-lp-analysisdumpers lima-lp-applyrecognizer lima-lp-automaton lima-lp-linguisticprocessors lima-lp-linguisticresources lima-lp-morphologicanalysis lima-lp-postagger lima-lp-specificentities lima-lp-syntacticanalysis lima-lp-flattokenizer lima-common-fsaaccess lima-common-factory lima-common-misc lima-common-tools ${Boost_LIBRARIES} lima-common-data  ${QT_LIBRARIES} lima-common-qslog lima-common-mediaprocessors lima-common-mediaticdata lima-common-processunitframework lima-lp-textsegmentation lima-common-factory lima-common-xmlconfigurationfiles ${optionalLibs})

install(TARGETS normalizeTerm DESTINATION bin)


########### next target ###############

SET(desaccent_SRCS
   desaccent.cpp
)

add_executable(desaccent ${desaccent_SRCS})

target_link_libraries(desaccent lima-lp-client lima-lp-linguisticprocessing-core lima-common-datahandler  lima-lp-analysisdict lima-lp-linguisticanalysisstructure lima-lp-annotationgraph lima-lp-analysishandlers lima-lp-bagofwords lima-lp-propertycode lima-lp-linguisticdata lima-lp-misc lima-lp-analysisdumpers lima-lp-applyrecognizer lima-lp-automaton lima-lp-linguisticprocessors lima-lp-linguisticresources lima-lp-morphologicanalysis lima-lp-postagger lima-lp-specificentities lima-lp-syntacticanalysis lima-lp-flattokenizer lima-common-fsaaccess lima-common-factory lima-common-misc lima-common-tools ${Boost_LIBRARIES} lima-common-data  ${QT_LIBRARIES} lima-common-qslog lima-common-mediaprocessors lima-common-mediaticdata lima-common-processunitframework lima-lp-textsegmentation lima-common-factory lima-common-xmlconfigurationfiles ${optionalLibs})

install(TARGETS desaccent DESTINATION bin)


########### install files ###############
