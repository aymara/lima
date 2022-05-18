# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

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
