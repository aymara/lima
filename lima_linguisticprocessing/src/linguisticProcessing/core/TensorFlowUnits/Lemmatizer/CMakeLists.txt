#   Copyright 2002-2020 CEA LIST
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

if (TensorFlow_FOUND AND Eigen_FOUND)
    SET(lima-lp-tensorflowlemmatizer_LIB_SRCS
        Lemmatizer.cpp
    )

    DECLARE_LIMA_PLUGIN(lima-lp-tensorflowlemmatizer)

    target_compile_definitions(lima-lp-tensorflowlemmatizer
        PRIVATE "-DLIMA_TENSORFLOWLEMMATIZER_EXPORTING"
    )

    target_link_libraries(lima-lp-tensorflowlemmatizer
        lima-lp-linguisticdata
        lima-lp-linguisticresources
        lima-lp-annotationgraph
        lima-lp-linguisticanalysisstructure
        lima-lp-textsegmentation
        lima-lp-syntacticanalysis
        lima-common
        ${TensorFlow_LIBRARIES}
    )

    set_target_properties(lima-lp-tensorflowlemmatizer PROPERTIES
        VERSION ${LIMA_LP_LIB_VERSION}
        SOVERSION ${LIMA_LP_LIB_SOVERSION}
    )

    install(
       TARGETS lima-lp-tensorflowlemmatizer
       DESTINATION ${LIB_INSTALL_DIR}
       COMPONENT runtime
    )

endif()

########### install files ###############
install(
    FILES
        LemmatizerExport.h
    DESTINATION
        include/linguisticProcessing/core/TensorFlowUnits/Lemmatizer
    COMPONENT devel
)

