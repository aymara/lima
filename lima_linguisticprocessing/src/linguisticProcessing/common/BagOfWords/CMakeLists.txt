# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )

add_definitions(-DLIMA_BOW_EXPORTING)

########### next target ###############

SET(lima-lp-bagofwords_LIB_SRCS
   BinaryWriterBoWDocumentHandler.cpp
   bowBinaryReaderWriter.cpp
   bowComplexToken.cpp
   bowDocument.cpp
   bowDocumentST.cpp
   bowNamedEntity.cpp
   BoWPredicate.cpp
   BoWRelation.cpp
   bowTerm.cpp
   bowText.cpp
   bowToken.cpp
   bowTokenIterator.cpp
   bowXMLReader.cpp
   bowXMLWriter.cpp
   defaultIdGenerator.cpp
   indexElement.cpp
   indexElementIterator.cpp
   TextWriterBoWDocumentHandler.cpp
)

add_library(lima-lp-bagofwords SHARED ${lima-lp-bagofwords_LIB_SRCS})
ADD_DLL_TO_PATH(lima-lp-bagofwords)


target_link_libraries(lima-lp-bagofwords 
  lima-lp-linguisticdata
)

set_target_properties(lima-lp-bagofwords PROPERTIES VERSION ${LIMA_LP_LIB_VERSION} SOVERSION ${LIMA_LP_LIB_SOVERSION})

install(TARGETS lima-lp-bagofwords DESTINATION ${LIB_INSTALL_DIR}
  COMPONENT runtime)


########### install files ###############
install(FILES
    AbstractBoWElement.h
    AbstractBoWDocumentHandler.h
    bowDocument.h
    BoWPredicate.h
    bowTerm.h
    bowXMLReader.h
    indexElementIterator.h
    BinaryWriterBoWDocumentHandler.h
    bowDocumentST.h
    bowText.h
    bowXMLWriter.h
    TextWriterBoWDocumentHandler.h
    bowBinaryReaderWriter.h
    bowNamedEntity.h
    bowToken.h
#     defaultIdGenerator.h
    bowComplexToken.h
    BoWRelation.h
    bowTokenIterator.h
    indexElement.h
  DESTINATION include/linguisticProcessing/common/BagOfWords
  COMPONENT devel
)



add_subdirectory(tests)
