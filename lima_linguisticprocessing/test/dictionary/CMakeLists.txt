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
include_directories(${CMAKE_SOURCE_DIR}/src )


########### next target ###############

SET(compileDictionary_SRCS
   CompileDictionary.cpp
   main.cpp
   ParseMarkupAscii.cpp
)

add_executable(compileDictionary ${compileDictionary_SRCS})

target_link_libraries(compileDictionary general lima-lp-dictionary lima-lp-syntacticanalysis lima-lp-specificentities lima-lp-postagger lima-lp-morphologicanalysis lima-lp-linguisticresources lima-lp-linguisticprocessors phenixGraph data compactdict lima-lp-automaton lima-lp-analysisdumpers lima-lp-analysishandlers  ${QT_LIBRARIES} lima-common-qslog lima-lp-bagofwords ${Boost_LIBRARIES} lima-lp-linguisticdata lima-common-factory lima-common-xmlconfigurationfiles misc ${optionalLibs})

install(TARGETS compileDictionary DESTINATION bin)


########### next target ###############

SET(testDict_SRCS
   testFsaDictionary.cpp
)

add_executable(testDict ${testDict_SRCS})

target_link_libraries(testDict lima-lp-dictionary compactdict general lima-lp-linguisticresources ${Boost_LIBRARIES} lima-common-factory lima-common-xmlconfigurationfiles lima-lp-linguisticdata misc  ${QT_LIBRARIES} lima-common-qslog ${optionalLibs})

install(TARGETS testDict DESTINATION bin)


########### next target ###############

SET(composedDict_SRCS
   testComposedDict.cpp
)

add_executable(composedDict ${composedDict_SRCS})

target_link_libraries(composedDict lima-lp-linguisticresources general lima-lp-dictionary compactdict ${Boost_LIBRARIES} lima-lp-linguisticdata lima-common-factory lima-common-xmlconfigurationfiles misc ${QT_LIBRARIES} lima-common-qslog ${optionalLibs})

install(TARGETS composedDict DESTINATION bin)


########### next target ###############

SET(sortUtf8_SRCS
   sortutf8.cpp
)

add_executable(sortUtf8 ${sortUtf8_SRCS})

target_link_libraries(sortUtf8 ${ICU_LIBRARY})

install(TARGETS sortUtf8 DESTINATION bin)


########### next target ###############

SET(perfdict_SRCS
   testPerfCompactDict.cpp
)

add_executable(perfdict ${perfdict_SRCS})

target_link_libraries(perfdict general compactdict lima-lp-linguisticresources lima-lp-dictionary ${Boost_LIBRARIES} lima-lp-linguisticdata lima-common-factory lima-common-xmlconfigurationfiles ${QT_LIBRARIES} lima-common-qslog misc ${optionalLibs})

install(TARGETS perfdict DESTINATION bin)


########### next target ###############

SET(testContentDict16_SRCS
   testContentDict16.cpp
)

add_executable(testContentDict16 ${testContentDict16_SRCS})

target_link_libraries(testContentDict16 general lima-lp-dictionary misc lima-common-fsaaccess lima-common-factory lima-common-xmlconfigurationfiles lima-lp-linguisticdata ${Boost_LIBRARIES} ${QT_LIBRARIES} lima-common-qslog  ${optionalLibs})

install(TARGETS testContentDict16 DESTINATION bin)


########### install files ###############
