#   Copyright 2002-2021 CEA LIST
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

if (Eigen_FOUND)
    include_directories(AFTER ${CMAKE_SOURCE_DIR}/deeplima/include)
    include_directories(AFTER ${CMAKE_SOURCE_DIR}/deeplima/libs)
    include_directories(AFTER ${CMAKE_SOURCE_DIR}/extern/libtorch/include)
    include_directories(AFTER ${CMAKE_SOURCE_DIR}/extern/libtorch/include/torch/csrc/api/include)
    include_directories(AFTER ${CMAKE_SOURCE_DIR}/extern/)

    SET(lima-lp-rnn-tokenizer_LIB_SRCS
        RnnTokenizer.cpp
    )

    DECLARE_LIMA_PLUGIN(lima-lp-rnn-tokenizer)

    target_compile_definitions(lima-lp-rnn-tokenizer
        PRIVATE "-DLIMA_RNN_TOKENIZER_EXPORTING"
        PUBLIC
        DEEPLIMA_INFERENCE_ENGINE=IE_EIGEN

    )

    target_link_libraries(lima-lp-rnn-tokenizer
        lima-lp-linguisticdata
        lima-lp-linguisticresources
        lima-lp-annotationgraph
        lima-lp-linguisticanalysisstructure
        lima-lp-textsegmentation
        lima-common
        ICU::uc
        convert_from_torch
    )

    set_target_properties(lima-lp-rnn-tokenizer PROPERTIES
        VERSION ${LIMA_LP_LIB_VERSION}
        SOVERSION ${LIMA_LP_LIB_SOVERSION}
    )

    install(
       TARGETS lima-lp-rnn-tokenizer
       DESTINATION ${LIB_INSTALL_DIR}
       COMPONENT runtime
    )

endif()

