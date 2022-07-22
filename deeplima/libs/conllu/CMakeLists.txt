# Copyright 2021 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

set(SOURCE_LIST
  file.cpp
  line.cpp
  treebank.cpp
)

set(HEADER_LIST
  file.h
  line.h
  treebank.h
)

add_library(conllu
  STATIC
  ${SOURCE_LIST}
  ${HEADER_LIST}
)

target_link_libraries(conllu
  PRIVATE
  ICU::uc
)
