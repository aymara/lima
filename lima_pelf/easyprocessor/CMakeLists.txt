# Copyright 2004-2019 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

########### next target ###############

SET(easyprocessor_SRCS
  easyprocessor.cpp
  EasySourceHandler.cpp
)

add_executable(easyprocessor ${easyprocessor_SRCS})

target_link_libraries(easyprocessor
  lima-common-xmlconfigurationfiles
  lima-common-mediaticdata
  lima-lp-analysishandlers
  lima-lp-client
  ${optionalLibs}
  Qt5::Xml
)

install(TARGETS easyprocessor DESTINATION bin
  COMPONENT runtime)

install(FILES
    easyprocessor.conf.xml
  DESTINATION share/config/lima
  COMPONENT runtime)

install(FILES
    easy.dtd
    passage.dtd
  DESTINATION share/apps/pelf/resources
  COMPONENT runtime)
