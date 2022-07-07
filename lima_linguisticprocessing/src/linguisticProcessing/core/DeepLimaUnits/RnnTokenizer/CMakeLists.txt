# Copyright 2002-2021 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )

########### next target ###############

if (EIGEN3_FOUND)
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
        convert_from_torch-shared
        ${TORCH_LIBRARIES}
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

