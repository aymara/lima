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

add_definitions(-DLIMA_TGV_EXPORTING)

########### next target ###############

SET(lima-lp-tgv_LIB_SRCS
   TestCaseError.cpp
   TestCase.cpp
   TestCaseProcessor.cpp
   TestCasesHandler.cpp
)

add_library(lima-lp-tgv SHARED ${lima-lp-tgv_LIB_SRCS})

target_link_libraries(lima-lp-tgv 
  lima-lp-misc  
  lima-common-mediaprocessors 
  lima-common-mediaticdata 
  lima-common-processunitframework
  lima-common-fsaaccess 
  lima-common-factory 
  lima-common-xmlconfigurationfiles 
  lima-common-misc
  lima-common-time 
  lima-common-datahandler
  lima-common-data 
  lima-common-tools
  ${QT_LIBRARIES} 
)

set_target_properties(lima-lp-tgv PROPERTIES VERSION ${LIMA_LP_LIB_VERSION} SOVERSION ${LIMA_LP_LIB_SOVERSION})

install(TARGETS lima-lp-tgv DESTINATION ${LIB_INSTALL_DIR})


########### install files ###############

install(FILES
  TestCase.h
  TestCaseProcessor.hpp
  TestCaseError.hpp
  TestCasesHandler.h
  DESTINATION include/linguisticProcessing/common/tgv)
