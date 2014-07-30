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
# set(ADDED_LIST_FILES
#   listesfermees.txt
#   listesfermees_prenoms_en.txt
# )
# 
# CONVERT(eng ${CMAKE_CURRENT_SOURCE_DIR}/entrees_composees.txt)

set(ENV{LC_ALL} "C")

file(GLOB dicoFiles ${CMAKE_CURRENT_SOURCE_DIR}/../freeling/*.dic)

add_custom_command(
  OUTPUT dico.xml  freeling-ext-lima.txt default-eng.dat
  COMMAND echo "<dictionary>" > dico.xml.tmp
  COMMAND cat ${dicoFiles} > freeling-ext-lima.txt
  COMMAND LC_ALL="C" sort -u -o freeling-ext-lima.txt freeling-ext-lima.txt
  COMMAND ${PROJECT_SOURCE_DIR}/scripts/xmlforms.pl freeling-ext-lima.txt dico.xml.tmp
  COMMAND cat ${CMAKE_CURRENT_SOURCE_DIR}/entrees_composees.txt >> dico.xml.tmp
  COMMAND echo "</dictionary>" >> dico.xml.tmp
  COMMAND ${PROJECT_SOURCE_DIR}/scripts/cmakeconvertdefautjys.pl ${CMAKE_CURRENT_SOURCE_DIR}/default-eng.txt ../code/convjys.txt default-eng.dat
  COMMAND mv dico.xml.tmp dico.xml
  DEPENDS  ${dicoFiles} ${CMAKE_CURRENT_SOURCE_DIR}/default-eng.txt ${CMAKE_CURRENT_SOURCE_DIR}/entrees_composees.txt
  COMMENT "produce XML dico"
  VERBATIM
)

add_custom_target(
  dicoxmleng
  ALL
  DEPENDS dico.xml
)
add_dependencies(dicoxmleng codeeng )

add_custom_target(
  converteng
  ALL
)
add_dependencies(converteng dicoxmleng )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/default-eng.dat COMPONENT eng DESTINATION share/apps/lima/resources/LinguisticProcessings/eng)
