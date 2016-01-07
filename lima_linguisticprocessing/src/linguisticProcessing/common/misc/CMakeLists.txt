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

add_definitions(-DLIMA_LPMISC_EXPORTING)

########### next target ###############

SET(lima-lp-misc_LIB_SRCS
#   textFormater.cpp
  positionLengthList.cpp
)

add_library(lima-lp-misc SHARED ${lima-lp-misc_LIB_SRCS})

target_link_libraries(lima-lp-misc lima-common-data ${QT_LIBRARIES} lima-common-qslog)

set_target_properties(lima-lp-misc PROPERTIES VERSION ${LIMA_LP_LIB_VERSION} SOVERSION ${LIMA_LP_LIB_SOVERSION})

install(TARGETS lima-lp-misc DESTINATION ${LIB_INSTALL_DIR})


########### install files ###############
install(FILES
#   AbstractEnhancedAccess.h
#   PortableGetTimeOfDay.h
#   S2ByteString.h
#   textFormater.h
#   lic2mLogger.h
    positionLengthList.h
  DESTINATION include/linguisticProcessing/common/misc)
