# Copyright 2002-2019 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

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
