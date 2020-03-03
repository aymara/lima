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

file (GLOB SYMBOLICCODESPOR ${PROJECT_SOURCE_DIR}/lima_linguisticdata/analysisDictionary/por/code/symbolicCode*-por.xml)

add_custom_command(
 OUTPUT idiomaticExpressions-por.rules
 COMMAND perl ${PROJECT_SOURCE_DIR}/lima_linguisticdata/scripts/reformat_idioms.pl ${CMAKE_CURRENT_SOURCE_DIR}/idioms-por.txt > idioms-por.grace
 COMMAND perl ${PROJECT_SOURCE_DIR}/lima_linguisticdata/scripts/transcodeIdiomatic.pl ${CMAKE_BINARY_DIR}/lima_linguisticprocessing/tools/common/convertSymbolicCodes --configDir=${PROJECT_SOURCE_DIR}/lima_common/conf/ ${PROJECT_SOURCE_DIR}/lima_linguisticdata/analysisDictionary/por/code/code-por.xml ${SYMBOLICCODESPOR} idioms-por.grace > idiomaticExpressions-por.rules
 DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/idioms-por.txt
    ${SYMBOLICCODESPOR}
    convertSymbolicCodes
 COMMENT "create por idiomatic expressions rules"
 VERBATIM
)

IDIOMATICENTITIES(por)
