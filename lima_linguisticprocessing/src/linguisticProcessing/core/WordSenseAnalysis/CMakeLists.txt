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

add_definitions(-DLIMA_WORDSENSEANALYSIS_EXPORTING)

########### next target ###############
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra  -Wall -Wno-return-type -g  ")  
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Wall -g  ")  


SET(lima-lp-wordsensetagger_LIB_SRCS
  KnnSearcher.cpp
  WordUnit.cpp
  WordSenseXmlLogger.cpp
  WordSenseAnnotation.cpp
  WordSenseDisambiguator.cpp
  WordSenseUnit.cpp
  lshsignature.cpp
  hasheddb.cpp
)

DECLARE_LIMA_PLUGIN(lima-lp-wordsensetagger)

target_link_libraries(lima-lp-wordsensetagger
  lima-common
  lima-lp-linguisticprocessors
  lima-lp-linguisticresources
  lima-lp-annotationgraph
  lima-lp-linguisticanalysisstructure
  lima-lp-textsegmentation
  lima-lp-syntacticanalysis
  lima-lp-automaton
  lima-lp-applyrecognizer
)

set_target_properties(lima-lp-wordsensetagger PROPERTIES VERSION ${LIMA_LP_LIB_VERSION} SOVERSION ${LIMA_LP_LIB_SOVERSION})


########### install files ###############

install(TARGETS lima-lp-wordsensetagger DESTINATION lib)
