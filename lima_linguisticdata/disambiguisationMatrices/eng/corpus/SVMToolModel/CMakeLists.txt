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

  message("SVMTlearn not found. eng SVMTool model will not build.")
  
elseif ( ${SVM_LIGHT_PROGRAM} STREQUAL "SVM_LIGHT_PROGRAM-NOTFOUND" )

  message("svm_light not found. eng SVMTool model will not build.")
  
else ()
  message("SVMTlearn is ${SVMT_LEARN}.")
  message("svm_light is ${SVM_LIGHT_PROGRAM}.")
  
  get_filename_component(SVM_LIGHT ${SVM_LIGHT_PROGRAM} DIRECTORY)
  
  configure_file(config-minimale.SVMT config-minimale.SVMT @ONLY)

  # Create a file allowing LIMA to find the resource dir
  file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/lima "")

  add_custom_command(
      OUTPUT lima.A0.UNK
      COMMAND sed -e "s@ @_@g" -e "s@\t@ @g" ${CMAKE_CURRENT_BINARY_DIR}/../../corpus_eng_merge.txt > corpus_eng.txt
      COMMAND echo "Before calling ${SVMT_LEARN}, PERL5LIB is $ENV{PERL5LIB}"
      COMMAND ${SVMT_LEARN} config-minimale.SVMT
      DEPENDS 
        config-minimale.SVMT
        ${CMAKE_CURRENT_SOURCE_DIR}/../PennTreeBankExtractForLima.patch
        ${CMAKE_CURRENT_BINARY_DIR}/../../corpus_eng_merge.txt
      COMMENT "Compiles eng SVMTool model"
      VERBATIM
    )

    add_custom_target(
      svmtoolmodeleng
      ALL
      DEPENDS lima.A0.UNK
    )

    add_dependencies(svmtoolmodeleng categs-eng)

    install(
    FILES 
      ${CMAKE_CURRENT_BINARY_DIR}/config-minimale.SVMT  
      ${CMAKE_CURRENT_BINARY_DIR}/lima
      ${CMAKE_CURRENT_BINARY_DIR}/lima.A0              
      ${CMAKE_CURRENT_BINARY_DIR}/lima.A0.UNK  
      ${CMAKE_CURRENT_BINARY_DIR}/lima.A1     
      ${CMAKE_CURRENT_BINARY_DIR}/lima.A1.UNK  
      ${CMAKE_CURRENT_BINARY_DIR}/lima.A2      
      ${CMAKE_CURRENT_BINARY_DIR}/lima.A2.UNK  
      ${CMAKE_CURRENT_BINARY_DIR}/lima.A3      
      ${CMAKE_CURRENT_BINARY_DIR}/lima.A3.UNK  
      ${CMAKE_CURRENT_BINARY_DIR}/lima.A4      
      ${CMAKE_CURRENT_BINARY_DIR}/lima.A4.UNK  
      ${CMAKE_CURRENT_BINARY_DIR}/lima.AMBP  
      ${CMAKE_CURRENT_BINARY_DIR}/lima.DICT  
      ${CMAKE_CURRENT_BINARY_DIR}/lima.EXP   
      ${CMAKE_CURRENT_BINARY_DIR}/lima.M0.LR.MRG      
      ${CMAKE_CURRENT_BINARY_DIR}/lima.UNK.M0.LR.MRG
      ${CMAKE_CURRENT_BINARY_DIR}/lima.UNKP
      ${CMAKE_CURRENT_BINARY_DIR}/lima.WIN
    COMPONENT eng 
    DESTINATION share/apps/lima/resources/Disambiguation/SVMToolModel-eng)

endif()
