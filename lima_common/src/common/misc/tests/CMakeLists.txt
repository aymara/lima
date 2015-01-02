################### tests ########################

########### next test ###############

SET(StringsPoolTest0_SRCS 
  StringsPoolTest0.cpp
)

add_executable(StringsPoolTest0 ${StringsPoolTest0_SRCS})

target_link_libraries(StringsPoolTest0 lima-common-misc lima-common-qslog ${Boost_LIBRARIES} ${QT_LIBRARIES})

add_test(StringsPoolTest0 StringsPoolTest0)

########### next test ###############
set (LimaFileSystemWatcherTest0_MOC_HEADERS LimaFileSystemWatcherTest.h)
qt4_wrap_cpp (LimaFileSystemWatcherTest0_MOC_OUTFILES ${LimaFileSystemWatcherTest0_MOC_HEADERS})


SET(LimaFileSystemWatcherTest0_SRCS 
  LimaFileSystemWatcherTest.cpp
)

add_executable(LimaFileSystemWatcherTest0 ${LimaFileSystemWatcherTest0_SRCS} ${LimaFileSystemWatcherTest0_MOC_OUTFILES})

target_link_libraries(LimaFileSystemWatcherTest0 lima-common ${Boost_LIBRARIES} ${QT_LIBRARIES})

add_test(LimaFileSystemWatcherTest0 LimaFileSystemWatcherTest0)

