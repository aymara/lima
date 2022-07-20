# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/. ${CMAKE_SOURCE_DIR}/src )

########### next target ###############

SET(tva_SRCS
   tva.cpp
   AnalysisTestCase.cpp
)

add_executable(tva ${tva_SRCS})

target_link_libraries(tva lima-linguisticprocessing)

install(TARGETS tva DESTINATION bin
  COMPONENT runtime)


########### install files ###############
