# Copyright 2002-2021 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )

########### next target ###############
if (PYTHON_LIBRARY_FOUND)

SET(lima-lp-pythontensorflowtokenizer_LIB_SRCS
    PythonUppsalaTensorFlowTokenizer.cpp
    PythonTensorFlowTokenizer.cpp
    PythonHelpers.cpp
)

DECLARE_LIMA_PLUGIN(lima-lp-pythontensorflowtokenizer)

target_compile_definitions(lima-lp-pythontensorflowtokenizer
    PRIVATE "-DLIMA_TENSORFLOWTOKENIZER_EXPORTING"
)

target_link_libraries(lima-lp-pythontensorflowtokenizer
    lima-lp-linguisticdata
    lima-lp-linguisticresources
    lima-lp-annotationgraph
    lima-lp-linguisticanalysisstructure
    lima-lp-textsegmentation
    lima-common
    ${PYTHON_LIBRARIES}
)

set_target_properties(lima-lp-pythontensorflowtokenizer PROPERTIES
    VERSION ${LIMA_LP_LIB_VERSION}
    SOVERSION ${LIMA_LP_LIB_SOVERSION}
)

install(
    TARGETS lima-lp-pythontensorflowtokenizer
    DESTINATION ${LIB_INSTALL_DIR}
    COMPONENT runtime
)
endif()
########### next target ###############

if (TensorFlow_FOUND AND Eigen_FOUND)
    SET(lima-lp-tensorflowtokenizer_LIB_SRCS
        tokUtils.cpp
        CppUppsalaTensorFlowTokenizer.cpp
    )

    DECLARE_LIMA_PLUGIN(lima-lp-tensorflowtokenizer)

    target_compile_definitions(lima-lp-tensorflowtokenizer
        PRIVATE "-DLIMA_TENSORFLOWTOKENIZER_EXPORTING"
    )

    target_link_libraries(lima-lp-tensorflowtokenizer
        lima-lp-linguisticdata
        lima-lp-linguisticresources
        lima-lp-annotationgraph
        lima-lp-linguisticanalysisstructure
        lima-lp-textsegmentation
        lima-common
        ${TensorFlow_LIBRARIES}
    )

    set_target_properties(lima-lp-tensorflowtokenizer PROPERTIES
        VERSION ${LIMA_LP_LIB_VERSION}
        SOVERSION ${LIMA_LP_LIB_SOVERSION}
    )

    install(
       TARGETS lima-lp-tensorflowtokenizer
       DESTINATION ${LIB_INSTALL_DIR}
       COMPONENT runtime
    )

endif()

########### install files ###############
install(
    FILES
        TensorFlowTokenizerExport.h
    DESTINATION
        include/linguisticProcessing/core/TensorFlowTokenizer
    COMPONENT devel
)

