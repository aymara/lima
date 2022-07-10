# Copyright 2002-2020 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

file (GLOB SYMBOLICCODESFRE ${PROJECT_SOURCE_DIR}/analysisDictionary/fre/code/symbolicCode*-fre.xml)

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/idiomaticExpressions-fre.rules
  COMMAND perl ${CMAKE_CURRENT_SOURCE_DIR}/buildSeparableVerbList.pl -list=${CMAKE_CURRENT_BINARY_DIR}/separableVerbs.lst -rules=${CMAKE_CURRENT_BINARY_DIR}/separableVerbsSecond.rules ${CMAKE_CURRENT_SOURCE_DIR}/idioms-fre.txt > ${CMAKE_CURRENT_BINARY_DIR}/idioms-fre.withoutVerbs.txt
  COMMAND perl ${PROJECT_SOURCE_DIR}/scripts/reformat_idioms.pl ${CMAKE_CURRENT_SOURCE_DIR}/idioms-fre.txt > ${CMAKE_CURRENT_BINARY_DIR}/idioms-fre.grace
  COMMAND cat ${CMAKE_CURRENT_SOURCE_DIR}/separableVerbs.rules ${CMAKE_CURRENT_BINARY_DIR}/separableVerbsSecond.rules >> ${CMAKE_CURRENT_BINARY_DIR}/idioms-fre.grace
  COMMAND perl ${PROJECT_SOURCE_DIR}/scripts/transcodeIdiomatic.pl ${CMAKE_BINARY_DIR}/lima_linguisticprocessing/tools/common/convertSymbolicCodes --configDir=${PROJECT_SOURCE_DIR}/../lima_common/conf/ ${PROJECT_SOURCE_DIR}/analysisDictionary/fre/code/code-fre.xml ${SYMBOLICCODESFRE} ${CMAKE_CURRENT_BINARY_DIR}/idioms-fre.grace > ${CMAKE_CURRENT_BINARY_DIR}/idiomaticExpressions-fre.rules
  DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/buildSeparableVerbList.pl
    ${CMAKE_CURRENT_SOURCE_DIR}/categories_convert
    ${PROJECT_SOURCE_DIR}/scripts/reformat_idioms.pl
    ${PROJECT_SOURCE_DIR}/scripts/transcodeIdiomatic.pl
    ${CMAKE_CURRENT_SOURCE_DIR}/idioms-fre.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/separableVerbs.rules
    ${SYMBOLICCODESFRE}
    convertSymbolicCodes
  COMMENT "create fre idiomatic expressions rules"
  VERBATIM
)

IDIOMATICENTITIES(fre)
