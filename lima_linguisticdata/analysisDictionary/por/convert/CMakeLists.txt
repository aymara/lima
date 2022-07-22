# Copyright 2002-2020 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

set(ENV{LC_ALL} "C")

file(GLOB dicoFiles ${CMAKE_CURRENT_SOURCE_DIR}/../delaf/*.txt)

if (NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  add_custom_command(
    OUTPUT dico.xml forms.dic default-por.dat
    COMMAND cat ${CMAKE_CURRENT_SOURCE_DIR}/../../opening_tag.txt > dico.xml.tmp
    COMMAND cat ${dicoFiles} > forms.dic
    COMMAND python3 ${PROJECT_SOURCE_DIR}/scripts/merge-dico-poscorpus.py por forms.dic ${PROJECT_SOURCE_DIR}/disambiguisationMatrices/por/corpus/macmorpho.conll.txt forms.dic.2
    COMMAND LC_ALL="C" sort -u -o forms.dic.3 forms.dic.2
    COMMAND perl ${PROJECT_SOURCE_DIR}/scripts/xmlforms.pl -desacc=no ${CMAKE_CURRENT_SOURCE_DIR}/dicoponctu.txt dico.xml.tmp
    COMMAND perl ${PROJECT_SOURCE_DIR}/scripts/xmlforms.pl forms.dic.3 dico.xml.tmp
    COMMAND cat ${CMAKE_CURRENT_SOURCE_DIR}/../../closing_tag.txt >> dico.xml.tmp
    COMMAND perl ${PROJECT_SOURCE_DIR}/scripts/cmakeconvertdefautjys.pl ${CMAKE_CURRENT_SOURCE_DIR}/default-por.txt ../code/convjys.txt default-por.dat
    COMMAND mv dico.xml.tmp dico.xml
    DEPENDS
      ${dicoFiles}
      ${CMAKE_CURRENT_SOURCE_DIR}/default-por.txt
      ${CMAKE_CURRENT_SOURCE_DIR}/dicoponctu.txt
      ${PROJECT_SOURCE_DIR}/disambiguisationMatrices/por/corpus/macmorpho.conll.txt
      ${PROJECT_SOURCE_DIR}/scripts/merge-dico-poscorpus.py
    COMMENT "produce por XML dico under Linux"
    VERBATIM
  )
else()
  # WARNING: VERBATIM option add unintentional double quotes symbols in XML file
  add_custom_command(
    OUTPUT dico.xml  forms.dic default-por.dat
    COMMAND cat ${CMAKE_CURRENT_SOURCE_DIR}/../../opening_tag.txt > dico.xml.tmp
    COMMAND cat ${dicoFiles} > forms.dic
    COMMAND python ${PROJECT_SOURCE_DIR}/scripts/merge-dico-poscorpus.py por forms.dic ${PROJECT_SOURCE_DIR}/disambiguisationMatrices/por/corpus/macmorpho.conll.txt forms.dic.2
    COMMAND sort -u -o forms.dic.3 forms.dic.2
    COMMAND perl ${PROJECT_SOURCE_DIR}/scripts/xmlforms.pl -desacc=no ${CMAKE_CURRENT_SOURCE_DIR}/dicoponctu.txt dico.xml.tmp
    COMMAND perl ${PROJECT_SOURCE_DIR}/scripts/xmlforms.pl forms.dic.3 dico.xml.tmp
    COMMAND cat ${CMAKE_CURRENT_SOURCE_DIR}/../../closing_tag.txt >> dico.xml.tmp
    COMMAND perl ${PROJECT_SOURCE_DIR}/scripts/cmakeconvertdefautjys.pl ${CMAKE_CURRENT_SOURCE_DIR}/default-por.txt ../code/convjys.txt default-por.dat
    COMMAND mv dico.xml.tmp dico.xml
    DEPENDS
      ${dicoFiles}
      ${CMAKE_CURRENT_SOURCE_DIR}/default-por.txt
      ${CMAKE_CURRENT_SOURCE_DIR}/dicoponctu.txt
      ${PROJECT_SOURCE_DIR}/disambiguisationMatrices/por/corpus/macmorpho.conll.txt
      ${PROJECT_SOURCE_DIR}/scripts/merge-dico-poscorpus.py
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
