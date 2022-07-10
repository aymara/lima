# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )

add_definitions( -DLIMA_TIME_EXPORTING )

########### next target ###############

SET(lima-common-time_LIB_SRCS
   PortableGetTimeOfDay.cpp
   traceUtils.cpp
   timeUtilsController.cpp
)

add_library(lima-common-time SHARED ${lima-common-time_LIB_SRCS})

ADD_DLL_TO_PATH(lima-common-time)

target_link_libraries(lima-common-time 
  lima-common-data 
)

set_target_properties(lima-common-time PROPERTIES VERSION ${LIMA_COMMON_LIB_VERSION} SOVERSION ${LIMA_COMMON_LIB_SOVERSION})
install(TARGETS lima-common-time DESTINATION ${LIB_INSTALL_DIR}
    COMPONENT runtime)


########### install files ###############
install(FILES
    PortableGetTimeOfDay.h
    traceUtils.h
    timeUtilsController.h
  DESTINATION include/common/time
  COMPONENT devel
)



add_subdirectory(tests)
