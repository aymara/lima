# Copyright 2021 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

########### next test ###############

SET(SearchTest0_SRCS
   SearchTest0.cpp
)

add_executable(SearchTest0 ${SearchTest0_SRCS} ${SearchTest0_MOC_OUTFILES})

target_link_libraries(SearchTest0
  Qt${QT_VERSION_MAJOR}::Test
)

add_test(NAME SearchTest0 COMMAND SearchTest0)

