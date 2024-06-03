# Copyright 2002-2020 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

file (GLOB SYMBOLICCODESPOR ${PROJECT_SOURCE_DIR}/analysisDictionary/por/code/symbolicCode*-por.xml)

add_custom_command(
 OUTPUT idiomaticExpressions-por.rules
 COMMAND python3 ${PROJECT_SOURCE_DIR}/scripts/reformat_idioms.py ${CMAKE_CURRENT_SOURCE_DIR}/idioms-por.txt > idioms-por.grace
 COMMAND python3 ${PROJECT_SOURCE_DIR}/scripts/transcode_idiomatic.py ${CMAKE_BINARY_DIR}/lima_linguisticprocessing/tools/common/convertSymbolicCodes ${PROJECT_SOURCE_DIR}/analysisDictionary/por/code/code-por.xml ${SYMBOLICCODESPOR} idioms-por.grace > idiomaticExpressions-por.rules
 DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/idioms-por.txt
    ${SYMBOLICCODESPOR}
    convertSymbolicCodes
 COMMENT "create por idiomatic expressions rules"
 VERBATIM
)

IDIOMATICENTITIES(por)
