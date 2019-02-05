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

add_definitions(-DLIMA_POSTAGGER_EXPORTING)
########### next target ###############

SET(lima-lp-postagger_LIB_SRCS
   disambiguationLogger.cpp
   dotGraphWriter.cpp
   frequencyCost.cpp
   integerCost.cpp
   ngramMatrices.cpp
   posTaggingGraphWriter.cpp
   dummyPosTagger.cpp
   greedyPosTagger.cpp
   ViterbiPosTagger.cpp
   DisambiguatedGraphXmlLogger.cpp
)

set(lima-lp-postagger_LIB_LINK
   lima-common-factory lima-common-misc lima-common-data lima-common-fsaaccess lima-common-mediaticdata lima-common-time lima-common-factory lima-common-xmlconfigurationfiles lima-common-processunitframework lima-common-mediaprocessors lima-lp-linguisticprocessors lima-lp-linguisticresources lima-lp-annotationgraph lima-lp-linguisticanalysisstructure  ${Boost_LIBRARIES}
)

if(SVMTOOL++_FOUND)
  set(lima-lp-postagger_LIB_SRCS
    ${lima-lp-postagger_LIB_SRCS}
    SvmToolPosTagger.cpp
    DynamicSvmToolPosTagger.cpp
  )
  set(lima-lp-postagger_LIB_LINK
    ${lima-lp-postagger_LIB_LINK}
    svmtool
  )
endif()


DECLARE_LIMA_PLUGIN(lima-lp-postagger)

target_link_libraries(lima-lp-postagger  ${lima-lp-postagger_LIB_LINK})


set_target_properties(lima-lp-postagger PROPERTIES VERSION ${LIMA_LP_LIB_VERSION} SOVERSION ${LIMA_LP_LIB_SOVERSION})

install(TARGETS lima-lp-postagger DESTINATION ${LIB_INSTALL_DIR}
  COMPONENT runtime)


########### install files ###############
