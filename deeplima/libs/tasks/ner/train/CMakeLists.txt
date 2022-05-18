# Copyright 2021 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

add_library(train_ner
  STATIC
  train_ner.cpp
)

target_link_libraries(train_ner
  PRIVATE
  birnn_classifier_for_ner
  train_common
  ${TORCH_LIBRARIES}
  fasttext-static
)

