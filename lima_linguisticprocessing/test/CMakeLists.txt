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

include_directories($ENV{LIMA_EXTERNALS}}/include)
add_subdirectory(testAnnotationGraphs)
add_subdirectory(perlTCPClientServer)
if (QHTTPSERVER_FOUND)
  add_subdirectory(limaServer)
endif()
add_subdirectory(test-xmlreader)

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )

########### next target ###############

SET(analyzeText_SRCS
   analyzeText.cpp
)

add_executable(analyzeText ${analyzeText_SRCS})
set_target_properties(analyzeText PROPERTIES VERSION ${LIMA_VERSION})
MESSAGE(STATUS "Optional libs: ${optionalLibs}")
MESSAGE(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")

target_link_libraries(analyzeText
  lima-lp-linguisticprocessing-core
  lima-lp-bagofwords
  lima-lp-eventanalyzer
  Boost::program_options
)

install(TARGETS analyzeText
  DESTINATION bin
  COMPONENT runtime)

 ########### next target ###############

add_executable(limasimple-analyze limasimple-analyze.cpp)
MESSAGE(STATUS "Optional libs: ${optionalLibs}")
MESSAGE(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")

target_link_libraries(limasimple-analyze
  lima-linguisticprocessing # this is really necessary
  Boost::program_options
)

install(TARGETS limasimple-analyze DESTINATION bin
  COMPONENT runtime)

########### next target ###############
# if (${PYTHONLIBS_FOUND})
#   SET(srl_SRCS
#     srl.cpp
#   )
#
#   add_executable(srl ${srl_SRCS})
#   set_target_properties(srl PROPERTIES VERSION ${LIMA_VERSION})
#   target_link_libraries(srl
#       ${PYTHON_LIBRARIES}
#       lima-common-tools
#   )
#
#   install(TARGETS srl DESTINATION bin)
# endif()
########### next target ###############

# SET(threadedAnalyzeText_SRCS
#    threadedAnalyzeText.cpp
# )
#
# add_executable(threadedAnalyzeText ${threadedAnalyzeText_SRCS})
#
# target_link_libraries(threadedAnalyzeText
#     lima-common
#     lima-linguisticprocessing
#   ${optionalLibs}
#   ${Qt5_LIBRARIES}
#   ${Boost_LIBRARIES}
# )
#
# install(TARGETS threadedAnalyzeText DESTINATION bin)

########### next target ###############

SET(analyzetextservercore_SRCS
   analyzetextservercore.cpp
)

add_executable(analyzetextservercore ${analyzetextservercore_SRCS})
set_target_properties(analyzetextservercore PROPERTIES VERSION ${LIMA_VERSION})

target_link_libraries(analyzetextservercore
  lima-linguisticprocessing
)

install(TARGETS analyzetextservercore
  DESTINATION bin
  COMPONENT runtime)

########### next target ###############
# IF(MMIMAGE_FOUND)
# ########### next target ###############

SET(analyzeXml_SRCS
   analyzeMultimediaXmlDocuments.cpp
)

add_executable(analyzeXml ${analyzeXml_SRCS})
set_target_properties(analyzeXml PROPERTIES VERSION ${LIMA_VERSION})

target_link_libraries(analyzeXml
  lima-common-time
  lima-common-factory
  lima-common-tools
  lima-xp-documentsreader
  lima-common-fsaaccess
  lima-lp-analysisdumpers
  lima-lp-analysishandlers
  lima-lp-applyrecognizer
  lima-lp-automaton
  lima-lp-bagofwords
  lima-lp-annotationgraph
  lima-common-data
  lima-common-misc
  lima-common-datahandler
  lima-lp-easyxmldumper
  lima-lp-linguisticdata
  lima-lp-linguisticprocessors
  lima-lp-linguisticresources
  lima-lp-client
  lima-lp-linguisticprocessing-core
  lima-lp-analysisdict
  lima-lp-corefsolver
  lima-lp-linguisticanalysisstructure
  lima-lp-semanticanalysis
  lima-lp-misc
  lima-common-mediaprocessors
  lima-common-mediaticdata
  lima-lp-morphologicanalysis
  lima-lp-postagger
  lima-common-processunitframework
  lima-lp-propertycode
  lima-lp-specificentities
  lima-lp-syntacticanalysis
  lima-lp-flattokenizer
  lima-common-xmlconfigurationfiles
#   lima-xp-multimediadocumenthandler
  lima-lp-textsegmentation
  lima-xp-client
  lima-xp-core
  lima-common-datahandler
  lima-common-fsaaccess
  lima-lp-annotationgraph
  lima-common-data
  lima-common-misc
  lima-common-tools
  lima-common-mediaprocessors
  lima-common-mediaticdata
  lima-common-processunitframework lima-lp-propertycode
  lima-common-xmlconfigurationfiles lima-lp-regexmatcher
  ${QT_LIBRARIES}
  ${Boost_LIBRARIES}
)


install(TARGETS analyzeXml DESTINATION bin)


########### next target ###############
SET(readMultFile_SRCS
   readMultFile.cpp
)

add_executable(readMultFile ${readMultFile_SRCS})
set_target_properties(readMultFile PROPERTIES VERSION ${LIMA_VERSION})

target_link_libraries(readMultFile
  lima-common-tools
  lima-common-time
  lima-common-factory
  lima-common-tools
#   lima-xp-multimediadocumenthandler
  lima-lp-linguisticprocessing-core
  lima-lp-analysishandlers
  lima-lp-bagofwords
  lima-lp-propertycode
  lima-lp-linguisticdata
  lima-lp-misc
  lima-lp-easyxmldumper
  lima-lp-analysisdumpers
  lima-lp-applyrecognizer
  lima-lp-automaton
  lima-lp-linguisticprocessors
  lima-lp-linguisticresources
  lima-lp-morphologicanalysis
  lima-lp-postagger
  lima-lp-eventanalyzer
  lima-lp-specificentities
  lima-lp-syntacticanalysis
  lima-common-datahandler
  lima-common-fsaaccess
  lima-common-data
  lima-common-misc
  lima-common-tools
  lima-common-datahandler
  lima-common-mediaprocessors
  lima-common-mediaticdata
  lima-common-processunitframework
  lima-common-xmlconfigurationfiles
  lima-lp-client
  lima-lp-linguisticanalysisstructure
  lima-lp-analysisdict
  lima-lp-corefsolver
  lima-lp-annotationgraph
  lima-lp-flattokenizer
  ${QT_LIBRARIES}
  ${Boost_LIBRARIES}
)

install(TARGETS readMultFile DESTINATION bin)

add_subdirectory(convertMultXmlToMultBin)

########### install files ###############
