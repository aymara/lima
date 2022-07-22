# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )


########### next target ###############
add_definitions( -DXMLREADERCLIENT_EXPORTING )

set(lima-xp-client_LIB_SRCS
    XmlReaderClientFactory.cpp
	AbstractXmlReaderClient.cpp
)

add_library(lima-xp-client SHARED ${lima-xp-client_LIB_SRCS})
ADD_DLL_TO_PATH(lima-xp-client)

target_link_libraries(lima-xp-client
  lima-common-mediaprocessors
  lima-common-mediaticdata
  lima-common-xmlconfigurationfiles
  lima-common-processunitframework
  lima-common-factory
  lima-common-tools
  lima-common-data
  ${QT_LIBRARIES}
)

set_target_properties(lima-xp-client PROPERTIES VERSION ${LIMA_LP_LIB_VERSION} SOVERSION ${LIMA_LP_LIB_SOVERSION})
install(TARGETS lima-xp-client DESTINATION ${LIB_INSTALL_DIR}
  COMPONENT runtime)


########### install files ###############
install(FILES
    AbstractXmlReaderClient.h
    XmlReaderClientFactory.h
    XmlReaderException.h
    XmlReaderClient_export.h
  DESTINATION include/xmlreader/client
  COMPONENT devel
)


