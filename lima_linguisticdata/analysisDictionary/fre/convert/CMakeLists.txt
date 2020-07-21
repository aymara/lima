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

# [1] Use a dictionary already in LIMA format to avoid the long conversion of
# the source extensional Lefff. This will have to be changed in the future
# because it will make difficult to transmit changes and corrections upstream
# Continuation of [1] above
#   COMMAND ${PROJECT_SOURCE_DIR}/scripts/convert-lefff-tags-to-lima-std.pl \
#           ${CMAKE_CURRENT_SOURCE_DIR}/convlefffstd.txt \
#           ../lefff/lefff-ext-simplified.txt > forms.dic
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
