SET (qt-lima_MOC_HEADERS
  Writer.h
  ConllParser.h
  TextAnalyzer.h
)

qt5_wrap_cpp (qt-lima_MOC_OUTFILES ${qt-lima_MOC_HEADERS})

SET (qt-lima_LIB_SRCS 
  Writer.cpp
  ConllParser.cpp
  TextAnalyzer.cpp
)

add_library(qt-lima SHARED 
  ${qt-lima_LIB_SRCS} 
  ${qt-lima_MOC_OUTFILES}
)
target_link_libraries(qt-lima 
  ${Qt5_LIBRARIES}
)

set (lima_gui_SRCS
  main.cpp
)
qt5_add_resources(lima_gui_SRCS resources.qrc)

add_executable(lima_gui 
  ${lima_gui_SRCS}
)
target_link_libraries(lima_gui 
  qt-lima 
  lima-common
  lima-lp-client
  lima-lp-analysishandlers
  lima-common-mediaticdata
  lima-common-factory
  lima-common-tools
  lima-linguisticprocessing
  ${Qt5_LIBRARIES}
)

install(TARGETS qt-lima DESTINATION lib)
install(TARGETS lima_gui DESTINATION bin)
