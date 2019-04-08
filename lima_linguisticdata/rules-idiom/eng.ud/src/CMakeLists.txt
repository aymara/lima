#   Copyright 2002-2019 CEA LIST
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

add_custom_command(
  OUTPUT idiomaticExpressions-eng.ud.rules
  COMMAND perl ${PROJECT_SOURCE_DIR}/scripts/reformat_idioms.pl ${CMAKE_CURRENT_SOURCE_DIR}/idioms-eng.ud.txt > idioms-eng.ud.grace
  COMMAND perl ${PROJECT_SOURCE_DIR}/scripts/transcodeIdiomatic.pl ${CMAKE_BINARY_DIR}/lima_linguisticprocessing/tools/common/convertSymbolicCodes --configDir=${PROJECT_SOURCE_DIR}/../lima_common/conf/ ${PROJECT_SOURCE_DIR}/analysisDictionary/eng.ud/code/code-eng.ud.xml ${PROJECT_BINARY_DIR}/analysisDictionary/eng.ud/convert/symbolicCode*-eng.ud.xml idioms-eng.ud.grace > idiomaticExpressions-eng.ud.rules
  DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/idioms-eng.ud.txt
    convertSymbolicCodes
    codeeng.ud
  COMMENT "create eng.ud idiomatic expressions rules"
  VERBATIM
)

IDIOMATICENTITIES(eng.ud)
