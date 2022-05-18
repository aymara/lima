# Copyright 2021 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

INCLUDE_DIRECTORIES(AFTER ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(conllu)
add_subdirectory(static_graph)
add_subdirectory(convert)
add_subdirectory(nn)
add_subdirectory(tasks)
add_subdirectory(version)
add_subdirectory(helpers)
