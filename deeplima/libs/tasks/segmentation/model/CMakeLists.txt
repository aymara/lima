# Copyright 2021 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

add_library(birnn_classifier_for_segmentation
  STATIC
  birnn_classifier_for_segmentation.cpp
  birnn_classifier_for_segmentation.h
)

target_link_libraries(birnn_classifier_for_segmentation
  PRIVATE
  train_common
  torch_birnn_seq_classifier
  ${TORCH_LIBRARIES}
)

