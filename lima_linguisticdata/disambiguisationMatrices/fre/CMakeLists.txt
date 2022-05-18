# Copyright 2002-2020 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

add_subdirectory(corpus)

add_custom_command(
  OUTPUT succession_categs_retag.txt
  COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/corpus/corpus_fre.txt corpus_fre_merge.txt
  COMMAND cat corpus_fre_merge.txt | gawk -F "	" "{print $2}" > succession_categs_retag.txt
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/corpus/corpus_fre.txt
  COMMENT "produce fre categs succession"
  VERBATIM
)

add_custom_target(
  categs-fre
  ALL
  DEPENDS succession_categs_retag.txt
)

DISAMBMATRICES(fre succession_categs_retag.txt ${CMAKE_CURRENT_SOURCE_DIR}/code_symbolic2lima.txt ${PROJECT_SOURCE_DIR}/scripts/disamb_matrices_prior-convert.pl U,ET,PREF,NPP,PONCT,CC,CS)

add_dependencies(trigrammatrix-fre categs-fre)

install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/corpus_fre_merge.txt
  DESTINATION share/apps/lima/resources/Disambiguation
)
