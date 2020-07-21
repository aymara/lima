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
add_subdirectory(corpus)

# file(GLOB DP_FILES $ENV{NLTK_PTB_DP_DIR}/*.dp)

if (NOT DEFINED ENV{NLTK_PTB_DP_FILE})
  message( FATAL_ERROR "Envirorment variable NLTK_PTB_DP_FILE must be defined.")
endif()

add_custom_command(
  OUTPUT succession_categs_retag.txt corpus_eng_merge.txt
  COMMAND gawk -F "\\t" "BEGIN {OFS = FS} {print $1\"\\t\"$2}" $ENV{NLTK_PTB_DP_FILE} > corpus_eng_merge.txt
  COMMAND patch -p 0 corpus_eng_merge.txt ${CMAKE_CURRENT_SOURCE_DIR}/corpus/PennTreeBankExtractForLima.patch | cat
#  COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/corpus/oanc.pos corpus_eng_merge.txt
#  COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/corpus/ptb.pos corpus_eng_merge.txt
#   COMMAND gawk -F "	" 'BEGIN {OFS = FS} {print $1"	"$2}' ${DP_FILES} > corpus_eng_merge.txt
#   COMMAND cat ${DP_FILES} > corpus_eng_merge.txt
  COMMAND cat corpus_eng_merge.txt | gawk -F "	" "{print $2}" > succession_categs_retag.txt

#   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/corpus/oanc.pos
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/corpus/PennTreeBankExtractForLima.patch $ENV{NLTK_PTB_DP_FILE}
  COMMENT "produce eng categs succession"
  VERBATIM
)

add_custom_target(
  categs-eng
  ALL
  DEPENDS succession_categs_retag.txt corpus_eng_merge.txt
)

DISAMBMATRICES(eng succession_categs_retag.txt ${CMAKE_CURRENT_SOURCE_DIR}/code_symbolic2lima.txt ${PROJECT_SOURCE_DIR}/scripts/disamb_matrices_prior-convert.pl X,SYM,UNK,PROPN,CONJ,NUM)

add_dependencies(trigrammatrix-eng categs-eng)

install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/corpus_eng_merge.txt
  DESTINATION share/apps/lima/resources/Disambiguation
)
