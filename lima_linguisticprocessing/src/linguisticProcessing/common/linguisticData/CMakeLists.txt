# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )

add_definitions(-DLIMA_LINGUISTICDATA_EXPORTING)

########### next target ###############

SET(lima-lp-linguisticdata_LIB_SRCS
   languageData.cpp
)

add_library(lima-lp-linguisticdata SHARED ${lima-lp-linguisticdata_LIB_SRCS})
ADD_DLL_TO_PATH(lima-lp-linguisticdata)


target_link_libraries(lima-lp-linguisticdata 
  lima-lp-propertycode 
)

set_target_properties(lima-lp-linguisticdata PROPERTIES VERSION ${LIMA_LP_LIB_VERSION} SOVERSION ${LIMA_LP_LIB_SOVERSION})

install(TARGETS lima-lp-linguisticdata DESTINATION ${LIB_INSTALL_DIR}
  COMPONENT runtime)


########### install files ###############
install(FILES
    languageData.h
    LimaStringText.h 
  DESTINATION include/linguisticProcessing/common/linguisticData
  COMPONENT devel
)
