# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

include_directories(${CMAKE_SOURCE_DIR}/src )


########### next target ###############

SET(lima-xp-xmldocumentparser_LIB_SRCS
   XmlDocumentParser.cpp
)

add_library(lima-xp-xmldocumentparser SHARED ${lima-xp-xmldocumentparser_LIB_SRCS})
ADD_DLL_TO_PATH(lima-xp-xmldocumentparser)

target_link_libraries(lima-xp-xmldocumentparser)

set_target_properties(lima-xp-xmldocumentparser PROPERTIES VERSION LIMA_LP_LIB_VERSION SOVERSION ${LIMA_LP_LIB_SOVERSION})
install(TARGETS lima-xp-xmldocumentparser DESTINATION ${LIB_INSTALL_DIR} COMPONENT runtime)


########### install files ###############



