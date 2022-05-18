# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

########### next test ###############

SET(MediaticDataTest_SRCS
   MediaticDataTest.cpp
)

add_executable(MediaticDataTest ${MediaticDataTest_SRCS})

target_link_libraries(MediaticDataTest lima-common ${Boost_LIBRARIES} ${Qt5_LIBRARIES})

add_test(NAME MediaticDataTest COMMAND MediaticDataTest)

