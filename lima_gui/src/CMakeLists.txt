add_subdirectory(treeview)

########### next target ###############

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
  lima-common-mediaticdata
  lima-common-xmlconfigurationfiles
  lima-common-tools
  lima-common-data 
  lima-common-misc
  lima-common
  lima-lp-annotationgraph
  lima-lp-lineartextrepresentation
  lima-lp-misc
  lima-lp-tgv
  lima-lp-linguisticdata
  lima-lp-bagofwords
  lima-lp-propertycode
  lima-lp-regexmatcher
  lima-lp-wordsensetagger
  lima-lp-linguisticprocessing-core
  lima-lp-analysisdict
  lima-lp-eventanalyzer
  lima-lp-corefsolver
  lima-lp-specificentities
  lima-lp-syntacticanalysis
  lima-lp-morphologicanalysis
  lima-lp-dictionary
  lima-lp-easyxmldumper
  lima-lp-analysisdumpers
  lima-lp-textsegmentation
  lima-lp-linguisticanalysisstructure
  lima-lp-semanticanalysis
  lima-lp-linguisticresources
  lima-lp-entitytracker
  lima-lp-flattokenizer
  lima-lp-applyrecognizer
  lima-lp-linguisticprocessors
  lima-lp-postagger
  lima-lp-automaton
  lima-lp-analysishandlers
  lima-lp-client
  ${Qt5_LIBRARIES}
)

install(TARGETS qt-lima DESTINATION lib
  COMPONENT runtime)

########### next target ###############

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

install(TARGETS lima_gui DESTINATION bin
  COMPONENT runtime)

########### install files ###############
install(PROGRAMS lima_gui.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
install(PROGRAMS qml/resources/lima_gui.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor)
