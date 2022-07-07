# Copyright 2021 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

set(SOURCE_LIST
  convert_from_torch.cpp
  entity_tagging_model.cpp
  lemmatization_model.cpp
)

set(HEADER_LIST
)

add_library(convert_from_torch
  STATIC
  ${SOURCE_LIST}
  ${HEADER_LIST}
)

target_link_libraries(convert_from_torch
  PRIVATE
  birnn_classifier_for_segmentation
  birnn_classifier_for_tag
  model_lemmatization
  ${TORCH_LIBRARIES}
)

if (NOT DEEPLIMA_STANDALONE)
  add_library(convert_from_torch-shared
    SHARED
    ${SOURCE_LIST}
    ${HEADER_LIST}
  )

  target_link_libraries(convert_from_torch-shared
    PRIVATE
    birnn_classifier_for_segmentation
    birnn_classifier_for_tag
    model_lemmatization
    ${TORCH_LIBRARIES}
  )

  install(TARGETS convert_from_torch-shared
    DESTINATION lib
    COMPONENT runtime
  )
endif()

