# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

################### tests ########################

########### next test ###############

configure_file(test-config-file.xml test-config-file.xml COPYONLY)

SET(XMLConfigurationFilesTest0_SRCS
   XMLConfigurationFilesTest0.cpp
)

add_executable(XMLConfigurationFilesTest0 ${XMLConfigurationFilesTest0_SRCS})

target_link_libraries(XMLConfigurationFilesTest0
  lima-common-xmlconfigurationfiles
  Boost::unit_test_framework
)

add_test(
  NAME XMLConfigurationFilesTest0
  COMMAND XMLConfigurationFilesTest0
)


