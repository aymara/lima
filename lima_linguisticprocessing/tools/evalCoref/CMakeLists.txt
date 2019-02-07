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
  Qt5::Core
  Boost::regex
)

install(TARGETS evalCoref DESTINATION bin
  COMPONENT runtime)

########### install files ###############
