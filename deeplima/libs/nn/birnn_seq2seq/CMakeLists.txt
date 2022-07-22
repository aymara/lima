# Copyright 2002-2021 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

set(SOURCE_LIST
  birnn_seq2seq.cpp
  birnn_seq2seq_script_generator.cpp
)

set(HEADER_LIST
  birnn_seq2seq.h
)

add_library(torch_birnn_seq2seq
  STATIC
  ${SOURCE_LIST}
  ${HEADER_LIST}
)

target_link_libraries(torch_birnn_seq2seq
  PRIVATE
  torch_birnn_seq_classifier
  ${TORCH_LIBRARIES}
)

