# Copyright 2002-2019 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )

add_subdirectory(AnalysisHandlers)
add_subdirectory(xmlreader)

add_definitions(-DLIMA_LINGUISTICPROCESSIONGCLIENT_EXPORTING)

########### next target ###############

SET(lima-lp-client_LIB_SRCS
    LinguisticProcessingClientFactory.cpp
)

add_library(lima-lp-client SHARED ${lima-lp-client_LIB_SRCS})
ADD_DLL_TO_PATH(lima-lp-client)


target_link_libraries(lima-lp-client
  lima-common-factory
)

set_target_properties(lima-lp-client PROPERTIES VERSION ${LIMA_LP_LIB_VERSION} SOVERSION ${LIMA_LP_LIB_SOVERSION})

install(TARGETS lima-lp-client DESTINATION ${LIB_INSTALL_DIR}
  COMPONENT runtime)


########### install files ###############
install(FILES
    LinguisticProcessingClientExport.h
    AbstractLinguisticProcessingClient.h
    LinguisticProcessingException.h
    LinguisticProcessingClientFactory.h
  DESTINATION include/linguisticProcessing/client
  COMPONENT devel
)
