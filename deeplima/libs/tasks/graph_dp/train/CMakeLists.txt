# Copyright 2021 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

add_library(train_graph_dp
  STATIC
  train_graph_dp.cpp
  conllu_file_iterator.cpp
)

target_link_libraries(train_graph_dp
  PRIVATE
  birnn_and_deep_biaffine_attention
  train_common
  ${TORCH_LIBRARIES}
  fasttext-static
  ${Boost_LIBRARIES}
)

