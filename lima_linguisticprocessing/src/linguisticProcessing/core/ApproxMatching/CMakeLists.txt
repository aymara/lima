# Copyright 2002-2019 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )

add_definitions(-DLIMA_APPROXSTRINGMATCHING_EXPORTING)

########### next target ###############

SET(lima-lp-approxstringmatching_LIB_SRCS
   ApproxStringMatcher.cpp
   NameIndexResource.cpp
)

DECLARE_LIMA_PLUGIN(lima-lp-approxstringmatching)

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
)

set_target_properties(lima-lp-approxstringmatching PROPERTIES VERSION ${LIMA_LP_LIB_VERSION} SOVERSION ${LIMA_LP_LIB_SOVERSION})

install(TARGETS lima-lp-approxstringmatching DESTINATION ${LIB_INSTALL_DIR}
  COMPONENT runtime)

install(FILES gene-specyNames.sorted.toy.lst
  DESTINATION share/apps/lima/resources/LinguisticProcessings/eng
  COMPONENT runtime)
########### install files ###############
