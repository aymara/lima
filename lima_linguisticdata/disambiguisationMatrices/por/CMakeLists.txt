# Copyright 2002-2020 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

add_subdirectory(corpus)

add_custom_command(
  OUTPUT succession_categs_retag.txt
  COMMAND cat ${CMAKE_CURRENT_SOURCE_DIR}/corpus/macmorpho.conll.txt | gawk -F "	" "{print $2}" > succession_categs_retag.txt
  COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/corpus/macmorpho.conll.txt corpus_por_merge.txt

  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/corpus/macmorpho.conll.txt
  COMMENT "produce por categs succession"
  VERBATIM
)

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
