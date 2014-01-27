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

add_definitions( -DLIMA_MEDIATICDATA_EXPORTING )
########### next target ###############

SET(lima-common-mediaticdata_LIB_SRCS
   EntityType.cpp
   mediaData.cpp
   mediaticData.cpp
)

add_library(lima-common-mediaticdata SHARED ${lima-common-mediaticdata_LIB_SRCS})

target_link_libraries(lima-common-mediaticdata lima-common-factory lima-common-xmlconfigurationfiles lima-common-misc lima-common-data)

set_target_properties(lima-common-mediaticdata PROPERTIES VERSION ${LIMA_COMMON_LIB_VERSION} SOVERSION ${LIMA_COMMON_LIB_SOVERSION})
install(TARGETS lima-common-mediaticdata DESTINATION lib)


########### install files ###############
install(FILES
    EntityType.h
    mediaData.h
    mediaticData.h
  DESTINATION include/common/MediaticData)
