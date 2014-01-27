#   Copyright 2002-2013 CEA LIST
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

target_link_libraries(testTgv lima-lp-tgv lima-lp-linguisticprocessing-core lima-lp-analysishandlers lima-lp-bagofwords lima-lp-propertycode lima-lp-linguisticdata lima-lp-misc lima-lp-easyxmldumper lima-lp-analysisdumpers lima-lp-applyrecognizer lima-lp-automaton lima-lp-linguisticprocessors lima-lp-linguisticresources lima-lp-morphologicanalysis lima-lp-postagger lima-lp-eventanalyzer lima-lp-specificentities lima-lp-syntacticanalysis lima-common-datahandler lima-common-fsaaccess ${Boost_LIBRARIES} lima-common-data lima-common-factory lima-common-misc lima-common-datahandler  ${QT_LIBRARIES} lima-common-qslog lima-common-mediaprocessors lima-common-mediaticdata lima-common-processunitframework lima-common-factory lima-common-xmlconfigurationfiles lima-lp-client lima-lp-linguisticanalysisstructure lima-lp-analysisdict lima-lp-corefsolver lima-lp-annotationgraph lima-lp-flattokenizer ${optionalLibs} ${LIBXML2_LIBRARIES})

install(TARGETS testTgv DESTINATION bin)

########### next target ###############

SET(catBowFiles_SRCS
   catBowFiles.cpp
)

add_executable(catBowFiles ${catBowFiles_SRCS})

target_link_libraries(catBowFiles lima-lp-bagofwords lima-common-factory lima-common-misc lima-lp-misc lima-common-data lima-common-fsaaccess lima-common-mediaticdata  lima-lp-linguisticdata lima-common-factory lima-common-xmlconfigurationfiles lima-lp-propertycode ${QT_LIBRARIES} lima-common-qslog ${Boost_LIBRARIES} lima-common-mediaticdata    ${optionalLibs})

install(TARGETS catBowFiles DESTINATION bin)

########### next target ###############

SET(readBowFile_SRCS
   readBoWFile.cpp
)

add_executable(readBowFile ${readBowFile_SRCS})

target_link_libraries(readBowFile lima-lp-linguisticprocessing-core lima-lp-analysishandlers lima-lp-bagofwords lima-lp-propertycode lima-lp-linguisticdata lima-lp-misc lima-lp-easyxmldumper lima-lp-analysisdumpers lima-lp-applyrecognizer lima-lp-automaton lima-lp-linguisticprocessors lima-lp-linguisticresources lima-lp-morphologicanalysis lima-lp-postagger lima-lp-eventanalyzer lima-lp-specificentities lima-lp-syntacticanalysis lima-common-datahandler lima-common-fsaaccess ${Boost_LIBRARIES} lima-common-data lima-common-factory lima-common-misc lima-common-datahandler  ${QT_LIBRARIES} lima-common-qslog lima-common-mediaprocessors lima-common-mediaticdata lima-common-processunitframework  ${QT_LIBRARIES} lima-common-qslog lima-common-factory lima-common-xmlconfigurationfiles lima-lp-client lima-lp-linguisticanalysisstructure lima-lp-analysisdict lima-lp-corefsolver lima-lp-annotationgraph lima-lp-flattokenizer ${optionalLibs})

install(TARGETS readBowFile DESTINATION bin)

########### next target ###############

SET(readLtrFile_SRCS
   readLtrFile.cpp
)

add_executable(readLtrFile ${readLtrFile_SRCS})

target_link_libraries(readLtrFile lima-lp-lineartextrepresentation lima-lp-linguisticprocessing-core lima-lp-analysishandlers lima-lp-bagofwords lima-lp-propertycode lima-lp-linguisticdata lima-lp-misc lima-lp-easyxmldumper lima-lp-analysisdumpers lima-lp-applyrecognizer lima-lp-automaton lima-lp-linguisticprocessors lima-lp-linguisticresources lima-lp-morphologicanalysis lima-lp-postagger lima-lp-eventanalyzer lima-lp-specificentities lima-lp-syntacticanalysis lima-common-datahandler lima-common-fsaaccess ${Boost_LIBRARIES} lima-common-data lima-common-factory lima-common-misc lima-common-datahandler  ${QT_LIBRARIES} lima-common-qslog lima-common-mediaprocessors lima-common-mediaticdata lima-common-processunitframework ${QT_LIBRARIES} lima-common-qslog lima-common-factory lima-common-xmlconfigurationfiles lima-lp-client lima-lp-linguisticanalysisstructure lima-lp-analysisdict lima-lp-corefsolver lima-lp-annotationgraph lima-lp-flattokenizer  ${optionalLibs})

