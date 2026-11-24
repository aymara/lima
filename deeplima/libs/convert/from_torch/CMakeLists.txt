# Copyright 2021 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

set(SOURCE_LIST
  convert_from_torch.cpp
  entity_tagging_model.cpp
  lemmatization_model.cpp
  graph_dp_model.cpp
)

set(HEADER_LIST
)

add_library(convert_from_torch
  STATIC
  ${SOURCE_LIST}
  ${HEADER_LIST}
)
# No *_EXPORTING definitions on the static library: the export macros only
# matter for the Windows DLL, and dllexport in a static library would make every
# DLL linking it re-export these symbols.

target_link_libraries(convert_from_torch
  PRIVATE
  birnn_classifier_for_segmentation
  birnn_classifier_for_tag
  model_lemmatization
  birnn_and_deep_biaffine_attention
  ${TORCH_LIBRARIES}
)

if (WIN32 AND NOT DEEPLIMA_STANDALONE)
  # On Windows, the LIMA plugins link the static library. The code is split
  # between static libs (segmentation/lemmatization inference...) and this one
  # with cross references in both directions (e.g. virtual methods of
  # BiRnnInferenceBase), which ELF shared objects tolerate but DLLs do not.
  add_library(convert_from_torch-shared ALIAS convert_from_torch)
elseif (NOT DEEPLIMA_STANDALONE)
  add_library(convert_from_torch-shared
    SHARED
    ${SOURCE_LIST}
    ${HEADER_LIST}
  )
  target_compile_definitions(convert_from_torch-shared
        PRIVATE "-DDEEPLIMA_CONVERTFROMTORCH_EXPORTING"
    )
  # Users of the DLL (the LIMA plugins) see its classes as dllimport; users of
  # the static convert_from_torch (the deeplima apps) must not.
  target_compile_definitions(convert_from_torch-shared
        INTERFACE DEEPLIMA_CONVERTFROMTORCH_SHARED
    )
  target_compile_definitions(convert_from_torch-shared
        PRIVATE "-DLEMM_EXPORTING"
    )
  target_compile_definitions(convert_from_torch-shared
        PRIVATE "-DTAG_EXPORTING" "-DSEGM_EXPORTING"
    )
  target_compile_definitions(convert_from_torch-shared
        PRIVATE "-DDEP_PARSING_EXPORTING"
    )

  target_link_libraries(convert_from_torch-shared
    PRIVATE
    birnn_classifier_for_segmentation
    birnn_classifier_for_tag
    model_lemmatization
    birnn_and_deep_biaffine_attention
    ${TORCH_LIBRARIES}
  )
  ADD_DLL_TO_PATH(convert_from_torch-shared)
  install(TARGETS convert_from_torch-shared
    DESTINATION ${LIB_INSTALL_DIR}
    COMPONENT runtime
  )
endif()

