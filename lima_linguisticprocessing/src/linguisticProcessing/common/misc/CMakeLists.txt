# Copyright 2002-2019 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )

add_definitions(-DLIMA_LPMISC_EXPORTING)

########### next target ###############

SET(lima-lp-misc_LIB_SRCS
#   textFormater.cpp
  AbstractLinguisticLogger.cpp
  positionLengthList.cpp
)

add_library(lima-lp-misc SHARED ${lima-lp-misc_LIB_SRCS})
ADD_DLL_TO_PATH(lima-lp-misc)


target_link_libraries(lima-lp-misc
  lima-common-factory
  lima-common-xmlconfigurationfiles
  lima-common-mediaprocessors
)

set_target_properties(lima-lp-misc PROPERTIES VERSION ${LIMA_LP_LIB_VERSION} SOVERSION ${LIMA_LP_LIB_SOVERSION})

install(TARGETS lima-lp-misc DESTINATION ${LIB_INSTALL_DIR}
  COMPONENT runtime)


########### install files ###############
install(FILES
    AbstractLinguisticLogger.h
    positionLengthList.h
  DESTINATION include/linguisticProcessing/common/misc
  COMPONENT devel
)
