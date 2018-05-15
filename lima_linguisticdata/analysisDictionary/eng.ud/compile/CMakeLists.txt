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

#   string(REPLACE "/" "" _dicostr ${CMAKE_CURRENT_BINARY_DIR}/../convert/dico.xml)
get_filename_component(_dicostr ${CMAKE_CURRENT_BINARY_DIR}/../convert/dico.xml NAME_WE)
#  set (CHARCHART "${CMAKE_INSTALL_PREFIX}/share/apps/lima/resources/LinguisticProcessings/eng.ud/tokenizerAutomaton-eng.ud.chars.tok")
set (CHARCHART "${PROJECT_SOURCE_DIR}/scratch/LinguisticProcessings/eng/tokenizerAutomaton-eng.chars.tok")
get_filename_component(DICOFILENAME ${CMAKE_CURRENT_BINARY_DIR}/../convert/dico.xml NAME_WE)

add_custom_command(
  OUTPUT ${DICOFILENAME}Dat-eng.ud.dat
  COMMAND compile-dictionary --charChart=${CHARCHART} --extractKeyList=keys ${CMAKE_CURRENT_BINARY_DIR}/../convert/dico.xml
  COMMAND LC_ALL="C" sort -T . -u keys > keys.sorted
  COMMAND testDict16 --charSize=2 --listOfWords=keys.sorted --output=${DICOFILENAME}Key-eng.ud.dat > output
#    COMMAND testDict16 --charSize=2 --input=${DICOFILENAME}Key-eng.ud.dat.tmp --spare --output=${DICOFILENAME}Key-eng.ud.dat >> output
  COMMAND compile-dictionary --charChart=${CHARCHART} --fsaKey=${DICOFILENAME}Key-eng.ud.dat --propertyFile=${CMAKE_CURRENT_SOURCE_DIR}/../code/code-eng.ud.xml --symbolicCodes=${CMAKE_CURRENT_BINARY_DIR}/../convert/symbolicCode-eng.ud.xml --output=${DICOFILENAME}Dat-eng.ud.dat ${CMAKE_CURRENT_BINARY_DIR}/../convert/dico.xml
  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/../convert/dico.xml ${CMAKE_CURRENT_SOURCE_DIR}/../code/code-eng.ud.xml ${CMAKE_CURRENT_BINARY_DIR}/../convert/symbolicCode-eng.ud.xml  ${CHARCHART}
  COMMENT "compile-dictionary output ${DICOFILENAME}Dat-eng.ud.dat"
  VERBATIM
)

add_custom_target(
  compilexmldiceng.ud${_dicostr}
  ALL
  DEPENDS ${DICOFILENAME}Dat-eng.ud.dat
)
add_dependencies(compilexmldiceng.ud${_dicostr} converteng.ud)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${DICOFILENAME}Key-eng.ud.dat
    ${CMAKE_CURRENT_BINARY_DIR}/${DICOFILENAME}Dat-eng.ud.dat
  COMPONENT eng.ud DESTINATION share/apps/lima/resources/LinguisticProcessings/eng.ud)

