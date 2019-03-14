#   Copyright 2002-2013 CEA LIST
#
#   This file is part of LIMA.
#
#   LIMA is free software: you can redistribute it and/or modify
#   it under the terms of the GNU Affero General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   LIMA is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU Affero General Public License for more details.
#
#   You should have received a copy of the GNU Affero General Public License
#   along with LIMA.  If not, see <http://www.gnu.org/licenses/>

########### install files ###############

install(FILES
    log4cpp.properties
    lima-analysis.xml
    lima-common-eng.xml
    lima-common-eng.ud.xml
    lima-common-fre.xml
    lima-common-por.xml
    lima-common.xml
  DESTINATION share/config/lima
  COMPONENT runtime)

  install(FILES
    limacommon.log.properties
  DESTINATION share/config/lima/log4cpp
  COMPONENT runtime)
