# Copyright 2002-2019 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )

add_definitions( -DLIMA_XMLCONFIGURATIONFILES_EXPORTING )
########### next target ###############

SET(lima-common-xmlconfigurationfiles_LIB_SRCS
   configurationStructure.cpp
   groupConfigurationStructure.cpp
   moduleConfigurationStructure.cpp
   xmlConfigurationFileHandler.cpp
   xmlConfigurationFileParser.cpp
   itemWithAttributes.cpp
)

add_library(lima-common-xmlconfigurationfiles SHARED ${lima-common-xmlconfigurationfiles_LIB_SRCS})

ADD_DLL_TO_PATH(lima-common-xmlconfigurationfiles)

target_link_libraries(lima-common-xmlconfigurationfiles
   lima-common-data
   Qt5::Xml
)

set_target_properties(lima-common-xmlconfigurationfiles PROPERTIES VERSION ${LIMA_COMMON_LIB_VERSION} SOVERSION ${LIMA_COMMON_LIB_SOVERSION})
install(TARGETS lima-common-xmlconfigurationfiles DESTINATION ${LIB_INSTALL_DIR} COMPONENT runtime)

########### install files ###############
install(FILES
#     xmlConfigurationFileHandler.h
    configurationStructure.h
    xmlConfigurationFileParser.h
    groupConfigurationStructure.h
    xmlConfigurationFileExceptions.h
    moduleConfigurationStructure.h
    itemWithAttributes.h
  DESTINATION include/common/XMLConfigurationFiles
  COMPONENT devel
)

add_subdirectory(tests)
