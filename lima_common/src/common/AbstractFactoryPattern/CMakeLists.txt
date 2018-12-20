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

########### next target ###############
add_definitions( -DLIMA_FACTORY_EXPORTING )

SET(lima-common-factory_LIB_SRCS
  MainFactoriesMap.cpp
  AmosePluginsManager.cpp
  DynamicLibrariesManager.cpp
)

add_library(lima-common-factory SHARED ${lima-common-factory_LIB_SRCS})

target_link_libraries(lima-common-factory lima-common-tools)

ADD_DLL_TO_PATH(lima-common-factory)

set_target_properties(lima-common-factory PROPERTIES 
  VERSION ${LIMA_COMMON_LIB_VERSION} 
  SOVERSION ${LIMA_COMMON_LIB_SOVERSION})

  install(
    TARGETS lima-common-factory 
    DESTINATION ${LIB_INSTALL_DIR} 
    COMPONENT runtime
)


########### install files ###############
install(FILES
    AbstractFactoryPatternExport.h
    ObjectManager.h # needed by InitializableObjectManager.h
    MainFactory.h # needed by RegistrableFactory.h
    MainFactoryLinux.h
    MainFactoryWin32.h
    InitializableObjectFactory.h # needed by template class SimpleFactory
    InitializableObjectManager.h # needed by InitializableObject.h
    AmosePluginsManager.h
    Singleton.h
    SimpleFactory.h
    InitializableObject.h
    RegistrableFactory.h
    ProcessingClientFactory.h
    MainFactoriesMap.h
    DynamicLibrariesManager.h
  DESTINATION include/common/AbstractFactoryPattern
  COMPONENT devel
  )
