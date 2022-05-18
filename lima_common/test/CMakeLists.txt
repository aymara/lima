# Copyright 2002-2019 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

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

message("Qt5_LIBRARIES are ${Qt5_LIBRARIES}")

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
