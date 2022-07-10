# Copyright 2002-2019 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )

add_definitions(-DLIMA_EASYXMLDUMPER_EXPORTING)


########### next target ###############

SET(lima-lp-easyxmldumper_LIB_SRCS
   ConstituantAndRelationExtractor.cpp
   EasyDumper.cpp
   easyXmlDumper.cpp
   forme.cpp
)

DECLARE_LIMA_PLUGIN(lima-lp-easyxmldumper)

target_link_libraries(lima-lp-easyxmldumper
   lima-lp-linguisticprocessors
   lima-lp-syntacticanalysis
   lima-lp-morphologicanalysis
)

set_target_properties(lima-lp-easyxmldumper PROPERTIES VERSION ${LIMA_LP_LIB_VERSION} SOVERSION ${LIMA_LP_LIB_SOVERSION})

install(TARGETS lima-lp-easyxmldumper DESTINATION ${LIB_INSTALL_DIR}
  COMPONENT runtime)


########### install files ###############
