# Copyright 2021 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

########### next test ###############

SET(ShiftFromTest0_SRCS
   ShiftFromTest0.cpp
)

add_executable(ShiftFromTest0 ${ShiftFromTest0_SRCS} ${ShiftFromTest0_MOC_OUTFILES})

target_link_libraries(ShiftFromTest0
  lima-common-datahandler
  Qt5::Test
)

add_test(NAME ShiftFromTest0 COMMAND ShiftFromTest0)

