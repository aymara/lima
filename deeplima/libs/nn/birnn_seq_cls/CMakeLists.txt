# Copyright 2021 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

set(SOURCE_LIST
  birnn_seq_classifier.cpp
  birnn_classifier_script_generator.cpp
)

set(HEADER_LIST
  birnn_seq_classifier.h
)

add_library(torch_birnn_seq_classifier
  STATIC
  ${SOURCE_LIST}
  ${HEADER_LIST}
)

target_link_libraries(torch_birnn_seq_classifier
  PRIVATE
  static_graph
  ${TORCH_LIBRARIES}
)

