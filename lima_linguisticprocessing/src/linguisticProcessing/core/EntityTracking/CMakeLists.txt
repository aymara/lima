# Copyright 2002-2019 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )


add_definitions(-DLIMA_ENTITYTRACKING_EXPORTING)

########### next target ###############

SET(lima-lp-entitytracker_LIB_SRCS CoreferenceEngine.cpp
   CoreferenceData.cpp
   EntityTracker.cpp
   EntityTrackerXmlLogger.cpp
)

DECLARE_LIMA_PLUGIN(lima-lp-entitytracker)

target_link_libraries(lima-lp-entitytracker
   lima-lp-linguisticprocessors
   lima-lp-linguisticanalysisstructure
)

set_target_properties(lima-lp-entitytracker PROPERTIES VERSION ${LIMA_LP_LIB_VERSION} SOVERSION ${LIMA_LP_LIB_SOVERSION})

install(TARGETS lima-lp-entitytracker DESTINATION ${LIB_INSTALL_DIR}
  COMPONENT runtime)
