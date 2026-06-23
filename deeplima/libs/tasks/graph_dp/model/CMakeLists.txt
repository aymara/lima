# Copyright 2021 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

add_library(birnn_and_deep_biaffine_attention
  STATIC
  birnn_and_deep_biaffine_attention.cpp
  birnn_and_deep_biaffine_attention_script_generator.cpp
)

target_link_libraries(birnn_and_deep_biaffine_attention
  PRIVATE
  torch_birnn_seq_classifier
  train_common
  ${TORCH_LIBRARIES}
)

