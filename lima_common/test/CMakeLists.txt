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
include_directories(BEFORE ${CMAKE_SOURCE_DIR}/. ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/src/common )

########### next target ###############

SET(testDict16_SRCS
   testFsaDict16.cpp
)

add_executable(testDict16 ${testDict16_SRCS})

target_link_libraries(testDict16
	  lima-common
	  #  lima-common-fsaaccess
	  #  lima-common-misc
	  #  lima-common-data
#  lima-common-time
#  lima-common-qslog
  ${Boost_LIBRARIES}
  ${QT_LIBRARIES}
)

install(TARGETS testDict16 DESTINATION bin)

########### next target ###############

SET(testLexicon_SRCS
   testLexicon.cpp
)

add_executable(testLexicon ${testLexicon_SRCS})

message("QT_LIBRARIES are ${QT_LIBRARIES}")

target_link_libraries(testLexicon
  lima-common-fsaaccess
  lima-common-misc
  lima-common-data
  lima-common-time
  lima-common-qslog
  ${Boost_LIBRARIES}
  ${QT_LIBRARIES}
)

install(TARGETS testLexicon DESTINATION bin)


########### install files ###############
