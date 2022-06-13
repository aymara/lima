#   Copyright 2002-2019 CEA LIST
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

target_link_libraries(sortutf16
  lima-common-factory
  lima-common-misc
)

install(TARGETS sortutf16 DESTINATION bin
  COMPONENT runtime)


########### next target ###############

SET(composedDict_SRCS
  testComposedDict.cpp
  DictionaryEntryLogger.cpp
)

add_executable(composedDict ${composedDict_SRCS})

target_link_libraries(composedDict
  lima-lp-analysisdict
  lima-lp-linguisticresources
)

install(TARGETS composedDict DESTINATION bin
  COMPONENT runtime)


########### next target ###############

SET(compile-dictionary_SRCS
  compileDictionary.cpp
  KeysLogger.cpp
  DictionaryReader.cpp
)

add_executable(compile-dictionary ${compile-dictionary_SRCS})

target_link_libraries(compile-dictionary
  lima-lp-flattokenizer
)

install(TARGETS compile-dictionary DESTINATION bin
  COMPONENT runtime)


########### next target ###############

SET(check-dictionary_SRCS
  checkDictionary.cpp
  BasicHandler.cpp
)

add_executable(check-dictionary ${check-dictionary_SRCS})

target_link_libraries(check-dictionary
  lima-lp-analysisdict
)

install(TARGETS check-dictionary DESTINATION bin
  COMPONENT runtime)
