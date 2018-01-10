

################### tests ########################

########### next test ###############

configure_file(test-config-file.xml test-config-file.xml COPYONLY)

SET(XMLConfigurationFilesTest0_SRCS
   XMLConfigurationFilesTest0.cpp
)

add_executable(XMLConfigurationFilesTest0 ${XMLConfigurationFilesTest0_SRCS})

target_link_libraries(XMLConfigurationFilesTest0 
  lima-common
  lima-common-xmlconfigurationfiles 
  ${Boost_LIBRARIES} 
  ${QT_LIBRARIES}
)

add_test(
  NAME XMLConfigurationFilesTest0 
  COMMAND XMLConfigurationFilesTest0 -v2 -nocrashhandler
)


