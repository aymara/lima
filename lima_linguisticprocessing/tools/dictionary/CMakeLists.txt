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
include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )


########### next target ###############

SET(sortutf16_SRCS
  sortutf16.cpp
  )

add_executable(sortutf16 ${sortutf16_SRCS})

target_link_libraries(
  sortutf16
  lima-lp-misc
  ${Boost_LIBRARIES}
   ${QT_LIBRARIES} lima-common-qslog
   ${optionalLibs}
  lima-common-data)

install(TARGETS sortutf16 DESTINATION bin)


########### next target ###############

SET(composedDict_SRCS
  testComposedDict.cpp
  DictionaryEntryLogger.cpp
  )

add_executable(composedDict ${composedDict_SRCS})

target_link_libraries(
  composedDict
  lima-common-qslog
  lima-lp-analysisdict
  lima-lp-propertycode
  lima-lp-linguisticdata
  lima-lp-misc
  lima-lp-linguisticresources
  lima-common-fsaaccess
  lima-common-factory
  lima-common-misc
  lima-common-data
  lima-common-mediaticdata
  lima-common-xmlconfigurationfiles
  lima-lp-client
  lima-lp-linguisticprocessing-core
  lima-common-mediaprocessors
  lima-lp-linguisticprocessors
  lima-lp-linguisticresources
  lima-common-processunitframework
  lima-common-qslog
  ${Boost_LIBRARIES}
  ${QT_LIBRARIES}
  ${optionalLibs}
)

install(TARGETS composedDict DESTINATION bin)


########### next target ###############

SET(compile-dictionary_SRCS
  compileDictionary.cpp
  KeysLogger.cpp
  DictionaryHandler.cpp
  )

add_executable(compile-dictionary ${compile-dictionary_SRCS})

target_link_libraries(
 compile-dictionary
  lima-lp-linguisticanalysisstructure
  lima-lp-linguisticprocessors
  lima-lp-linguisticresources
  lima-lp-flattokenizer
  lima-common-fsaaccess
 ${Boost_LIBRARIES}
  lima-lp-annotationgraph
   ${QT_LIBRARIES} lima-common-qslog lima-lp-linguisticdata
   ${optionalLibs} lima-common-factory lima-common-misc
  lima-common-processunitframework
  lima-lp-propertycode
  lima-common-xmlconfigurationfiles
  lima-common-data
  lima-common-mediaticdata
  lima-common-mediaprocessors
  lima-lp-analysisdict
  )

install(TARGETS compile-dictionary DESTINATION bin)


########### next target ###############

SET(check-dictionary_SRCS
  checkDictionary.cpp BasicHandler.cpp
  )

add_executable(check-dictionary ${check-dictionary_SRCS})

target_link_libraries(check-dictionary
  lima-lp-analysisdict
  lima-lp-linguisticprocessors
  lima-lp-linguisticresources
  lima-common-fsaaccess
 ${Boost_LIBRARIES}
   ${QT_LIBRARIES} lima-common-qslog
  lima-lp-linguisticdata
  lima-common-factory lima-common-misc
  lima-common-mediaticdata
  lima-common-mediaprocessors
  lima-common-data
  lima-common-processunitframework
  lima-lp-propertycode
  lima-common-xmlconfigurationfiles
 ${optionalLibs}
)

install(TARGETS check-dictionary DESTINATION bin)
