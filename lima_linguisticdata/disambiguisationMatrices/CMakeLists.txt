# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

option(BUILD_SVMTOOL_MODELS "Set to true to build and install the SVMTool PoS tagging models."
                             ON)

foreach(LANG ${LIMA_LANGUAGES})
  if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${LANG})
    add_subdirectory(${LANG})
  endif (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${LANG})
endforeach(LANG ${LIMA_LANGUAGES})
