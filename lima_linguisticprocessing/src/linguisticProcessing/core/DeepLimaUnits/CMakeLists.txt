# Copyright 2002-2021 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )

########### next target ###############

include_directories(common)

add_subdirectory(RnnTokenizer)
add_subdirectory(RnnNER)
add_subdirectory(RnnTokensAnalyzer)
add_subdirectory(RnnDependencyParser)