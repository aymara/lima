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

add_definitions(-DLIMA_LINGUISTICRESOURCES_EXPORTING)
########### next target ###############
set (lima-lp-linguisticresources_MOC_HEADERS
  AbstractResource.h
)

qt5_wrap_cpp (lima-lp-linguisticresources_MOC_OUTFILES ${lima-lp-linguisticresources_MOC_HEADERS})


SET(lima-lp-linguisticresources_LIB_SRCS
  AbstractResource.cpp
  LinguisticResources.cpp
)

add_library(lima-lp-linguisticresources SHARED ${lima-lp-linguisticresources_LIB_SRCS} ${lima-lp-linguisticresources_MOC_OUTFILES})

target_link_libraries(lima-lp-linguisticresources lima-common-factory lima-common-misc lima-common-tools lima-common-data lima-common-mediaticdata lima-common-time lima-common-factory lima-common-xmlconfigurationfiles lima-common-processunitframework lima-common-mediaprocessors lima-lp-linguisticprocessors)

set_target_properties(lima-lp-linguisticresources PROPERTIES VERSION ${LIMA_LP_LIB_VERSION} SOVERSION ${LIMA_LP_LIB_SOVERSION})

install(TARGETS lima-lp-linguisticresources DESTINATION ${LIB_INSTALL_DIR})


########### install files ###############
install(FILES AbstractResource.h  LinguisticResources.h LinguisticResourcesExport.h DESTINATION include/linguisticProcessing/core/LinguisticResources)
