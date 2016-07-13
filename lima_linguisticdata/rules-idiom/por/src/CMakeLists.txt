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
# file (GLOB SYMBOLICCODESENG ${PROJECT_SOURCE_DIR}/analysisDictionary/por/code/symbolicCode*-por.xml)

# add_custom_command(
#  OUTPUT ${CMAKE_BINARY_DIR}/execEnv/resources/LinguisticProcessings/por/code-por.xml
#  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/execEnv/resources/LinguisticProcessings/por
#  COMMAND ${CMAKE_COMMAND} -E copy 
#    ${CMAKE_SOURCE_DIR}/analysisDictionary/por/code/code-por.xml
#    ${CMAKE_BINARY_DIR}/execEnv/resources/LinguisticProcessings/por/code-por.xml
#  DEPENDS
#    ${CMAKE_SOURCE_DIR}/analysisDictionary/por/code/code-por.xml
#  COMMENT "create exec env for por idiomatic expressions rules"
#  VERBATIM
# )

# add_custom_command(
#  OUTPUT idiomaticExpressions-por.rules
#  COMMAND ${PROJECT_SOURCE_DIR}/scripts/reformat_idioms.pl ${CMAKE_CURRENT_SOURCE_DIR}/idioms-por.txt > idioms-por.grace
#  COMMAND ${PROJECT_SOURCE_DIR}/scripts/transcodeIdiomatic.pl ${PROJECT_SOURCE_DIR}/analysisDictionary/por/code/code-por.xml ${SYMBOLICCODESENG} idioms-# por.grace > idiomaticExpressions-por.rules
#  DEPENDS
#    ${CMAKE_CURRENT_SOURCE_DIR}/idioms-por.txt
#    ${CMAKE_BINARY_DIR}/execEnv/resources/LinguisticProcessings/por/code-por.xml
#    ${SYMBOLICCODESENG}
#  COMMENT "create por idiomatic expressions rules"
#  VERBATIM
# )

# add_custom_target(
#   rules-por
#   ALL
#   DEPENDS idiomaticExpressions-por.rules
# )

# IDIOMATICENTITIES(por)
