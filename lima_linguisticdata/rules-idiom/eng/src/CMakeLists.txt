# Copyright 2002-2020 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

file (GLOB SYMBOLICCODESENG ${PROJECT_SOURCE_DIR}/analysisDictionary/eng/code/symbolicCode*-eng.xml)

add_custom_command(
  OUTPUT idiomaticExpressions-eng.rules
  COMMAND perl ${PROJECT_SOURCE_DIR}/scripts/reformat_idioms.pl ${CMAKE_CURRENT_SOURCE_DIR}/idioms-eng.txt > idioms-eng.grace
  COMMAND perl ${PROJECT_SOURCE_DIR}/scripts/transcodeIdiomatic.pl ${CMAKE_BINARY_DIR}/lima_linguisticprocessing/tools/common/convertSymbolicCodes --configDir=${PROJECT_SOURCE_DIR}/../lima_common/conf/ ${PROJECT_SOURCE_DIR}/analysisDictionary/eng/code/code-eng.xml ${SYMBOLICCODESENG} idioms-eng.grace > idiomaticExpressions-eng.rules
  DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/idioms-eng.txt
    ${SYMBOLICCODESENG}
    convertSymbolicCodes
  COMMENT "create eng idiomatic expressions rules"
  VERBATIM
)

IDIOMATICENTITIES(eng)
