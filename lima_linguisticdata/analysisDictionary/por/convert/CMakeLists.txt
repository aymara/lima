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
# CONVERT(por ${CMAKE_CURRENT_SOURCE_DIR}/entrees_composees.txt)+

set(ENV{LC_ALL} "C")

file(GLOB dicoFiles ${CMAKE_CURRENT_SOURCE_DIR}/../delaf/*.txt)

if (NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  add_custom_command(
    OUTPUT dico.xml forms.dic default-por.dat
    COMMAND echo "<dictionary>" > dico.xml.tmp
    COMMAND cat ${dicoFiles} > forms.dic
    COMMAND python3 ${PROJECT_SOURCE_DIR}/scripts/merge-dico-poscorpus.py por forms.dic ${PROJECT_SOURCE_DIR}/disambiguisationMatrices/por/corpus/macmorpho.conll.txt > forms.dic.2
    COMMAND LC_ALL="C" sort -u -o forms.dic forms.dic.2
    COMMAND ${PROJECT_SOURCE_DIR}/scripts/xmlforms.pl -desacc=no ${CMAKE_CURRENT_SOURCE_DIR}/dicoponctu.txt dico.xml.tmp
    COMMAND ${PROJECT_SOURCE_DIR}/scripts/xmlforms.pl forms.dic dico.xml.tmp
    COMMAND echo "</dictionary>" >> dico.xml.tmp
    COMMAND ${PROJECT_SOURCE_DIR}/scripts/cmakeconvertdefautjys.pl ${CMAKE_CURRENT_SOURCE_DIR}/default-por.txt ../code/convjys.txt default-por.dat
    COMMAND mv dico.xml.tmp dico.xml
    DEPENDS
      ${dicoFiles}
      ${CMAKE_CURRENT_SOURCE_DIR}/default-por.txt
      ${CMAKE_CURRENT_SOURCE_DIR}/dicoponctu.txt
      ${PROJECT_SOURCE_DIR}/disambiguisationMatrices/por/corpus/macmorpho.conll.txt
    COMMENT "produce por XML dico under Linux"
    VERBATIM
  )
else()
  # WARNING: VERBATIM option add unintentional double quotes symbols in XML file
  add_custom_command(
    OUTPUT dico.xml  forms.dic default-por.dat
    COMMAND echo ^<dictionary^> > dico.xml.tmp
    COMMAND cat ${dicoFiles} > forms.dic
    COMMAND sort -u -o forms.dic forms.dic
    COMMAND perl ${PROJECT_SOURCE_DIR}/scripts/xmlforms.pl forms.dic dico.xml.tmp
    COMMAND echo ^</dictionary>^ >> dico.xml.tmp
    COMMAND perl ${PROJECT_SOURCE_DIR}/scripts/cmakeconvertdefautjys.pl ${CMAKE_CURRENT_SOURCE_DIR}/default-por.txt ../code/convjys.txt default-por.dat
    COMMAND mv dico.xml.tmp dico.xml
    DEPENDS
      ${dicoFiles}
      ${CMAKE_CURRENT_SOURCE_DIR}/default-por.txt
      ${CMAKE_CURRENT_SOURCE_DIR}/dicoponctu.txt
    COMMENT "produce por XML dico under Windows"
  )
endif()

add_custom_target(
  dicoxmlpor
  ALL
  DEPENDS dico.xml
)
add_dependencies(dicoxmlpor codepor )

add_custom_target(
  convertpor
  ALL
)
add_dependencies(convertpor dicoxmlpor )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/default-por.dat COMPONENT por DESTINATION share/apps/lima/resources/LinguisticProcessings/por)
