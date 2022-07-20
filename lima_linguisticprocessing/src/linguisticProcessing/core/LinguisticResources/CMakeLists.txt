# Copyright 2002-2019 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )

add_definitions(-DLIMA_LINGUISTICRESOURCES_EXPORTING)
########### next target ###############

SET(lima-lp-linguisticresources_LIB_SRCS
  AbstractAccessResource.cpp
  AbstractResource.cpp
  AbstractResourceBase.cpp
  LinguisticResources.cpp
)

add_library(lima-lp-linguisticresources SHARED
            ${lima-lp-linguisticresources_LIB_SRCS})
ADD_DLL_TO_PATH(lima-lp-linguisticresources)


target_link_libraries(lima-lp-linguisticresources
  lima-common-mediaticdata
)

set_target_properties(lima-lp-linguisticresources PROPERTIES
  VERSION ${LIMA_LP_LIB_VERSION}
  SOVERSION ${LIMA_LP_LIB_SOVERSION})

install(TARGETS lima-lp-linguisticresources
  DESTINATION ${LIB_INSTALL_DIR}
  COMPONENT runtime)


########### install files ###############
install(FILES
    AbstractResource.h
    AbstractResourceBase.h
    LinguisticResources.h
    LinguisticResourcesExport.h
  DESTINATION include/linguisticProcessing/core/LinguisticResources
  COMPONENT devel
)
