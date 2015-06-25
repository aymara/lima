################### tests ########################

########### next test ###############
set (LimaFileSystemWatcherTest0_MOC_HEADERS LimaFileSystemWatcherTest.h)
qt5_wrap_cpp (LimaFileSystemWatcherTest0_MOC_OUTFILES ${LimaFileSystemWatcherTest0_MOC_HEADERS})


SET(LimaFileSystemWatcherTest0_SRCS 
  LimaFileSystemWatcherTest.cpp
)

add_executable(LimaFileSystemWatcherTest0 ${LimaFileSystemWatcherTest0_SRCS} ${LimaFileSystemWatcherTest0_MOC_OUTFILES})

target_link_libraries(LimaFileSystemWatcherTest0 lima-common ${Boost_LIBRARIES} ${QT_LIBRARIES})

add_test(LimaFileSystemWatcherTest0 LimaFileSystemWatcherTest0)
set_tests_properties(LimaFileSystemWatcherTest0 PROPERTIES ENVIRONMENT LIMA_CONF=${CMAKE_SOURCE_DIR}/conf)


