#   Copyright 2002-2019 CEA LIST
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

target_link_libraries(lima-lp-test-annotationgraphs
  lima-lp-linguisticprocessors
  lima-lp-annotationgraph
  ${optionalLibs}
)

set_target_properties(lima-lp-test-annotationgraphs PROPERTIES VERSION ${LIMA_LP_LIB_VERSION} SOVERSION ${LIMA_LP_LIB_SOVERSION})

install(TARGETS lima-lp-test-annotationgraphs DESTINATION ${LIB_INSTALL_DIR}
  COMPONENT runtime)

########### install files ###############
