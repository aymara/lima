# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

# FlatTokenizer automata of the deprecated legacy media
if (WITH_LEGACY_PIPELINES)
  add_subdirectory(eng)
  add_subdirectory(fre)
  add_subdirectory(por)
endif()
add_subdirectory(StopLists)

########### install files ###############
