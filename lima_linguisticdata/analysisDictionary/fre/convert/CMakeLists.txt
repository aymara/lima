# Copyright 2002-2020 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

set(ENV{LC_ALL} "C")

file(GLOB dicoFiles ${CMAKE_CURRENT_SOURCE_DIR}/../lefff/*.dic)

if (NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  add_custom_command(
    OUTPUT dico.xml forms.dic forms.dic.2 forms.dic.3 default-fre.dat error.txt
    COMMAND cat ${CMAKE_CURRENT_SOURCE_DIR}/../../opening_tag.txt > dico.xml.tmp
    COMMAND cat ${dicoFiles} > forms.dic
    COMMAND python3 ${PROJECT_SOURCE_DIR}/scripts/merge-dico-poscorpus.py fre forms.dic ${PROJECT_SOURCE_DIR}/disambiguisationMatrices/fre/corpus/corpus_fre.txt forms.dic.2
    COMMAND LC_ALL="C" sort -u -o forms.dic.3 forms.dic.2
    COMMAND perl ${PROJECT_SOURCE_DIR}/scripts/xmlforms.pl forms.dic.3 dico.xml.tmp
    COMMAND cat ${CMAKE_CURRENT_SOURCE_DIR}/../../closing_tag.txt >> dico.xml.tmp
    COMMAND perl ${PROJECT_SOURCE_DIR}/scripts/cmakeconvertdefautjys.pl ${CMAKE_CURRENT_SOURCE_DIR}/default-fre.txt ../code/convjys.txt default-fre.dat
    COMMAND mv dico.xml.tmp dico.xml
    DEPENDS
      ${dicoFiles}
      ${CMAKE_CURRENT_SOURCE_DIR}/default-fre.txt
      ${PROJECT_SOURCE_DIR}/disambiguisationMatrices/fre/corpus/corpus_fre.txt
      ${PROJECT_SOURCE_DIR}/scripts/merge-dico-poscorpus.py
    COMMENT "produce fre XML dico under Linux"
    VERBATIM
  )
else()
  # WARNING: VERBATIM option add unintentional double quotes symbols in XML file
  add_custom_command(
    OUTPUT dico.xml forms.dic forms.dic.2 forms.dic.3 default-fre.dat error.txt
    COMMAND cat ${CMAKE_CURRENT_SOURCE_DIR}/../../opening_tag.txt > dico.xml.tmp
    COMMAND cat ${dicoFiles} > forms.dic
    COMMAND python ${PROJECT_SOURCE_DIR}/scripts/merge-dico-poscorpus.py fre forms.dic ${PROJECT_SOURCE_DIR}/disambiguisationMatrices/fre/corpus/corpus_fre.txt forms.dic.2
    # WARNING: the sort command here must be a POSIX one. If you get failures
    # here without clear error messages, verify that the sort command used is
    # not the Windows one.
    COMMAND sort -u -o forms.dic.3 forms.dic.2
    COMMAND perl ${PROJECT_SOURCE_DIR}/scripts/xmlforms.pl forms.dic.3 dico.xml.tmp
    COMMAND cat ${CMAKE_CURRENT_SOURCE_DIR}/../../closing_tag.txt >> dico.xml.tmp
    COMMAND perl ${PROJECT_SOURCE_DIR}/scripts/cmakeconvertdefautjys.pl ${CMAKE_CURRENT_SOURCE_DIR}/default-fre.txt ../code/convjys.txt default-fre.dat
    COMMAND mv dico.xml.tmp dico.xml
    DEPENDS
      ${dicoFiles}
      ${CMAKE_CURRENT_SOURCE_DIR}/default-fre.txt
      ${PROJECT_SOURCE_DIR}/disambiguisationMatrices/fre/corpus/corpus_fre.txt
      ${PROJECT_SOURCE_DIR}/scripts/merge-dico-poscorpus.py
    COMMENT "produce fre XML dico under Windows"
  )
endif()

add_custom_target(
  dicoxmlfre
  ALL
  DEPENDS dico.xml
)
# add_dependencies(dicoxmlfre leffffre )
add_dependencies(dicoxmlfre codefre )

add_custom_target(
  convertfre
  ALL
)
add_dependencies(convertfre dicoxmlfre )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/default-fre.dat COMPONENT fre DESTINATION share/apps/lima/resources/LinguisticProcessings/fre)
