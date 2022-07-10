# Copyright 2002-2019 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )

add_definitions( -DLIMA_COMMONMISC_EXPORTING )
########### next target ###############

SET(lima-common-misc_LIB_SRCS
    AbstractAccessIterators.cpp
    fsaStringsPool.cpp
    stringspool.cpp
)

if (NOT (${CMAKE_SYSTEM_NAME} STREQUAL "Windows"))
  SET(lima-common-misc_LIB_SRCS
      ${lima-common-misc_LIB_SRCS}
      linenoise.c
  )
endif()

add_library(lima-common-misc SHARED ${lima-common-misc_LIB_SRCS})

ADD_DLL_TO_PATH(lima-common-misc)

target_link_libraries(lima-common-misc
  lima-common-data
)

set_target_properties(lima-common-misc PROPERTIES
  VERSION ${LIMA_COMMON_LIB_VERSION}
  SOVERSION ${LIMA_COMMON_LIB_SOVERSION}
)

install(TARGETS lima-common-misc DESTINATION ${LIB_INSTALL_DIR}
  COMPONENT runtime
)

add_subdirectory(tests)

########### install files ###############
install(FILES
    AbstractAccessByString.h
    AbstractAccessIterators.h
    AbstractRwAccessByString.h
    fsaStringsPool.h
    stringspool.h
    Exceptions.h
    adjacency_listnowarn.hpp
    graphviznowarn.hpp
    depth_first_searchnowarn.hpp
    NodeRefListnowarn.hpp
    linenoise.h
    escaper.h
  DESTINATION include/common/misc
  COMPONENT devel
)