install(TARGETS readLtrFile DESTINATION bin)

########### next target ###############

SET(getLexiconFromBoW_SRCS
   getLexiconFromBoW.cpp
)

add_executable(getLexiconFromBoW ${getLexiconFromBoW_SRCS})

target_link_libraries(getLexiconFromBoW lima-lp-bagofwords lima-common-factory lima-common-misc lima-lp-misc lima-common-data lima-common-fsaaccess lima-common-mediaticdata lima-lp-linguisticdata lima-common-factory lima-common-xmlconfigurationfiles lima-lp-propertycode  ${QT_LIBRARIES} lima-common-qslog ${Boost_LIBRARIES}  ${optionalLibs})

install(TARGETS getLexiconFromBoW DESTINATION bin)

########### next target ###############

SET(convertToUtf8_SRCS
   convertToUtf8.cpp
)

add_executable(convertToUtf8 ${convertToUtf8_SRCS})

target_link_libraries(convertToUtf8 lima-common-factory lima-common-misc lima-lp-misc lima-common-data ${Boost_LIBRARIES}  ${QT_LIBRARIES} lima-common-qslog ${optionalLibs})

install(TARGETS convertToUtf8 DESTINATION bin)

########### next target ###############

SET(icu_wstring2nstring_SRCS
   icu_wstring2nstring.cpp
)

add_executable(icu_wstring2nstring ${icu_wstring2nstring_SRCS})

target_link_libraries(icu_wstring2nstring lima-common-factory lima-common-misc lima-lp-misc lima-common-data ${Boost_LIBRARIES}  ${QT_LIBRARIES} lima-common-qslog ${optionalLibs})

install(TARGETS icu_wstring2nstring DESTINATION bin)

########### next target ###############

SET(testAbstractFactoryPattern_SRCS
   testAbstractFactoryPattern.cpp
)

add_executable(testAbstractFactoryPattern ${testAbstractFactoryPattern_SRCS})

target_link_libraries(testAbstractFactoryPattern lima-common-factory lima-common-factory lima-common-xmlconfigurationfiles lima-common-factory lima-common-misc
#   lima-lp-misc
  lima-common-data ${Boost_LIBRARIES}  ${QT_LIBRARIES} lima-common-qslog  ${optionalLibs}
  )

install(TARGETS testAbstractFactoryPattern DESTINATION bin)


########### next target ###############

SET(testAccessMethod_SRCS
   testAccessMethod.cpp
)

add_executable(testAccessMethod ${testAccessMethod_SRCS})

target_link_libraries(testAccessMethod lima-common-factory lima-common-misc
#   lima-lp-misc
  lima-common-data
  lima-common-fsaaccess ${Boost_LIBRARIES}  ${QT_LIBRARIES} lima-common-qslog  ${optionalLibs})

install(TARGETS testAccessMethod DESTINATION bin)

########### next target ###############

SET(parseXMLPropertyFile_SRCS
   parseXMLPropertyFile.cpp
)

add_executable(parseXMLPropertyFile ${parseXMLPropertyFile_SRCS})

target_link_libraries(parseXMLPropertyFile lima-common-factory lima-common-misc
#   lima-lp-misc
  lima-common-data lima-lp-propertycode ${Boost_LIBRARIES}  ${QT_LIBRARIES} lima-common-qslog  ${optionalLibs})

install(TARGETS parseXMLPropertyFile DESTINATION bin)

########### next target ###############

SET(convertSymbolicCodes_SRCS
   convertSymbolicCodes.cpp
)

add_executable(convertSymbolicCodes ${convertSymbolicCodes_SRCS})

target_link_libraries(convertSymbolicCodes lima-common-factory lima-common-misc lima-lp-misc lima-common-data lima-lp-propertycode ${Boost_LIBRARIES}  ${QT_LIBRARIES} lima-common-qslog  ${optionalLibs})

install(TARGETS convertSymbolicCodes DESTINATION bin)

########### next target ###############

