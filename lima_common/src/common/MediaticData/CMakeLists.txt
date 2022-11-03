# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )

# add_subdirectory(tests)

add_definitions( -DLIMA_MEDIATICDATA_EXPORTING )
########### next target ###############

SET(lima-common-mediaticdata_LIB_SRCS
   EntityType.cpp
   mediaData.cpp
   mediaticData.cpp
)

add_library(lima-common-mediaticdata SHARED ${lima-common-mediaticdata_LIB_SRCS})
ADD_DLL_TO_PATH(lima-common-mediaticdata)

target_link_libraries(lima-common-mediaticdata 
  lima-common-fsaaccess 
  lima-common-factory 
  lima-common-xmlconfigurationfiles 
)

set_target_properties(lima-common-mediaticdata PROPERTIES VERSION ${LIMA_COMMON_LIB_VERSION} SOVERSION ${LIMA_COMMON_LIB_SOVERSION})
install(TARGETS lima-common-mediaticdata DESTINATION ${LIB_INSTALL_DIR}
    COMPONENT runtime)


########### install files ###############
install(FILES
    EntityType.h
    mediaData.h
    mediaticData.h
  DESTINATION include/common/MediaticData
  COMPONENT devel
)
