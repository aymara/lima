# Copyright 2002-2020 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

################### tests ########################

########### next test ###############

# TODO Enable again this test when the switch from svm_light to TinySVM is complete

# if(SVMTOOL++_FOUND)
#
# add_test(NAME TestXmlReader0 COMMAND perl ${CMAKE_CURRENT_SOURCE_DIR}/test-positions.pl -language=fre -datadir=${CMAKE_CURRENT_BINARY_DIR}/data)
# set_tests_properties(TestXmlReader0 PROPERTIES ENVIRONMENT "PATH=${CMAKE_BINARY_DIR}/lima_linguisticprocessing/test:$ENV{PATH};LIMA_CONF=${CMAKE_BINARY_DIR}/execEnv/config;LIMA_RESOURCES=${CMAKE_BINARY_DIR}/execEnv/resources;LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}/execEnv/lib:$ENV{LD_LIBRARY_PATH}")
#
# add_custom_target(
#   TestXmlReader0
#   ALL
# )
# add_custom_command(TARGET TestXmlReader0 PRE_BUILD
#                    COMMAND ${CMAKE_COMMAND} -E copy_directory
#                        ${CMAKE_CURRENT_SOURCE_DIR}/data ${CMAKE_CURRENT_BINARY_DIR}/data)
# else()
#   message(STATUS "Could not find SVMTool++. XML reader tests will not be executed as its results depend on tokens' data.")
# endif()
