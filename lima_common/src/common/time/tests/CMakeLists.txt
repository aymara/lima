
################### tests ########################

########### next test ###############

SET(TimeUtilsTest0_SRCS
   TimeUtilsTest0.cpp
)

add_executable(TimeUtilsTest0 ${TimeUtilsTest0_SRCS})

target_link_libraries(TimeUtilsTest0 lima-common ${Boost_LIBRARIES} ${QT_LIBRARIES})

add_test(NAME TimeUtilsTest0 COMMAND TimeUtilsTest0)

