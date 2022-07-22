# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )


########### next target ###############
add_definitions( -DDOCUMENTSREADER_EXPORTING )

set(lima-xp-documentsreader_LIB_SRCS
   DocumentElements.cpp
   contentDocument.cpp
   documentReader.cpp
   documentReaderResources.cpp
   documentProperty.cpp
   #documentXMLParser.cpp
   structuredDocumentXMLParser.cpp
)

add_library(lima-xp-documentsreader SHARED ${lima-xp-documentsreader_LIB_SRCS})
ADD_DLL_TO_PATH(lima-xp-documentsreader)

target_link_libraries(lima-xp-documentsreader
  lima-common-mediaprocessors
  lima-common-mediaticdata
  lima-common-xmlconfigurationfiles
  lima-common-processunitframework
  lima-common-factory
  lima-common-tools
  lima-common-data
  lima-common-misc
  lima-common-fsaaccess
  lima-common-mediaticdata
  lima-common-time
  lima-lp-linguisticprocessors
  lima-lp-linguisticresources
  lima-lp-annotationgraph
  lima-lp-syntacticanalysis
  lima-lp-bagofwords
  lima-lp-linguisticanalysisstructure
  lima-lp-lineartextrepresentation
  lima-lp-textsegmentation
  lima-xp-qtsgmlentities
  ${QT_LIBRARIES}
)

set_target_properties(lima-xp-documentsreader
  PROPERTIES
    VERSION ${LIMA_LP_LIB_VERSION}
    SOVERSION ${LIMA_LP_LIB_SOVERSION}
)
install(TARGETS lima-xp-documentsreader DESTINATION ${LIB_INSTALL_DIR}
        COMPONENT runtime)


########### install files ###############
install(FILES
  documentReaderResources.h
  documentReader.h
  contentDocument.h
  structuredDocumentXMLParser.h
  tokensstream.h
  documentReaderException.h
  documentProperty.h
DESTINATION include/xmlreader/lima-xp-documentsreader
  COMPONENT devel
)


