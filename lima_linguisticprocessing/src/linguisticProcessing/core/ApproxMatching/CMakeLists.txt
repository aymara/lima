#   Copyright 2002-2017 CEA LIST
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

add_definitions(-DLIMA_APPROXSTRINGMATCHING_EXPORTING)

########### next target ###############

SET(lima-lp-approxstringmatching_LIB_SRCS
   ApproxStringMatcher.cpp
   NameIndexResource.cpp
)

DECLARE_LIMA_PLUGIN(lima-lp-approxstringmatching)
ADD_DLL_TO_PATH(lima-lp-approxstringmatching)

target_link_libraries(lima-lp-approxstringmatching
    lima-common-factory
    lima-common-misc
    lima-common-data
    lima-common-mediaticdata
    lima-common-time
    lima-common-xmlconfigurationfiles
    lima-common-processunitframework
    lima-common-mediaprocessors
    lima-lp-linguisticprocessors
    lima-lp-linguisticresources
    lima-lp-annotationgraph
    lima-lp-linguisticanalysisstructure
    lima-lp-specificentities
    ${optionalLibs}
    ${Boost_LIBRARIES}
    ${Qt5_LIBRARIES}
)

set_target_properties(lima-lp-approxstringmatching PROPERTIES VERSION ${LIMA_LP_LIB_VERSION} SOVERSION ${LIMA_LP_LIB_SOVERSION})

install(TARGETS lima-lp-approxstringmatching DESTINATION ${LIB_INSTALL_DIR}
  COMPONENT runtime)

install(FILES gene-specyNames.sorted.toy.lst 
  DESTINATION share/apps/lima/resources/LinguisticProcessings/eng
  COMPONENT runtime)
########### install files ###############
