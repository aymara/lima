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
set(ENV{LC_ALL} "C")

file(GLOB dicoFiles ${CMAKE_CURRENT_SOURCE_DIR}/../lefff/*.dic)


add_custom_command(
  OUTPUT dico.xml
  COMMAND echo "<dictionary>" > dico.xml.tmp
#   COMMAND ${PROJECT_SOURCE_DIR}/scripts/convert-lefff-tags-to-lima-std.pl ${CMAKE_CURRENT_SOURCE_DIR}/convlefffstd.txt ../lefff/lefff-ext-simplified.txt > lefff-ext-lima.dic
  COMMAND cat ${dicoFiles} > lefff-ext-lima.dic
  COMMAND LC_ALL="C" sort -u -o lefff-ext-lima.dic lefff-ext-lima.dic
  COMMAND ${PROJECT_SOURCE_DIR}/scripts/xmlforms.pl lefff-ext-lima.dic dico.xml.tmp
  COMMAND echo "</dictionary>" >> dico.xml.tmp
  COMMAND ${PROJECT_SOURCE_DIR}/scripts/cmakeconvertdefautjys.pl ${CMAKE_CURRENT_SOURCE_DIR}/default-fre.txt ../code/convjys.txt default-fre.dat
  COMMAND mv dico.xml.tmp dico.xml
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/dicoponctu.txt ${CMAKE_CURRENT_SOURCE_DIR}/default-fre.txt ${dicoFiles}
  COMMENT "produce XML dico"
  VERBATIM
)

add_custom_target(
  dicoxmlfre
  ALL
  DEPENDS dico.xml
)
add_dependencies(dicoxmlfre leffffre )

add_custom_target(
  convertfre
  ALL
)
add_dependencies(convertfre dicoxmlfre )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/default-fre.dat COMPONENT fre DESTINATION share/apps/lima/resources/LinguisticProcessings/fre)
