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
if (BUILD_SVMTOOL_MODELS)
  add_subdirectory(SVMToolModel)
endif()

# Use the --features option of convert-ud-to-success-categ-retag.py to change
# the list of UD features kept from the source UD corpus
add_custom_command(
  OUTPUT succession_categs_retag.txt corpus_eng.ud_merge.txt
  COMMAND ${PROJECT_SOURCE_DIR}/scripts/convert-ud-to-success-categ-retag.py --features=none --output=corpus_eng.ud_merge.txt ${CMAKE_CURRENT_SOURCE_DIR}/UD_English-EWT/en_ewt-ud-train.conllu
  COMMAND cat corpus_eng.ud_merge.txt | gawk -F "\\t" "{print \$2}" > succession_categs_retag.txt
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/UD_English-EWT/en_ewt-ud-train.conllu
  COMMENT "produce eng categs succession"
  VERBATIM
)

add_custom_target(
  categs-eng.ud
  ALL
  DEPENDS succession_categs_retag.txt corpus_eng.ud_merge.txt
)

DISAMBMATRICES(
  eng.ud
  succession_categs_retag.txt
  ${CMAKE_CURRENT_SOURCE_DIR}/code_symbolic2lima.txt
  ${PROJECT_SOURCE_DIR}/scripts/disamb_matrices_prior-convert.pl
  X,SYM,UNK,PROPN,CONJ,NUM
)

add_dependencies(trigrammatrix-eng.ud categs-eng.ud)

install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/corpus_eng.ud_merge.txt
  DESTINATION share/apps/lima/resources/Disambiguation
)
