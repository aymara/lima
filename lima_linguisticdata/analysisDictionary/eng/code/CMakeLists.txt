# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

file(GLOB CODES_FILES_LIST "${CMAKE_CURRENT_SOURCE_DIR}/symbolicCode*.xml")
CODES(eng ${CODES_FILES_LIST})
