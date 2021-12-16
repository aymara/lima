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


################### tests ########################

########### next test ###############



add_test(NAME TestXmlReader0 COMMAND perl ${CMAKE_CURRENT_SOURCE_DIR}/test-positions.pl -language=fre -datadir=${CMAKE_CURRENT_BINARY_DIR}/data)
set_property(TEST TestXmlReader0 PROPERTY ENVIRONMENT "PATH=${CMAKE_BINARY_DIR}/lima_linguisticprocessing/test:$ENV{PATH};LIMA_CONF=${CMAKE_BINARY_DIR}/execEnv/config;LIMA_RESOURCES=${CMAKE_BINARY_DIR}/execEnv/resources;LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}/execEnv/lib:$ENV{LD_LIBRARY_PATH}")

add_custom_target(
  TestXmlReader0
  ALL
)
add_custom_command(TARGET TestXmlReader0 PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_CURRENT_SOURCE_DIR}/data ${CMAKE_CURRENT_BINARY_DIR}/data)
