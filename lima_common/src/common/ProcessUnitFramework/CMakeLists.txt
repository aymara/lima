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


add_definitions( -DLIMA_PROCESSUNITFRAMEWORK_EXPORTING )

########### next target ###############

SET(lima-common-processunitframework_LIB_SRCS
   AnalysisContent.cpp
)

add_library(lima-common-processunitframework SHARED ${lima-common-processunitframework_LIB_SRCS})

target_link_libraries(lima-common-processunitframework 
  lima-common-data 
)

set_target_properties(lima-common-processunitframework PROPERTIES 
  VERSION ${LIMA_COMMON_LIB_VERSION} 
  SOVERSION ${LIMA_COMMON_LIB_SOVERSION})
  
install(TARGETS lima-common-processunitframework DESTINATION ${LIB_INSTALL_DIR})


########### install files ###############
install(FILES
    AbstractProcessUnit.h
    AnalysisContent.h
    ProcessUnitPipeline.h
    ProcessUnitPipeline.tcc
  DESTINATION include/common/ProcessUnitFramework
  COMPONENT devel
)
