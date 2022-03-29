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

add_definitions( -DLIMA_DATAHANDLER_EXPORTING )

########### next target ###############
add_definitions( -DQTSGMLENTITIES_EXPORTING )

set(lima-xp-qtsgmlentities_LIB_SRCS
   qtSgmlEntities.cpp
)

add_library(lima-xp-qtsgmlentities SHARED ${lima-xp-qtsgmlentities_LIB_SRCS})
ADD_DLL_TO_PATH(lima-xp-qtsgmlentities)

target_link_libraries(lima-xp-qtsgmlentities
  Qt5::Core
)

set_target_properties(lima-xp-qtsgmlentities
  PROPERTIES
    VERSION ${LIMA_LP_LIB_VERSION}
    SOVERSION ${LIMA_LP_LIB_SOVERSION}
)
install(TARGETS lima-xp-qtsgmlentities DESTINATION ${LIB_INSTALL_DIR})


########### next target ###############

SET(lima-common-datahandler_LIB_SRCS
   structureHandler.cpp
   shiftFrom.cpp
#    contentHandler.cpp
)

add_library(lima-common-datahandler SHARED ${lima-common-datahandler_LIB_SRCS})
ADD_DLL_TO_PATH(lima-common-datahandler)

target_link_libraries(lima-common-datahandler
  lima-common-data
  lima-xp-qtsgmlentities
)

set_target_properties(lima-common-datahandler PROPERTIES VERSION ${LIMA_COMMON_LIB_VERSION} SOVERSION ${LIMA_COMMON_LIB_SOVERSION})
install(TARGETS lima-common-datahandler DESTINATION ${LIB_INSTALL_DIR}
    COMPONENT runtime)


########### install files ###############
install(FILES
    AbstractAnalysisHandler.h
    AbstractXmlAnalysisHandler.h
    AbstractXmlDocumentHandler.h
    AbstractDocumentHandler.h
    AbstractProcessingClientHandler.h
    structureHandler.h
    contentHandler.h
    contentHandler.tcc
    shiftFrom.h
  DESTINATION include/common/Handler
  COMPONENT devel
)


add_subdirectory(tests)
