# Copyright 2021 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

add_library(birnn_classifier_for_tag
  STATIC
  birnn_classifier_for_tag.cpp
)

target_link_libraries(birnn_classifier_for_tag
  PRIVATE
  torch_birnn_seq_classifier
  train_common
  ${TORCH_LIBRARIES}
)

