# Copyright 2002-2021 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

add_library(model_lemmatization
  STATIC
  seq2seq_for_lemmatization.cpp
)

target_link_libraries(model_lemmatization
  PRIVATE
  torch_birnn_seq2seq
  train_common
  morph_model
  ${TORCH_LIBRARIES}
)

