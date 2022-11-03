# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

include_directories(BEFORE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/src/common )


########### next target ###############

SET(convertToUtf8_SRCS
   convertToUtf8.cpp
)

add_executable(convertToUtf8 ${convertToUtf8_SRCS})

target_link_libraries(convertToUtf8 lima-common)

install(TARGETS convertToUtf8 DESTINATION bin
    COMPONENT runtime)

########### install files ###############
