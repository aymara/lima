# Copyright 2021 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

set(SOURCE_LIST
  static_graph.cpp
  dict.cpp
)

set(HEADER_LIST
  static_graph.h
  dict_base.h
  dict.h
)

add_library(static_graph
  STATIC
  ${SOURCE_LIST}
  ${HEADER_LIST}
)

target_link_libraries(static_graph
  PRIVATE
  ${TORCH_LIBRARIES}
)


