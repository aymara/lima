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
#

set(ENV{LC_ALL} "C")

file(GLOB dicoFiles ${CMAKE_CURRENT_SOURCE_DIR}/../freeling/*.dic)
# to add wordnet entries
#file(GLOB dicoFiles ${CMAKE_CURRENT_SOURCE_DIR}/../freeling/*.dic ${CMAKE_CURRENT_SOURCE_DIR}/../wordnet/*.dic)

if (NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  add_custom_command(
    OUTPUT dico.xml forms.dic default-eng.dat
    COMMAND cat ${PROJECT_SOURCE_DIR}/../../opening_tag.txt > dico.xml.tmp
    COMMAND cat ${dicoFiles} > forms.dic
    COMMAND python3 ${PROJECT_SOURCE_DIR}/scripts/merge-dico-poscorpus.py eng forms.dic ${CMAKE_BINARY_DIR}/disambiguisationMatrices/eng/corpus_eng_merge.txt > forms.dic.2
    COMMAND LC_ALL="C" sort -u -o forms.dic forms.dic.2
    COMMAND ${PROJECT_SOURCE_DIR}/scripts/xmlforms.pl forms.dic dico.xml.tmp
    COMMAND cat ${CMAKE_CURRENT_SOURCE_DIR}/compound_entries.txt >> dico.xml.tmp
    COMMAND cat ${PROJECT_SOURCE_DIR}/../../closing_tag.txt >> dico.xml.tmp
    COMMAND ${PROJECT_SOURCE_DIR}/scripts/cmakeconvertdefautjys.pl ${CMAKE_CURRENT_SOURCE_DIR}/default-eng.txt ../code/convjys.txt default-eng.dat
    COMMAND mv dico.xml.tmp dico.xml
    DEPENDS  
      ${dicoFiles}
      ${CMAKE_CURRENT_SOURCE_DIR}/default-eng.txt
      ${CMAKE_CURRENT_SOURCE_DIR}/compound_entries.txt
      ${CMAKE_CURRENT_BINARY_DIR}/../../../disambiguisationMatrices/eng/corpus_eng_merge.txt
    COMMENT "produce eng XML dico under Linux"
    VERBATIM
  )
else()
  # WARNING: VERBATIM option add unintentional double quotes symbols in XML file
  add_custom_command(
    OUTPUT dico.xml  freeling-ext-lima.txt default-eng.dat
    COMMAND cat ${PROJECT_SOURCE_DIR}/../../opening_tag.txt > dico.xml.tmp
    COMMAND cat ${dicoFiles} > forms.dic
    COMMAND sort -u -o freeling-ext-lima.txt forms.dic
    COMMAND perl ${PROJECT_SOURCE_DIR}/scripts/xmlforms.pl freeling-ext-lima.txt dico.xml.tmp
    COMMAND cat ${CMAKE_CURRENT_SOURCE_DIR}/compound_entries.txt >> dico.xml.tmp
    COMMAND cat ${PROJECT_SOURCE_DIR}/../../closing_tag.txt >> dico.xml.tmp
    COMMAND perl ${PROJECT_SOURCE_DIR}/scripts/cmakeconvertdefautjys.pl ${CMAKE_CURRENT_SOURCE_DIR}/default-eng.txt ../code/convjys.txt default-eng.dat
    COMMAND mv dico.xml.tmp dico.xml
    DEPENDS  ${dicoFiles} ${CMAKE_CURRENT_SOURCE_DIR}/default-eng.txt ${CMAKE_CURRENT_SOURCE_DIR}/compound_entries.txt
    COMMENT "produce eng XML dico under Windows"
  )
endif()

add_custom_target(
  dicoxmleng
  ALL
  DEPENDS dico.xml
)
add_dependencies(dicoxmleng codeeng categs-eng )

add_custom_target(
  converteng
  ALL
)
add_dependencies(converteng dicoxmleng )

install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/default-eng.dat 
  COMPONENT eng 
  DESTINATION share/apps/lima/resources/LinguisticProcessings/eng
)
