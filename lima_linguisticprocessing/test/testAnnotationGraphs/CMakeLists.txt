# Copyright 2002-2019 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )

########### next target ###############

SET(lima-lp-test-annotationgraphs_LIB_SRCS
  AnnotationGraphTestProcessUnit.cpp
)

DECLARE_LIMA_PLUGIN(lima-lp-test-annotationgraphs)

target_link_libraries(lima-lp-test-annotationgraphs
  lima-lp-linguisticprocessors
  lima-lp-annotationgraph
  ${optionalLibs}
)

set_target_properties(lima-lp-test-annotationgraphs PROPERTIES VERSION ${LIMA_LP_LIB_VERSION} SOVERSION ${LIMA_LP_LIB_SOVERSION})

install(TARGETS lima-lp-test-annotationgraphs DESTINATION ${LIB_INSTALL_DIR}
  COMPONENT runtime)

########### install files ###############
