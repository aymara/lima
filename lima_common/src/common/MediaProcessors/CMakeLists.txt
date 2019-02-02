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

add_definitions( -DLIMA_MEDIAPROCESSORS_EXPORTING )

########### next target ###############

SET(lima-common-mediaprocessors_LIB_SRCS
   MediaProcessors.cpp
   MediaProcessUnitPipeline.cpp
   MediaAnalysisDumper.cpp
   HandlerStreamBuf.cpp
   DumperStream.cpp
#    AbstractAnalysisDumper.cpp
)

add_library(lima-common-mediaprocessors SHARED ${lima-common-mediaprocessors_LIB_SRCS})

ADD_DLL_TO_PATH(lima-common-mediaprocessors)

target_link_libraries(lima-common-mediaprocessors 
  lima-common-mediaticdata 
  lima-common-processunitframework
)

set_target_properties(lima-common-mediaprocessors PROPERTIES VERSION ${LIMA_COMMON_LIB_VERSION} SOVERSION ${LIMA_COMMON_LIB_SOVERSION})
install(TARGETS lima-common-mediaprocessors DESTINATION ${LIB_INSTALL_DIR}
    COMPONENT runtime)


########### install files ###############
install(FILES
    MediaProcessors.h
    MediaProcessUnit.h
    MediaProcessUnitPipeline.h
    MediaAnalysisDumper.h
    HandlerStreamBuf.h
    DumperStream.h
#     AbstractAnalysisDumper.h
  DESTINATION include/common/MediaProcessors
  COMPONENT devel
)
