# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

################### tests ########################

########### next test ###############

SET(TimeUtilsTest0_SRCS
   TimeUtilsTest0.cpp
)

add_executable(TimeUtilsTest0 ${TimeUtilsTest0_SRCS})

target_link_libraries(TimeUtilsTest0 lima-common ${Boost_LIBRARIES} ${Qt_LIBRARIES})

add_test(NAME TimeUtilsTest0 COMMAND TimeUtilsTest0)

