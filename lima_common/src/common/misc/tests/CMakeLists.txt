# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

################### tests ########################

########### next test ###############

SET(StringsPoolTest0_SRCS
  StringsPoolTest0.cpp
)

add_executable(StringsPoolTest0 ${StringsPoolTest0_SRCS})

target_link_libraries(StringsPoolTest0
  lima-common-misc
  Boost::unit_test_framework
)

add_test(NAME StringsPoolTest0 COMMAND StringsPoolTest0)
