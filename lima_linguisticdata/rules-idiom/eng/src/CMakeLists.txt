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
file (GLOB SYMBOLICCODESENG ${PROJECT_SOURCE_DIR}/analysisDictionary/eng/code/symbolicCode*-eng.xml)

add_custom_command(
  OUTPUT idiomaticExpressions-eng.rules
  COMMAND perl ${PROJECT_SOURCE_DIR}/scripts/reformat_idioms.pl ${CMAKE_CURRENT_SOURCE_DIR}/idioms-eng.txt > idioms-eng.grace
  COMMAND perl ${PROJECT_SOURCE_DIR}/scripts/transcodeIdiomatic.pl ${PROJECT_SOURCE_DIR}/analysisDictionary/eng/code/code-eng.xml ${SYMBOLICCODESENG} idioms-eng.grace > idiomaticExpressions-eng.rules
  DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/idioms-eng.txt
    ${SYMBOLICCODESENG}
  COMMENT "create eng idiomatic expressions rules"
  VERBATIM
)

IDIOMATICENTITIES(eng)
