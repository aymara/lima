################### tests ########################

########### next test ###############

SET(StringsPoolTest0_SRCS 
  StringsPoolTest0.cpp
)

add_executable(StringsPoolTest0 ${StringsPoolTest0_SRCS})

target_link_libraries(StringsPoolTest0 lima-common-misc lima-common-tools ${Boost_LIBRARIES} ${Qt5_LIBRARIES})

add_test(NAME StringsPoolTest0 COMMAND StringsPoolTest0)
