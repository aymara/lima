#   Copyright 2002-2020 CEA LIST
#
#   This file is part of LIMA.
#
#   LIMA is free software: you can redistribute it and/or modify
#   it under the terms of the GNU Affero General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   LIMA is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU Affero General Public License for more details.
#
#   You should have received a copy of the GNU Affero General Public License
#   along with LIMA.  If not, see <http://www.gnu.org/licenses/>
include_directories(BEFORE ${CMAKE_SOURCE_DIR}/. ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/src/common )


########### next target ###############
SET(testTgv_SRCS
   testlibTgv.cpp
)

add_executable(testTgv ${testTgv_SRCS})

target_link_libraries(testTgv
  lima-common-xmlconfigurationfiles
  lima-lp-tgv
  ${optionalLibs}
)

install(TARGETS testTgv DESTINATION bin)

########### next target ###############

SET(catBowFiles_SRCS
   catBowFiles.cpp
)

add_executable(catBowFiles ${catBowFiles_SRCS})

target_link_libraries(catBowFiles
  lima-lp-bagofwords
  ${optionalLibs}
)

install(TARGETS catBowFiles DESTINATION bin
  COMPONENT runtime)

########### next target ###############

SET(readBowFile_SRCS
   readBoWFile.cpp
)

add_executable(readBowFile ${readBowFile_SRCS})

target_link_libraries(readBowFile
  lima-lp-bagofwords
  ${optionalLibs}
)

install(TARGETS readBowFile DESTINATION bin
  COMPONENT runtime)

########### next target ###############

SET(readLtrFile_SRCS
   readLtrFile.cpp
)

add_executable(readLtrFile ${readLtrFile_SRCS})

target_link_libraries(readLtrFile
  lima-lp-lineartextrepresentation
  Boost::program_options
  ${optionalLibs}
)

install(TARGETS readLtrFile DESTINATION bin
  COMPONENT runtime)

########### next target ###############

SET(getLexiconFromBoW_SRCS
   getLexiconFromBoW.cpp
)

add_executable(getLexiconFromBoW ${getLexiconFromBoW_SRCS})

target_link_libraries(getLexiconFromBoW
  lima-lp-bagofwords
  ${optionalLibs}
)

install(TARGETS getLexiconFromBoW DESTINATION bin
  COMPONENT runtime)

########### next target ###############

SET(parseXMLPropertyFile_SRCS
   parseXMLPropertyFile.cpp
)

add_executable(parseXMLPropertyFile ${parseXMLPropertyFile_SRCS})

target_link_libraries(parseXMLPropertyFile
  lima-lp-propertycode
  ${optionalLibs}
)

install(TARGETS parseXMLPropertyFile DESTINATION bin
  COMPONENT runtime)

########### next target ###############

SET(convertSymbolicCodes_SRCS
   convertSymbolicCodes.cpp
)

add_executable(convertSymbolicCodes ${convertSymbolicCodes_SRCS})

target_link_libraries(convertSymbolicCodes
  lima-common
  lima-common-factory
  lima-lp-propertycode
  ${optionalLibs}
)

install(TARGETS convertSymbolicCodes DESTINATION bin
  COMPONENT runtime)

########### next target ###############

SET(readLinguisticData_SRCS
   readLinguisticData.cpp
)

add_executable(readLinguisticData ${readLinguisticData_SRCS})

target_link_libraries(readLinguisticData
  lima-common-factory
  lima-common-mediaticdata
  ${optionalLibs}
)

install(TARGETS readLinguisticData DESTINATION bin
  COMPONENT runtime)


########### next target ###############

SET(testContentDict16_SRCS
   testContentDict16.cpp
)

add_executable(testContentDict16 ${testContentDict16_SRCS})

target_link_libraries(testContentDict16
  lima-common-factory
  lima-common-fsaaccess
  ${optionalLibs}
)

install(TARGETS testContentDict16 DESTINATION bin
  COMPONENT runtime)


########### next target ###############

SET(convertXmlToSBoW_SRCS
   convertXmlToSBoW.cpp
)

add_executable(convertXmlToSBoW ${convertXmlToSBoW_SRCS})

target_link_libraries(convertXmlToSBoW
  lima-lp-bagofwords
  ${optionalLibs}
)

install(TARGETS convertXmlToSBoW DESTINATION bin
  COMPONENT runtime)

########### next target ###############

SET(buildCompoundsDict_SRCS
   buildCompoundsDict.cpp
)

add_executable(buildCompoundsDict ${buildCompoundsDict_SRCS})

target_link_libraries(buildCompoundsDict
  lima-common-factory
  lima-common-fsaaccess
  Boost::program_options
  ${optionalLibs}
)

install(TARGETS buildCompoundsDict DESTINATION bin
  COMPONENT runtime)

########### next target ###############

SET(testReadLexicon_SRCS
   testReadLexicon.cpp
)

add_executable(testReadLexicon ${testReadLexicon_SRCS})

target_link_libraries(testReadLexicon
  lima-common-factory
  lima-common-fsaaccess
  ${optionalLibs}
)

install(TARGETS testReadLexicon DESTINATION bin
  COMPONENT runtime)

########### next target ###############

SET(testPropManager_SRCS
   testPropManager.cpp
)

add_executable(testPropManager ${testPropManager_SRCS})

target_link_libraries(testPropManager
  lima-common-mediaticdata
  lima-lp-propertycode
  lima-lp-linguisticdata
  ${optionalLibs}
)

install(TARGETS testPropManager DESTINATION bin
  COMPONENT runtime)

########### install files ###############
