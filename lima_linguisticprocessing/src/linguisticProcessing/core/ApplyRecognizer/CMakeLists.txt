# Copyright 2002-2019 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )

add_definitions(-DLIMA_APPLYRECOGNIZER_EXPORTING)

########### next target ###############

SET(lima-lp-applyrecognizer_LIB_SRCS
   applyRecognizerActions.cpp
   applyRecognizer.cpp
   recognizerResultLogger.cpp
)

DECLARE_LIMA_PLUGIN(lima-lp-applyrecognizer)

target_link_libraries(lima-lp-applyrecognizer
   lima-lp-linguisticprocessors
   lima-lp-annotationgraph
   lima-lp-automaton
   lima-lp-textsegmentation
)

set_target_properties(lima-lp-applyrecognizer PROPERTIES VERSION ${LIMA_LP_LIB_VERSION} SOVERSION ${LIMA_LP_LIB_SOVERSION})

install(TARGETS lima-lp-applyrecognizer DESTINATION ${LIB_INSTALL_DIR}
  COMPONENT runtime)


########### install files ###############

install(FILES
    applyRecognizer.h
    applyRecognizerActions.h
    recognizerResultLogger.h
  DESTINATION include/linguisticProcessing/core/ApplyRecognizer
  COMPONENT devel
)
