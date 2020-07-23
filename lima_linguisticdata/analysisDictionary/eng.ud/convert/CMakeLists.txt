#   Copyright 2002-2020 CEA LIST
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

file(GLOB dicoFiles ${PROJECT_SOURCE_DIR}/disambiguisationMatrices/eng.ud/UD_English-EWT/*-train.conllu)
# file(GLOB dicoFiles ${CMAKE_CURRENT_SOURCE_DIR}/../freeling/ud.dic)
# file(GLOB dicoFiles ${CMAKE_CURRENT_SOURCE_DIR}/../freeling/*.dic)
# to add wordnet entries
#file(GLOB dicoFiles ${CMAKE_CURRENT_SOURCE_DIR}/../freeling/*.dic ${CMAKE_CURRENT_SOURCE_DIR}/../wordnet/*.dic)


# generate symbolic code file from code file
add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/convjys.txt ${CMAKE_CURRENT_BINARY_DIR}/symbolicCode-eng.ud.xml
  COMMAND ${PROJECT_SOURCE_DIR}/scripts/convertCodeToSymbolicCode.py ../code/coded.txt > ${CMAKE_CURRENT_BINARY_DIR}/symbolicCode-eng.ud.xml
  COMMAND convertSymbolicCodes --configDir=${CMAKE_BINARY_DIR}/execEnv/config --code=../code/code-eng.ud.xml --output=${CMAKE_CURRENT_BINARY_DIR}/convjys.txt ${CMAKE_CURRENT_BINARY_DIR}/symbolicCode-eng.ud.xml
  COMMAND parseXMLPropertyFile --configDir=${CMAKE_BINARY_DIR}/execEnv/config --code=../code/code-eng.ud.xml --output=${CMAKE_CURRENT_BINARY_DIR}/code-eng.ud.xml.log
  DEPENDS ../code/code-eng.ud.xml ../code/coded.txt ${CMAKE_BINARY_DIR}/execEnv/config/log4cpp.properties
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMENT "Generate convjys.txt and symbolicCode-eng.ud.xml"
  VERBATIM
)
add_custom_target(
  codeeng.ud
  ALL
  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/convjys.txt
)

if (NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  add_custom_command(
    OUTPUT dico.xml  freeling-ext-lima.txt default-eng.ud.dat
    COMMAND echo "<dictionary>" > dico.xml.tmp
    COMMAND ${PROJECT_SOURCE_DIR}/scripts/convert-ud-to-dic.py --features=none --output=freeling-ext-lima.txt.tmp ${dicoFiles}
    COMMAND LC_ALL="C" sort -u -o freeling-ext-lima.txt freeling-ext-lima.txt.tmp
    COMMAND ${PROJECT_SOURCE_DIR}/scripts/xmlforms.pl freeling-ext-lima.txt dico.xml.tmp
    COMMAND echo "</dictionary>" >> dico.xml.tmp
    COMMAND ${PROJECT_SOURCE_DIR}/scripts/cmakeconvertdefautjys.pl ${CMAKE_CURRENT_SOURCE_DIR}/default-eng.txt ${CMAKE_CURRENT_BINARY_DIR}/convjys.txt default-eng.ud.dat
    COMMAND mv dico.xml.tmp dico.xml
    DEPENDS
      ${dicoFiles}
      ${CMAKE_CURRENT_SOURCE_DIR}/default-eng.txt
      ${PROJECT_SOURCE_DIR}/scripts/convert-ud-to-dic.py
    COMMENT "produce eng XML dico under Linux"
    VERBATIM
  )
else()
  # WARNING: VERBATIM option add unintentional double quotes symbols in XML file
  add_custom_command(
    OUTPUT dico.xml  freeling-ext-lima.txt default-eng.ud.dat
    COMMAND echo ^<dictionary^> > dico.xml.tmp
    COMMAND python ${PROJECT_SOURCE_DIR}/scripts/convert-ud-to-dic.py --features=none --output=freeling-ext-lima.txt.tmp ${dicoFiles}
    COMMAND sort -u -o freeling-ext-lima.txt freeling-ext-lima.txt.tmp
    COMMAND perl ${PROJECT_SOURCE_DIR}/scripts/xmlforms.pl freeling-ext-lima.txt dico.xml.tmp
    COMMAND echo ^</dictionary^> >> dico.xml.tmp
    COMMAND perl ${PROJECT_SOURCE_DIR}/scripts/cmakeconvertdefautjys.pl ${CMAKE_CURRENT_SOURCE_DIR}/default-eng.txt ${CMAKE_CURRENT_BINARY_DIR}/convjys.txt default-eng.ud.dat
    COMMAND mv dico.xml.tmp dico.xml
    DEPENDS
      ${dicoFiles}
      ${CMAKE_CURRENT_SOURCE_DIR}/default-eng.txt
      ${PROJECT_SOURCE_DIR}/scripts/convert-ud-to-dic.py
    COMMENT "produce eng XML dico under Windows"
  )
endif()

add_custom_target(
  dicoxmleng.ud
  ALL
  DEPENDS dico.xml
)
add_dependencies(dicoxmleng.ud codeeng.ud )

add_custom_target(
  converteng.ud
  ALL
)
add_dependencies(converteng.ud dicoxmleng.ud )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/default-eng.ud.dat COMPONENT eng DESTINATION share/apps/lima/resources/LinguisticProcessings/eng.ud)
