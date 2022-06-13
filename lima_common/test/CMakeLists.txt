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
include_directories(BEFORE ${CMAKE_SOURCE_DIR}/. ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/src/common )

########### next target ###############

SET(showinttypessizes_SRCS
   showinttypessizes.cpp
)

add_executable(showinttypessizes ${showinttypessizes_SRCS})
target_link_libraries(showinttypessizes)

install(TARGETS showinttypessizes DESTINATION bin
    COMPONENT runtime)

########### next target ###############

SET(testDict16_SRCS
   testFsaDict16.cpp
)

add_executable(testDict16 ${testDict16_SRCS})

target_link_libraries(testDict16
  lima-common-fsaaccess
  lima-common-time
)

install(TARGETS testDict16 DESTINATION bin)

########### next target ###############

SET(testLexicon_SRCS
   testLexicon.cpp
)

add_executable(testLexicon ${testLexicon_SRCS})

message("Qt_LIBRARIES are ${Qt_LIBRARIES}")

target_link_libraries(testLexicon
  lima-common-fsaaccess
)

install(TARGETS testLexicon DESTINATION bin
    COMPONENT runtime)


########### next target ###############

SET(testAccessMethod_SRCS
   testAccessMethod.cpp
)

add_executable(testAccessMethod ${testAccessMethod_SRCS})

target_link_libraries(testAccessMethod
  lima-common-factory
  lima-common-fsaaccess
  ${optionalLibs}
)

install(TARGETS testAccessMethod DESTINATION bin
    COMPONENT runtime)

########### next target ###############

SET(parseXMLFile_SRCS
   parseXMLFile.cpp
)

add_executable(parseXMLFile ${parseXMLFile_SRCS})

target_link_libraries(parseXMLFile
  lima-common-factory
  lima-common-xmlconfigurationfiles
  ${optionalLibs}
)

install(TARGETS parseXMLFile DESTINATION bin
    COMPONENT runtime)

########### next target ###############

SET(testAbstractFactoryPattern_SRCS
   testAbstractFactoryPattern.cpp
)

add_executable(testAbstractFactoryPattern ${testAbstractFactoryPattern_SRCS})

target_link_libraries(testAbstractFactoryPattern
  lima-common-factory
  lima-common-xmlconfigurationfiles
  ${optionalLibs}
)

install(TARGETS testAbstractFactoryPattern DESTINATION bin
    COMPONENT runtime)

########### next target ###############

SET(testProcessUnitFramework_SRCS
   testProcessUnitFramework.cpp
)

add_executable(testProcessUnitFramework ${testProcessUnitFramework_SRCS})

target_link_libraries(testProcessUnitFramework
  lima-common-factory
  lima-common-processunitframework
  lima-common-xmlconfigurationfiles
  ${optionalLibs}
)

install(TARGETS testProcessUnitFramework DESTINATION bin
    COMPONENT runtime)

########### install files ###############