SET(testProcessUnitFramework_SRCS
   testProcessUnitFramework.cpp
)

add_executable(testProcessUnitFramework ${testProcessUnitFramework_SRCS})

target_link_libraries(testProcessUnitFramework lima-common-factory lima-common-factory lima-common-xmlconfigurationfiles lima-common-factory lima-common-misc
#   lima-lp-misc
  lima-common-data lima-common-processunitframework  ${QT_LIBRARIES} lima-common-qslog ${Boost_LIBRARIES}  ${optionalLibs})

install(TARGETS testProcessUnitFramework DESTINATION bin)

########### next target ###############

SET(readLinguisticData_SRCS
   readLinguisticData.cpp
)

add_executable(readLinguisticData ${readLinguisticData_SRCS})

target_link_libraries(readLinguisticData lima-lp-linguisticdata lima-common-mediaticdata lima-common-factory lima-common-xmlconfigurationfiles lima-lp-propertycode lima-common-factory lima-common-misc lima-lp-misc lima-common-data ${Boost_LIBRARIES}  ${QT_LIBRARIES} lima-common-qslog ${optionalLibs})

install(TARGETS readLinguisticData DESTINATION bin)

########### next target ###############

SET(parseXMLFile_SRCS
   parseXMLFile.cpp
)

add_executable(parseXMLFile ${parseXMLFile_SRCS})

target_link_libraries(parseXMLFile lima-common-factory lima-common-xmlconfigurationfiles lima-common-factory lima-common-misc
#   lima-lp-misc
  lima-common-data ${Boost_LIBRARIES}  ${QT_LIBRARIES} lima-common-qslog ${optionalLibs})

install(TARGETS parseXMLFile DESTINATION bin)


########### next target ###############

SET(testContentDict16_SRCS
   testContentDict16.cpp
)

add_executable(testContentDict16 ${testContentDict16_SRCS})

target_link_libraries(testContentDict16 lima-lp-client lima-lp-linguisticprocessing-core lima-common-mediaprocessors lima-lp-linguisticprocessors lima-lp-linguisticresources lima-common-processunitframework lima-lp-linguisticdata lima-common-mediaticdata lima-common-factory lima-common-xmlconfigurationfiles lima-common-fsaaccess lima-common-factory lima-common-misc lima-lp-misc lima-common-data lima-lp-propertycode ${Boost_LIBRARIES}  ${QT_LIBRARIES} lima-common-qslog  ${optionalLibs})

install(TARGETS testContentDict16 DESTINATION bin)


########### next target ###############

SET(convertXmlToSBoW_SRCS
   convertXmlToSBoW.cpp
)

add_executable(convertXmlToSBoW ${convertXmlToSBoW_SRCS})

target_link_libraries(convertXmlToSBoW lima-common-factory lima-common-misc lima-lp-misc lima-common-data lima-common-fsaaccess lima-lp-bagofwords lima-common-mediaticdata lima-lp-linguisticdata lima-common-factory lima-common-xmlconfigurationfiles lima-lp-propertycode ${Boost_LIBRARIES}  ${QT_LIBRARIES} lima-common-qslog ${optionalLibs})

install(TARGETS convertXmlToSBoW DESTINATION bin)

########### next target ###############

SET(buildCompoundsDict_SRCS
   buildCompoundsDict.cpp
)

add_executable(buildCompoundsDict ${buildCompoundsDict_SRCS})

target_link_libraries(buildCompoundsDict lima-common-factory lima-common-misc lima-lp-misc lima-common-data lima-common-mediaticdata lima-lp-linguisticdata lima-common-factory lima-common-xmlconfigurationfiles lima-lp-propertycode lima-common-fsaaccess ${Boost_LIBRARIES}  ${QT_LIBRARIES} lima-common-qslog  ${optionalLibs})

install(TARGETS buildCompoundsDict DESTINATION bin)

########### next target ###############

SET(testReadLexicon_SRCS
   testReadLexicon.cpp
)

add_executable(testReadLexicon ${testReadLexicon_SRCS})

target_link_libraries(testReadLexicon lima-common-factory lima-common-misc
#   lima-lp-misc
  lima-common-data lima-common-fsaaccess ${Boost_LIBRARIES}  ${QT_LIBRARIES} lima-common-qslog  ${optionalLibs})

install(TARGETS testReadLexicon DESTINATION bin)

########### install files ###############
