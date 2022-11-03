# Copyright 2002-2020 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )

add_definitions(-DLIMA_DICTIONARY_EXPORTING)

########### next target ###############

SET(lima-lp-dictionary_LIB_SRCS
  BinaryEntry.cpp
  ConcatenatedEntry.cpp
  DictionaryEntry.cpp
  LingInfoEntry.cpp
  LingPropertyEntry.cpp
  SingleConcatenatedEntry.cpp
)

add_library(lima-lp-dictionary SHARED ${lima-lp-dictionary_LIB_SRCS})
ADD_DLL_TO_PATH(lima-lp-dictionary)

target_link_libraries(lima-lp-dictionary
  lima-common-misc
  lima-common
)

set_target_properties(lima-lp-dictionary PROPERTIES VERSION ${LIMA_LP_LIB_VERSION} SOVERSION ${LIMA_LP_LIB_SOVERSION})

install(TARGETS lima-lp-dictionary DESTINATION ${LIB_INSTALL_DIR}
  COMPONENT runtime)

########### install files ###############
