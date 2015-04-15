#   Copyright 2002-2015 CEA LIST
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

add_definitions( -DLIMA_COMMONMISC_EXPORTING )
########### next target ###############

set (lima-common-misc_MOC_HEADERS
    LimaFileSystemWatcher.h
    LimaFileSystemWatcher_p.h
    LimaMainTaskRunner.h)

qt5_wrap_cpp (lima-common-misc_MOC_OUTFILES ${lima-common-misc_MOC_HEADERS})

SET(lima-common-misc_LIB_SRCS 
    LimaFileSystemWatcher.cpp
    AbstractAccessIterators.cpp
    fsaStringsPool.cpp
    stringspool.cpp
    LimaMainTaskRunner.cpp
  )

if (NOT (${CMAKE_SYSTEM_NAME} STREQUAL "Windows"))
SET(lima-common-misc_LIB_SRCS
    ${lima-common-misc_LIB_SRCS}
    linenoise.c
  )

endif()
add_library(lima-common-misc SHARED ${lima-common-misc_LIB_SRCS} ${lima-common-misc_MOC_OUTFILES})

target_link_libraries(lima-common-misc lima-common-data ${BOOST_LIBRARIES} ${QT_LIBRARIES})

set_target_properties(lima-common-misc PROPERTIES VERSION ${LIMA_COMMON_LIB_VERSION} SOVERSION ${LIMA_COMMON_LIB_SOVERSION})
install(TARGETS lima-common-misc DESTINATION lib)

if (NOT DEFINED ${LIMA_DISABLE_FSW_TESTING})
add_subdirectory(tests)
endif()

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
    XSLExceptionnowarn.hpp
    NodeRefListnowarn.hpp
    XalanOutputStreamPrintWriternowarn.hpp
    linenoise.h
    LimaMainTaskRunner.h
    LimaFileSystemWatcher.h
  DESTINATION include/common/misc)
