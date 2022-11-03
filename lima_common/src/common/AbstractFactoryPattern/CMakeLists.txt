# Copyright 2002-2019 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )

########### next target ###############
add_definitions( -DLIMA_FACTORY_EXPORTING )

SET(lima-common-factory_LIB_SRCS
  AmosePluginsManager.cpp
  DynamicLibrariesManager.cpp
  MainFactoriesMap.cpp
)

add_library(lima-common-factory SHARED ${lima-common-factory_LIB_SRCS})

target_link_libraries(lima-common-factory
   lima-common-tools
   Boost::system
   Boost::filesystem
)

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
