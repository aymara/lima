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
file (GLOB SYMBOLICCODESFRE ${PROJECT_SOURCE_DIR}/analysisDictionary/fre/code/symbolicCode*-fre.xml)

add_custom_command(
  OUTPUT ${CMAKE_BINARY_DIR}/execEnv/resources/LinguisticProcessings/fre/code-fre.xml
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/execEnv/resources/LinguisticProcessings/fre
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/analysisDictionary/fre/code/code-fre.xml ${CMAKE_BINARY_DIR}/execEnv/resources/LinguisticProcessings/fre/code-fre.xml
  DEPENDS
    ${CMAKE_SOURCE_DIR}/analysisDictionary/fre/code/code-fre.xml
  COMMENT "create exec env for fre idiomatic expressions rules"
  VERBATIM
)

add_custom_command(
  OUTPUT idiomaticExpressions-fre.rules
  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/buildSeparableVerbList.pl -list=separableVerbs.lst -rules=separableVerbsSecond.rules ${CMAKE_CURRENT_SOURCE_DIR}/idioms-fre.txt > idioms-fre.withoutVerbs.txt
  COMMAND ${PROJECT_SOURCE_DIR}/scripts/reformat_idioms.pl ${CMAKE_CURRENT_SOURCE_DIR}/idioms-fre.txt > idioms-fre.grace
  COMMAND cat ${CMAKE_CURRENT_SOURCE_DIR}/separableVerbs.rules separableVerbsSecond.rules >> idioms-fre.grace
  COMMAND ${PROJECT_SOURCE_DIR}/scripts/transcodeIdiomatic.pl ${PROJECT_SOURCE_DIR}/analysisDictionary/fre/code/code-fre.xml ${SYMBOLICCODESFRE} idioms-fre.grace > idiomaticExpressions-fre.rules
  DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/buildSeparableVerbList.pl
    ${CMAKE_CURRENT_SOURCE_DIR}/categories_convert
    ${PROJECT_SOURCE_DIR}/scripts/reformat_idioms.pl
    ${PROJECT_SOURCE_DIR}/scripts/transcodeIdiomatic.pl
    ${CMAKE_CURRENT_SOURCE_DIR}/idioms-fre.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/separableVerbs.rules
    ${CMAKE_BINARY_DIR}/execEnv/resources/LinguisticProcessings/fre/code-fre.xml
    ${SYMBOLICCODESFRE}
  COMMENT "create fre idiomatic expressions rules"
  VERBATIM
)
# add_custom_target(
#   rules-fre
#   ALL
#   DEPENDS idiomaticExpressions-fre.rules
# )

# file (MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/execEnv/resources/LinguisticProcessings/fre/")
# file (COPY ${CMAKE_SOURCE_DIR}/analysisDictionary/fre/code/code-fre.xml DESTINATION "${CMAKE_BINARY_DIR}/execEnv/resources/LinguisticProcessings/fre/")

IDIOMATICENTITIES(fre)
