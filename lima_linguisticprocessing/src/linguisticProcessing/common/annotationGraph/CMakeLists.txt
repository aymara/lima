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
include_directories( BEFORE ${CMAKE_SOURCE_DIR}/src )

add_definitions(-DLIMA_ANNOTATIONGRAPH_EXPORTING)

########### next target ###############

SET(lima-lp-annotationgraph_LIB_SRCS
    AnnotationGraph.cpp
    AnnotationData.cpp 
    AnnotationGraphXmlDumper.cpp
    AnnotationGraphWriter.cpp
)

add_library(lima-lp-annotationgraph SHARED ${lima-lp-annotationgraph_LIB_SRCS})

target_link_libraries(lima-lp-annotationgraph lima-common-data lima-common-factory lima-common-misc lima-common-mediaticdata lima-common-processunitframework lima-lp-propertycode lima-lp-linguisticdata)

set_target_properties(lima-lp-annotationgraph PROPERTIES VERSION ${LIMA_LP_LIB_VERSION} SOVERSION ${LIMA_LP_LIB_SOVERSION})

install(TARGETS lima-lp-annotationgraph DESTINATION lib)


########### install files ###############
install(FILES
    GenericAnnotation.h
    AnnotationData.h
    AnnotationGraph.h
    AnnotationGraphXmlDumper.h
    AnnotationGraphWriter.h
  DESTINATION include/linguisticProcessing/common/annotationGraph/)
