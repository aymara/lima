# Copyright 2002-2019 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )

add_definitions(-DLIMA_REGEXMATCHER_EXPORTING)
########### next target ###############

SET(lima-lp-regexmatcher_LIB_SRCS
  RegexMatcher.cpp
)

DECLARE_LIMA_PLUGIN(lima-lp-regexmatcher)
target_link_libraries(lima-lp-regexmatcher
   lima-common-fsaaccess
   lima-common-time
   lima-common-processunitframework
   lima-common-xmlconfigurationfiles
   lima-lp-linguisticanalysisstructure
)

set_target_properties(lima-lp-regexmatcher PROPERTIES VERSION ${LIMA_LP_LIB_VERSION} SOVERSION ${LIMA_LP_LIB_SOVERSION})

install(TARGETS lima-lp-regexmatcher DESTINATION ${LIB_INSTALL_DIR}
  COMPONENT runtime)

########### install files ###############
