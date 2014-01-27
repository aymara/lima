SET(QT_USE_QTXML 1)
INCLUDE(${QT_USE_FILE})

########### next target ###############

SET(annoqt_SRCS
           main.cpp
           annoqt.cpp
           annotationConfigurationHandler.cpp
           annotationEditWidget.cpp
           specificEntitiesHandler.cpp
           specificEntity.cpp)

set (annoqt_MOC_HEADERS annoqt.h
           annotationConfigurationHandler.h
           annotationEditWidget.h
           specificEntity.h)
qt4_wrap_cpp (annoqt_MOC_OUTFILES ${annoqt_MOC_HEADERS})

qt4_add_resources(annoqt_QRCS annoqt.qrc)

add_executable(annoqt ${annoqt_SRCS} ${annoqt_QRCS} ${annoqt_MOC_OUTFILES})

target_link_libraries(annoqt ${QT_LIBRARIES})


########### next target ###############

SET(annoqtconfeditor_SRCS
           confeditor.cpp
           confeditormain.cpp
           annotationConfigurationHandler.cpp
           kxyselector.cpp
           kselector.cpp
           kcolorvalueselector.cpp
           kcolorcollection.cpp
           kcolormimedata.cpp
           khuesaturationselect.cpp
           kcolordialog.cpp)

set (annoqtconfeditor_MOC_HEADERS 
           confeditor.h
           annotationConfigurationHandler.h
           kxyselector.h
           kselector.h
           kcolorvalueselector.h
           khuesaturationselect.h
           kcolordialog.h)
qt4_wrap_cpp (annoqtconfeditor_MOC_OUTFILES ${annoqtconfeditor_MOC_HEADERS})

qt4_add_resources(annoqtconfeditor_QRCS annoqtconfeditor.qrc)

add_executable(annoqtconfeditor ${annoqtconfeditor_SRCS} ${annoqtconfeditor_QRCS} ${annoqtconfeditor_MOC_OUTFILES})

target_link_libraries(annoqtconfeditor ${QT_LIBRARIES})



########### install files ###############
install(TARGETS annoqt DESTINATION bin)
install(TARGETS annoqtconfeditor DESTINATION bin)
