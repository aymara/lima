# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src)

add_definitions(-DLIMA_LINEARTEXTREPRESENTATION_EXPORTING)
########### next target ###############

set(lima-lp-lineartextrepresentation_LIB_SRCS
  ltrToken.cpp
  ltrText.cpp
)

add_library(lima-lp-lineartextrepresentation SHARED ${lima-lp-lineartextrepresentation_LIB_SRCS})
ADD_DLL_TO_PATH(lima-lp-lineartextrepresentation)


target_link_libraries(lima-lp-lineartextrepresentation lima-lp-bagofwords)

set_target_properties(lima-lp-lineartextrepresentation PROPERTIES VERSION ${LIMA_LP_LIB_VERSION} SOVERSION ${LIMA_LP_LIB_SOVERSION})

install(TARGETS lima-lp-lineartextrepresentation DESTINATION ${LIB_INSTALL_DIR}
  COMPONENT runtime)


########### install files ###############
install(
  FILES
    ltrToken.h 
    ltrText.h 
  DESTINATION include/linguisticProcessing/common/linearTextRepresentation
  COMPONENT devel
)
