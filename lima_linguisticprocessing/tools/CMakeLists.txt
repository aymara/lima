# Copyright 2002-2019 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

# add_subdirectory(applyAutomaton)
add_subdirectory(automatonCompiler)
add_subdirectory(common)
add_subdirectory(dictionary)
# add_subdirectory(evalCoref) # TODO corect boost regex compilation in some contexts
add_subdirectory(normalize)
add_subdirectory(tva)
add_subdirectory(tvx)

include_directories(${CMAKE_SOURCE_DIR}/src )

########### install files ###############
