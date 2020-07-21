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


find_program (SVMT_LEARN SVMTlearn)
find_program (SVM_LIGHT_PROGRAM svm_learn)

if ( ${SVMT_LEARN} STREQUAL "SVMT_LEARN-NOTFOUND" )

  message("SVMTlearn not found. eng.ud SVMTool model will not build.")

elseif ( ${SVM_LIGHT_PROGRAM} STREQUAL "SVM_LIGHT_PROGRAM-NOTFOUND" )

  message("svm_light not found. eng.ud SVMTool model will not build.")

else ()

  message("SVMTlearn is ${SVMT_LEARN}.")
  message("svm_light is ${SVM_LIGHT_PROGRAM}.")

  get_filename_component(SVM_LIGHT ${SVM_LIGHT_PROGRAM} DIRECTORY)

  configure_file(config-minimale.SVMT config-minimale.SVMT @ONLY)

  # Create a file allowing LIMA to find the resource dir
  file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/lima "")


  if (SHORTEN_POR_CORPUS_FOR_SVMLEARN)
    add_custom_command(
        OUTPUT lima.A0.UNK
        COMMAND sed -e "s@ @_@g" -e "s@\t@ @g" ${CMAKE_CURRENT_BINARY_DIR}/../corpus_eng.ud_merge.txt | head -n 100000 > corpus_eng.ud.txt
        COMMAND PERL5LIB=$ENV{PERL5LIB} ${SVMT_LEARN} config-minimale.SVMT
        DEPENDS
          config-minimale.SVMT
          ${CMAKE_CURRENT_BINARY_DIR}/../corpus_eng.ud_merge.txt
        COMMENT "Compiles eng.ud SVMTool model"
        VERBATIM
      )

    add_custom_target(
      svmtoolmodeleng.ud
      ALL
      DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/lima.A0.UNK
    )
  else()
  add_custom_command(
      OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/lima.A0.UNK
      COMMAND sed -e "s@ @_@g" -e "s@\t@ @g" ${CMAKE_CURRENT_BINARY_DIR}/../corpus_eng.ud_merge.txt > corpus_eng.ud.txt
      COMMAND echo "Before calling ${SVMT_LEARN}, PERL5LIB is $ENV{PERL5LIB}"
      COMMAND PERL5LIB=$ENV{PERL5LIB} ${SVMT_LEARN} config-minimale.SVMT
      DEPENDS
        config-minimale.SVMT
        ${CMAKE_CURRENT_BINARY_DIR}/../corpus_eng.ud_merge.txt
      COMMENT "Compiles eng.ud SVMTool model"
      VERBATIM
    )

    add_custom_target(
      svmtoolmodeleng.ud
      ALL
      DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/lima.A0.UNK
    )

  endif()


    add_dependencies(svmtoolmodeleng.ud categs-eng.ud)

    install(
    FILES
      ${CMAKE_CURRENT_BINARY_DIR}/config-minimale.SVMT
    COMPONENT eng.ud
    DESTINATION share/apps/lima/resources/Disambiguation/SVMToolModel-eng.ud)

    install(
    CODE "file( GLOB _SVMToolModelFiles \"${CMAKE_CURRENT_BINARY_DIR}/lima*\" )"
    CODE "file( INSTALL \${_SVMToolModelFiles} DESTINATION \"${CMAKE_INSTALL_PREFIX}/share/apps/lima/resources/Disambiguation/SVMToolModel-eng.ud\" )"
    COMPONENT eng.ud)

endif()
