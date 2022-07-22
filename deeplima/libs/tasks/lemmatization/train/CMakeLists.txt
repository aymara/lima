# Copyright 2002-2021 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

add_library(train_lemmatization
  STATIC
  train_lemmatization.cpp
)

target_link_libraries(train_lemmatization
  PRIVATE
  model_lemmatization
  train_common
  ${TORCH_LIBRARIES}
)

