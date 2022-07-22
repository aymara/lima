# Copyright 2002-2021 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

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

add_library(lima-common SHARED
  LimaCommon.cpp
  LinguisticCode/StdBitset.cpp
)

add_subdirectory(LinguisticCode/tests)

target_link_libraries(lima-common
  lima-common-factory
  lima-common-mediaticdata
  lima-common-xmlconfigurationfiles
  lima-common-tools
  lima-common-datahandler
  lima-common-mediaprocessors
  lima-common-time
  lima-common-misc
  lima-common-data
)

ADD_DLL_TO_PATH(lima-common)

set_target_properties(lima-common PROPERTIES VERSION ${LIMA_COMMON_LIB_VERSION} SOVERSION ${LIMA_COMMON_LIB_SOVERSION})

install(TARGETS lima-common DESTINATION ${LIB_INSTALL_DIR}
    COMPONENT runtime)

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )

configure_file (
  "LimaVersion.h.in"
  "LimaVersion.h"
  @ONLY
  )


########### install files ###############

install(FILES
    LimaCommon.h
    ${CMAKE_CURRENT_BINARY_DIR}/LimaVersion.h
  DESTINATION include/common
  COMPONENT devel
)

install(FILES
    LinguisticCode/StdBitset.h
  DESTINATION include/common/LinguisticCode
  COMPONENT devel
)
