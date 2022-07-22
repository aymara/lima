# Copyright 2002-2021 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

if (TensorFlow_FOUND AND Eigen_FOUND)
    include_directories(BEFORE ${CMAKE_SOURCE_DIR}/extern )

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

