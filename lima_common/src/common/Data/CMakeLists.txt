# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )

add_subdirectory(tests)

add_definitions( -DLIMA_DATA_EXPORTING )

########### next target ###############

SET(lima-common-data_LIB_SRCS
   DataTypes.cpp
   genericDocumentProperties.cpp
   readwritetools.cpp
   strwstrtools.cpp
   LimaString.cpp
)

add_library(lima-common-data SHARED ${lima-common-data_LIB_SRCS})

ADD_DLL_TO_PATH(lima-common-data)

target_link_libraries(lima-common-data
  lima-common-tools
)

set_target_properties(lima-common-data PROPERTIES
  VERSION ${LIMA_COMMON_LIB_VERSION}
  SOVERSION ${LIMA_COMMON_LIB_SOVERSION})
install(TARGETS lima-common-data DESTINATION ${LIB_INSTALL_DIR}
    COMPONENT runtime)


########### install files ###############
install(FILES
    base64.h
    DataTypes.h
    genericDocumentProperties.h
    LimaString.h
    readwritetools.h
    strwstrtools.h
  DESTINATION include/common/Data
  COMPONENT devel
)
