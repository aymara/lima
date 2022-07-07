# Copyright 2021 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

add_library(train_tag
  STATIC
  train_tag.cpp
)

target_link_libraries(train_tag
  PRIVATE
  birnn_classifier_for_tag
  train_common
  ${TORCH_LIBRARIES}
  fasttext-static
)

