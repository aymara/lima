# Copyright 2002-2021 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )

add_definitions(-DLIMA_LP_HELPERS_EXPORTING)

########### next target ###############

SET(lima-lp-helpers_LIB_SRCS
  ConfigurationHelper.cpp
  DeepTokenizerBase.cpp
)

add_library(lima-lp-helpers
  SHARED ${lima-lp-helpers_LIB_SRCS}
)
ADD_DLL_TO_PATH(lima-lp-helpers)

target_link_libraries(lima-lp-helpers
  lima-common-xmlconfigurationfiles
  lima-common-mediaticdata
  lima-lp-linguisticanalysisstructure
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
  DeepTokenizerBase.h
  LangCodeHelpers.h
  DESTINATION include/linguisticProcessing/common/helpers
  COMPONENT devel
)
