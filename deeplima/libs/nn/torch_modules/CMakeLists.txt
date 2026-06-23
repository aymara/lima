# Copyright 2022 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

set(SOURCE_LIST
  deep_biaffine_attention_decoder.cpp
  deep_biaffine_attention_label_decoder.cpp
  # stanza_models_depparse_model.cpp
)

set(HEADER_LIST
  deep_biaffine_attention_decoder.h
  deep_biaffine_attention_label_decoder.h
  # stanza_models_depparse_model.h
)

add_library(torch_modules
  STATIC
  ${SOURCE_LIST}
  ${HEADER_LIST}
)

target_link_libraries(torch_modules
  PRIVATE
  ${TORCH_LIBRARIES}
)

