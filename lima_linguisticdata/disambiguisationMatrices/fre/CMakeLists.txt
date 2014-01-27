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
