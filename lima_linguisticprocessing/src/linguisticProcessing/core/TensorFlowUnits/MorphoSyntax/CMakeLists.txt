#   Copyright 2002-2019 CEA LIST
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

if (TensorFlow_FOUND AND Eigen_FOUND)
    add_subdirectory(fastText)

    SET(lima-lp-tensorflowmorphosyntax_LIB_SRCS
        TensorFlowMorphoSyntax.cpp
    )

    DECLARE_LIMA_PLUGIN(lima-lp-tensorflowmorphosyntax)

    target_compile_definitions(lima-lp-tensorflowmorphosyntax
        PRIVATE "-DLIMA_TENSORFLOWMORPHOSYNTAX_EXPORTING"
    )

    target_link_libraries(lima-lp-tensorflowmorphosyntax
        lima-lp-linguisticdata
        lima-lp-linguisticresources
        lima-lp-annotationgraph
        lima-lp-linguisticanalysisstructure
        lima-lp-textsegmentation
        lima-lp-syntacticanalysis
        lima-common
        ${TensorFlow_LIBRARIES}
    )

    if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
        target_link_libraries(lima-lp-tensorflowmorphosyntax fasttext-static)
    else()
        target_link_libraries(lima-lp-tensorflowmorphosyntax fasttext-shared)
    endif()

    set_target_properties(lima-lp-tensorflowmorphosyntax PROPERTIES
        VERSION ${LIMA_LP_LIB_VERSION}
        SOVERSION ${LIMA_LP_LIB_SOVERSION}
    )

    install(
       TARGETS lima-lp-tensorflowmorphosyntax
       DESTINATION ${LIB_INSTALL_DIR}
       COMPONENT runtime
    )

endif()

########### install files ###############
install(
    FILES
        TensorFlowMorphoSyntaxExport.h
    DESTINATION
        include/linguisticProcessing/core/TensorFlowMorphoSyntax
    COMPONENT devel
)

