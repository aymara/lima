SET (qt-lima_MOC_HEADERS
  LimaGuiApplication.h
  Threads.h
  ConllParser.h
  
  config/LimaConfiguration.h
  config/ConfigurationTree.h
  config/ConfigurationTreeModel.h
  
  ConllListModel.h
  NamedEntitiesParser.h
  
  tools/extract/FileTextExtractor.h
  
)

qt5_wrap_cpp (qt-lima_MOC_OUTFILES ${qt-lima_MOC_HEADERS})

SET (qt-lima_LIB_SRCS 
  LimaGuiApplication.cpp
  Threads.cpp
  ConllParser.cpp
  
  config/LimaConfiguration.cpp
  config/ConfigurationTree.cpp
  config/ConfigurationTreeModel.cpp
  
  ConllListModel.cpp
  NamedEntitiesParser.cpp
  
  tools/extract/FileTextExtractor.cpp
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
  lima-common-xmlconfigurationfiles
  lima-linguisticprocessing
  ${Qt5_LIBRARIES}
)

install(TARGETS qt-lima DESTINATION lib)
install(TARGETS lima_gui DESTINATION bin)
