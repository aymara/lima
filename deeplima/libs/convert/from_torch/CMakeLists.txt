# Copyright 2021 CEA LIST
#
# This file is part of LIMA.
#
# LIMA is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# LIMA is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with LIMA.  If not, see <https://www.gnu.org/licenses/>

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

