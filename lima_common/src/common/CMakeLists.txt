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
add_subdirectory(tools)
add_subdirectory(QsLog)
add_subdirectory(AbstractFactoryPattern)
add_subdirectory(AbstractProcessingClient)
add_subdirectory(Data)
add_subdirectory(FsaAccess)
add_subdirectory(Handler)
add_subdirectory(MediaProcessors)
add_subdirectory(misc)
add_subdirectory(MediaticData)
add_subdirectory(ProcessUnitFramework)
add_subdirectory(StringMap)
add_subdirectory(XMLConfigurationFiles)
add_subdirectory(time)

add_definitions( -DLIMA_COMMON_EXPORTING )

add_library(lima-common SHARED LimaCommon.cpp)
target_link_libraries(lima-common 
    lima-common-processunitframework 
    lima-common-factory
    lima-common-fsaaccess 
    lima-common-qslog 
    lima-common-xmlconfigurationfiles 
    lima-common-factory 
    lima-common-data 
    lima-common-misc 
    lima-common-datahandler 
    lima-common-time 
    lima-common-mediaprocessors 
    lima-common-mediaticdata)

set_target_properties(lima-common PROPERTIES VERSION ${LIMA_COMMON_LIB_VERSION} SOVERSION ${LIMA_COMMON_LIB_SOVERSION})

install(TARGETS lima-common DESTINATION lib)

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )


########### install files ###############
install(FILES LimaCommon.h DESTINATION include/common)
