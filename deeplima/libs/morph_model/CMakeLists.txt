# Copyright 2002-2021 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

set(SOURCE_LIST
  morph_model.cpp
  morph_model_builder.cpp
)

set(HEADER_LIST
  morph_model_builder.h
)

add_library(morph_model
  STATIC
  ${SOURCE_LIST}
  ${HEADER_LIST}
)

