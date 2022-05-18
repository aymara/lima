# Copyright 2002-2019 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

########### next test ###############

SET(AnnotationGraphTest0_SRCS
   AnnotationGraphTest0.cpp
)

add_executable(AnnotationGraphTest0 ${AnnotationGraphTest0_SRCS})

target_link_libraries(AnnotationGraphTest0
  lima-common-data
  lima-lp-annotationgraph
  Boost::unit_test_framework
)

add_test(NAME AnnotationGraphTest0 COMMAND AnnotationGraphTest0)

