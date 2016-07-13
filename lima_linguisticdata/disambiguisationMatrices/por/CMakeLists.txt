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

add_subdirectory(corpus)

if (NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  add_custom_command(
    OUTPUT succession_categs_retag.txt
    COMMAND cat ${CMAKE_CURRENT_SOURCE_DIR}/corpus/macmorpho.conll.txt | gawk -F "	" "{print $2}" > succession_categs_retag.txt
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/corpus/macmorpho.conll.txt corpus_por_merge.txt 

    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/corpus/macmorpho.conll.txt
    COMMENT "produce por categs succession"
    VERBATIM
  )
else ()
  add_custom_command(
    OUTPUT succession_categs_retag.txt
  # COMMAND c:\\msys64\\usr\\bin\\gawk -F "	" "BEGIN {OFS = FS} {print $1\"\\t\"$2}" $ENV{NLTK_PTB_DP_FILE} > corpus_por_merge.txt
    COMMAND C:\\MinGW\\msys\\1.0\\bin\\dos2unix -n ${CMAKE_CURRENT_SOURCE_DIR}/corpus/macmorpho.conll.txt corpus_por_merge.txt
    COMMAND C:\\MinGW\\msys\\1.0\\bin\\patch -p 0 corpus_por_merge.txt macmorpho.conll.txt | cat
  #  COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/corpus/oanc.pos corpus_por_merge.txt
  #  COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/corpus/ptb.pos corpus_por_merge.txt
  #   COMMAND gawk -F " " 'BEGIN {OFS = FS} {print $1"  "$2}' ${DP_FILES} > corpus_por_merge.txt
  #   COMMAND cat ${DP_FILES} > corpus_por_merge.txt
    COMMAND c:\\msys64\\usr\\bin\\cat corpus_por_merge.txt | gawk -F "	" "{print $2}" > succession_categs_retag.txt

  #   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/corpus/oanc.pos
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/corpus/macmorpho.conll.txt
    COMMENT "produce por categs succession"
    VERBATIM
  )
endif ()
add_custom_target(
  categs-por
  ALL
  DEPENDS succession_categs_retag.txt
)

DISAMBMATRICES(por succession_categs_retag.txt ${CMAKE_CURRENT_SOURCE_DIR}/code_symbolic2lima.txt ${PROJECT_SOURCE_DIR}/scripts/disamb_matrices_prior-convert.pl DIVERS_*,NOM_PROPRE*,CONJ_*,NOMBRE*)

add_dependencies(trigrammatrix-por categs-por)

install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/corpus_por_merge.txt
  DESTINATION share/apps/lima/resources/Disambiguation
)