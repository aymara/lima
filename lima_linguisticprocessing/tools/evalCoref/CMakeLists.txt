# Copyright 2002-2019 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

include_directories(${CMAKE_SOURCE_DIR}/src )

########### next target ###############

SET(XmlParser_SRCS
  preprocessCorpus.cpp
)

add_executable(XmlParser ${XmlParser_SRCS})

target_link_libraries(XmlParser
  lima-common-misc
  lima-common-time
  lima-common-data
  Boost::regex
  ${optionalLibs}
)

install(TARGETS XmlParser DESTINATION bin
  COMPONENT runtime)

########### next target ###############

SET(evalCoref_SRCS
  evalCoref.cpp
)

add_executable(evalCoref ${evalCoref_SRCS})

target_link_libraries(evalCoref
  Qt${QT_VERSION_MAJOR}::Core
  Boost::regex
)

install(TARGETS evalCoref DESTINATION bin
  COMPONENT runtime)

########### install files ###############
