# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )


add_definitions( -DLIMA_PROCESSUNITFRAMEWORK_EXPORTING )

########### next target ###############

SET(lima-common-processunitframework_LIB_SRCS
   AnalysisContent.cpp
)

add_library(lima-common-processunitframework SHARED ${lima-common-processunitframework_LIB_SRCS})

ADD_DLL_TO_PATH(lima-common-processunitframework)

target_link_libraries(lima-common-processunitframework 
  lima-common-data 
)

set_target_properties(lima-common-processunitframework PROPERTIES 
  VERSION ${LIMA_COMMON_LIB_VERSION} 
  SOVERSION ${LIMA_COMMON_LIB_SOVERSION})
  
install(TARGETS lima-common-processunitframework DESTINATION ${LIB_INSTALL_DIR}
    COMPONENT runtime)


########### install files ###############
install(FILES
    AbstractProcessUnit.h
    AnalysisContent.h
    ProcessUnitPipeline.h
    ProcessUnitPipeline.tcc
  DESTINATION include/common/ProcessUnitFramework
  COMPONENT devel
)
