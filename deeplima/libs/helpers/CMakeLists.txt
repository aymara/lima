# Copyright 2021 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

set(SOURCE_LIST
  path_resolver.cpp
)

set(HEADER_LIST
  path_resolver.h
)

add_library(utils
  STATIC
  ${SOURCE_LIST}
  ${HEADER_LIST}
)

target_link_libraries(utils
  PRIVATE
  Boost::filesystem
)

