################### tests ########################

########### next test ###############

SET(StringsPoolTest0_SRCS 
  StringsPoolTest0.cpp
)

add_executable(StringsPoolTest0 ${StringsPoolTest0_SRCS})

target_link_libraries(StringsPoolTest0 lima-common-misc lima-common-qslog ${Boost_LIBRARIES} ${QT_LIBRARIES})

add_test(StringsPoolTest0 StringsPoolTest0)
