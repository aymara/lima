# Copyright 2002-2020 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

########### install files ###############

install(FILES
    log4cpp.properties
    lima-analysis.xml
    lima-common-eng.xml
    lima-common-fre.xml
    lima-common-por.xml
    lima-common.xml
    lima-common-ud.xml # Generic config for Universal Dependencies
    lima-common-ud-eng.xml # Generic config for Universal Dependencies
  DESTINATION share/config/lima
  COMPONENT runtime)

  install(FILES
    log4cpp/limacommon.log.properties
  DESTINATION share/config/lima/log4cpp
  COMPONENT runtime)
