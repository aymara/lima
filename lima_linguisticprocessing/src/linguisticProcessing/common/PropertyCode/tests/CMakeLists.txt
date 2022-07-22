# Copyright 2002-2020 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

########### next test ###############

SET(PropertyCodeTest0_SRCS PropertyCodeTest0.cpp)

set (PropertyCodeTest0_MOC_HEADERS PropertyCodeTest0.h)

add_executable(PropertyCodeTest0 ${PropertyCodeTest0_SRCS} ${PropertyCodeTest0_MOC_OUTFILES})
target_link_libraries(PropertyCodeTest0 lima-linguisticprocessing Qt${QT_VERSION_MAJOR}::Test)

add_test(NAME PropertyCodeTest0 COMMAND PropertyCodeTest0)

