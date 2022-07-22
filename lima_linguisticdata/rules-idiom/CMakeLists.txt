# Copyright 2002-2020 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

foreach(LANG ${LIMA_LANGUAGES})
  if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${LANG})
    add_subdirectory(${LANG})
  endif (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${LANG})
endforeach(LANG ${LIMA_LANGUAGES})

