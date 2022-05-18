# Copyright 2021 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

add_library(train_segmentation
  STATIC
  train_segmentation.cpp
)

target_link_libraries(train_segmentation
  PRIVATE
  birnn_classifier_for_segmentation
  ${TORCH_LIBRARIES}
)

