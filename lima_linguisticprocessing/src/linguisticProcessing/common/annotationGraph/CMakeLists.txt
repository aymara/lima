# Copyright 2002-2019 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

include_directories( BEFORE ${CMAKE_SOURCE_DIR}/src )

add_definitions(-DLIMA_ANNOTATIONGRAPH_EXPORTING)

########### next target ###############

SET(lima-lp-annotationgraph_LIB_SRCS
  AnnotationGraph.cpp
  AnnotationData.cpp
  AnnotationGraphWriter.cpp
)

add_library(lima-lp-annotationgraph SHARED ${lima-lp-annotationgraph_LIB_SRCS})
ADD_DLL_TO_PATH(lima-lp-annotationgraph)

target_link_libraries(lima-lp-annotationgraph
  lima-lp-linguisticdata
)

set_target_properties(lima-lp-annotationgraph PROPERTIES VERSION ${LIMA_LP_LIB_VERSION} SOVERSION ${LIMA_LP_LIB_SOVERSION})

install(TARGETS lima-lp-annotationgraph DESTINATION ${LIB_INSTALL_DIR}
  COMPONENT runtime)

########### install files ###############
install(FILES
    GenericAnnotation.h
    AnnotationData.h
    AnnotationGraph.h
    AnnotationGraphWriter.h
  DESTINATION include/linguisticProcessing/common/annotationGraph/
  COMPONENT devel
)

add_subdirectory(tests)
