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
include_directories(${CMAKE_SOURCE_DIR}/src )

########### next target ###############

SET(XmlParser_SRCS
   preprocessCorpus.cpp
)

add_executable(XmlParser ${XmlParser_SRCS})

target_link_libraries(XmlParser ${Boost_LIBRARIES}  ${QT_LIBRARIES} lima-common-qslog lima-common-time lima-common-data ${optionalLibs})

install(TARGETS XmlParser DESTINATION bin)


########### next target ###############

SET(evalCoref_SRCS
   evalCoref.cpp
)

add_executable(evalCoref ${evalCoref_SRCS})

target_link_libraries(evalCoref lima-lp-analysisdict lima-lp-linguisticanalysisstructure lima-lp-annotationgraph lima-lp-bagofwords lima-lp-propertycode lima-lp-linguisticdata lima-lp-misc lima-lp-analysisdumpers lima-lp-applyrecognizer lima-lp-automaton lima-lp-linguisticprocessors lima-lp-linguisticresources lima-lp-morphologicanalysis lima-lp-postagger lima-lp-specificentities lima-lp-syntacticanalysis lima-lp-flattokenizer lima-common-fsaaccess lima-common-factory lima-common-time lima-common-misc ${Boost_LIBRARIES} lima-common-data  ${QT_LIBRARIES} lima-common-qslog lima-common-mediaprocessors lima-common-mediaticdata lima-common-processunitframework lima-lp-textsegmentation lima-common-factory lima-common-xmlconfigurationfiles ${optionalLibs})

install(TARGETS evalCoref DESTINATION bin)


########### install files ###############
