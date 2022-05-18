# Copyright 2002-2020 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )

add_subdirectory(tests)

add_definitions(-DLIMA_PROPERTYCODE_EXPORTING)

########### next target ###############

SET(lima-lp-propertycode_LIB_SRCS
   PropertyAccessor.cpp
   PropertyManager.cpp
   PropertyCodeManager.cpp
   XMLPropertyHandler.cpp
   SymbolicCodeXMLHandler.cpp
)

add_library(lima-lp-propertycode SHARED ${lima-lp-propertycode_LIB_SRCS})
ADD_DLL_TO_PATH(lima-lp-propertycode)


target_link_libraries(lima-lp-propertycode
  lima-lp-misc
  lima-common
  lima-common-mediaticdata
  lima-common-time
  lima-common-datahandler
)

set_target_properties(lima-lp-propertycode PROPERTIES VERSION ${LIMA_LP_LIB_VERSION} SOVERSION ${LIMA_LP_LIB_SOVERSION})

install(TARGETS lima-lp-propertycode DESTINATION ${LIB_INSTALL_DIR}
  COMPONENT runtime)


########### install files ###############
install(FILES
    PropertyCodeManager.h
    PropertyManager.h
    PropertyAccessor.h
    SymbolicCodeXMLHandler.h
    XMLPropertyHandler.h
  DESTINATION include/linguisticProcessing/common/PropertyCode
  COMPONENT devel
)
