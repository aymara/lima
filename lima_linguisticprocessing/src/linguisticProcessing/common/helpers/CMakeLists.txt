#   Copyright 2002-2020 CEA LIST
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

add_definitions(-DLIMA_LP_HELPERS_EXPORTING)

########### next target ###############

SET(lima-lp-helpers_LIB_SRCS
  ConfigurationHelper.cpp
)

add_library(lima-lp-helpers
  SHARED ${lima-lp-helpers_LIB_SRCS}
)
ADD_DLL_TO_PATH(lima-lp-helpers)

target_link_libraries(lima-lp-helpers
  lima-common-xmlconfigurationfiles
  lima-common-mediaticdata
)

set_target_properties(lima-lp-helpers
  PROPERTIES
  VERSION ${LIMA_LP_LIB_VERSION}
  SOVERSION ${LIMA_LP_LIB_SOVERSION}
)

install(
  TARGETS lima-lp-helpers
  DESTINATION ${LIB_INSTALL_DIR}
  COMPONENT runtime
)

########### install files ###############

install(FILES
  ConfigurationHelper.h
  DESTINATION include/linguisticProcessing/common/helpers
  COMPONENT devel
)
